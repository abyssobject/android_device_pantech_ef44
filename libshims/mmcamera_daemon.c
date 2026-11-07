/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * LD_PRELOADed into mm-qcamera-daemon (see init.qcom.rc).
 *
 * liboemcamera's post-processing (pp) thread polls up to four fds from a
 * pollfd array on its stack: the command pipe, the pp socket, and two
 * optional ones that are only added (and only counted in nfds) when they
 * are set. After poll() it nevertheless tests the revents of the last slot
 * unconditionally and, if POLLIN|POLLRDNORM is set there, read()s a 636 byte
 * command from the fd stored in the pp context (offset 0x1c). When that slot
 * was not polled its revents is whatever was left on the stack. With the
 * Android 12 bionic the leftover value regularly has those bits set; if
 * the other thread has meanwhile stored a pipe in that field, the pp thread
 * blocks for good in read() on an empty pipe, never acknowledges the next
 * mctl_pp_cmd(), and the camera preview and the torch hang (most often on
 * the first use after boot).
 *
 * Fix: for the poll() issued by that thread, clear revents of the slots
 * past nfds, so the unpolled slot reads as "no event" as the code expects.
 * Every other poll() in the daemon is passed through untouched.
 */

#define LOG_TAG "ef44-mmcamera-shim"

#include <dlfcn.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>

#include <log/log.h>

/* JB liboemcamera: the pp thread's poll() returns to mctl_pp_cmd + 0x143c */
#define PP_POLL_RET_FROM_MCTL_PP_CMD 0x143c
#define PP_POLL_SLOTS 4

static pthread_once_t sInitOnce = PTHREAD_ONCE_INIT;
static int (*sRealPoll)(struct pollfd *, nfds_t, int);
static uintptr_t sPpPollRet;

static void shim_init(void)
{
    void *h, *sym;

    sRealPoll = dlsym(RTLD_NEXT, "poll");

    h = dlopen("liboemcamera.so", RTLD_NOW | RTLD_NOLOAD);
    if (!h)
        return;
    sym = dlsym(h, "mctl_pp_cmd");
    if (sym)
        sPpPollRet = ((uintptr_t)sym & ~(uintptr_t)1) /* Thumb bit */
                + PP_POLL_RET_FROM_MCTL_PP_CMD;
    dlclose(h);
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    uintptr_t ra = (uintptr_t)__builtin_return_address(0) & ~(uintptr_t)1;
    nfds_t i;

    pthread_once(&sInitOnce, shim_init);

    if (sPpPollRet && ra == sPpPollRet && nfds < PP_POLL_SLOTS) {
        for (i = nfds; i < PP_POLL_SLOTS; i++)
            fds[i].revents = 0;
    }

    if (!sRealPoll) {
        errno = ENOSYS;
        return -1;
    }
    return sRealPoll(fds, nfds, timeout);
}
