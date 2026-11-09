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
 * rmt_storage's main loop is
 *
 *     for (;;) {
 *         select(os_params.max_fd + 1, &os_params.fds, NULL, NULL, NULL);
 *         qmi_csi_handle_event(service, &os_params);
 *     }
 *
 * passing QCSI's own fd set to select(), which overwrites it with the ready
 * ones. When the modem's server announcements (control port) arrive just
 * before its first remote storage request (server port), select() returns
 * the control port alone and the server socket drops out of the set for
 * good: the request is never read and the modem never finishes booting.
 *
 * Remember every descriptor the caller asked to read and ask for all of
 * them again on each call, as if the caller copied its set first (QCSI's
 * documented pattern). Descriptors that have been closed are forgotten.
 * Preloaded into rmt_storage only (with the mmap fix in rmt_mmap.c).
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <sys/select.h>
#include <time.h>

static pthread_mutex_t watched_lock = PTHREAD_MUTEX_INITIALIZER;
static fd_set watched;
static int watched_max = -1;

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout)
{
    struct timespec ts, *tsp = NULL;
    int fd;

    if (readfds && nfds > 0 && nfds <= FD_SETSIZE) {
        pthread_mutex_lock(&watched_lock);
        for (fd = 0; fd < nfds; fd++) {
            if (FD_ISSET(fd, readfds)) {
                FD_SET(fd, &watched);
                if (fd > watched_max)
                    watched_max = fd;
            }
        }
        for (fd = 0; fd <= watched_max; fd++) {
            if (!FD_ISSET(fd, &watched))
                continue;
            if (fcntl(fd, F_GETFD) < 0 && errno == EBADF) {
                FD_CLR(fd, &watched);
                FD_CLR(fd, readfds);
                continue;
            }
            FD_SET(fd, readfds);
            if (fd >= nfds)
                nfds = fd + 1;
        }
        pthread_mutex_unlock(&watched_lock);
    }

    /* rmt_storage waits without a timeout; one passed in is not updated */
    if (timeout) {
        ts.tv_sec = timeout->tv_sec;
        ts.tv_nsec = timeout->tv_usec * 1000;
        tsp = &ts;
    }
    return pselect(nfds, readfds, writefds, exceptfds, tsp, NULL);
}
