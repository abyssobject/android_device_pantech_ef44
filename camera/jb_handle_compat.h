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
 * ef44: the JB Pantech camera HAL reads gralloc's private_handle_t with
 * bufferType placed before size/offset (QCOM_BSP_CAMERA_ABI_HACK layout),
 * while the Adreno blobs need the regular layout. Hand the HAL a proxy
 * preview window whose dequeued handles are copies in the JB order, and
 * map them back to the real buffers on enqueue/cancel/lock.
 */

#include <string.h>
#include <cutils/native_handle.h>

#define JB_MAX_BUFFERS 32

/* index of size/offset/bufferType in data[] (fd, fd_metadata, magic, flags, ...) */
#define N_IDX_SIZE        4
#define N_IDX_OFFSET      5
#define N_IDX_BUFFERTYPE  6

struct jb_slot {
    buffer_handle_t *orig;      /* the window's own buffer_handle_t storage */
    buffer_handle_t jb;         /* what the HAL sees */
    native_handle_t *copy;
};

struct jb_window {
    preview_stream_ops_t ops;   /* must stay first: the HAL gets &ops */
    preview_stream_ops_t *real;
    android::Mutex lock;
    jb_slot slots[JB_MAX_BUFFERS];
};

static jb_window *to_jb(preview_stream_ops_t *w)
{
    return reinterpret_cast<jb_window *>(w);
}

static void jb_reset(jb_window *jw)
{
    for (int i = 0; i < JB_MAX_BUFFERS; i++) {
        free(jw->slots[i].copy);
        memset(&jw->slots[i], 0, sizeof(jw->slots[i]));
    }
}

/* returns the window's handle storage for a HAL handle pointer */
static buffer_handle_t *jb_unwrap(jb_window *jw, buffer_handle_t *buffer)
{
    for (int i = 0; i < JB_MAX_BUFFERS; i++)
        if (&jw->slots[i].jb == buffer)
            return jw->slots[i].orig;
    return buffer;
}

static int jb_dequeue_buffer(preview_stream_ops_t *w,
        buffer_handle_t **buffer, int *stride)
{
    jb_window *jw = to_jb(w);
    buffer_handle_t *orig = NULL;
    int rc = jw->real->dequeue_buffer(jw->real, &orig, stride);
    if (rc || !orig || !*orig) {
        *buffer = orig;
        return rc;
    }

    android::Mutex::Autolock l(jw->lock);
    const native_handle_t *src = *orig;
    jb_slot *slot = NULL, *free_slot = NULL;
    for (int i = 0; i < JB_MAX_BUFFERS; i++) {
        if (jw->slots[i].orig && *jw->slots[i].orig == *orig) {
            slot = &jw->slots[i];
            break;
        }
        if (!jw->slots[i].orig && !free_slot)
            free_slot = &jw->slots[i];
    }
    if (!slot)
        slot = free_slot;
    if (!slot) {
        ALOGE("%s: out of handle slots", __FUNCTION__);
        *buffer = orig;
        return rc;
    }

    size_t sz = sizeof(native_handle_t) +
            sizeof(int) * (src->numFds + src->numInts);
    if (!slot->copy)
        slot->copy = (native_handle_t *)malloc(sz);
    memcpy(slot->copy, src, sz);
    const int *n = src->data;
    int *j = slot->copy->data;
    if (src->numFds == 2 && src->numInts > N_IDX_BUFFERTYPE) {
        j[N_IDX_SIZE] = n[N_IDX_BUFFERTYPE];
        j[N_IDX_OFFSET] = n[N_IDX_SIZE];
        j[N_IDX_BUFFERTYPE] = n[N_IDX_OFFSET];
    }
    slot->orig = orig;
    slot->jb = slot->copy;
    *buffer = &slot->jb;
    return rc;
}

static int jb_enqueue_buffer(preview_stream_ops_t *w, buffer_handle_t *buffer)
{
    jb_window *jw = to_jb(w);
    android::Mutex::Autolock l(jw->lock);
    return jw->real->enqueue_buffer(jw->real, jb_unwrap(jw, buffer));
}

static int jb_cancel_buffer(preview_stream_ops_t *w, buffer_handle_t *buffer)
{
    jb_window *jw = to_jb(w);
    android::Mutex::Autolock l(jw->lock);
    return jw->real->cancel_buffer(jw->real, jb_unwrap(jw, buffer));
}

static int jb_lock_buffer(preview_stream_ops_t *w, buffer_handle_t *buffer)
{
    jb_window *jw = to_jb(w);
    android::Mutex::Autolock l(jw->lock);
    if (!jw->real->lock_buffer)
        return 0;
    return jw->real->lock_buffer(jw->real, jb_unwrap(jw, buffer));
}

static int jb_set_buffer_count(preview_stream_ops_t *w, int count)
{
    jb_window *jw = to_jb(w);
    {
        android::Mutex::Autolock l(jw->lock);
        jb_reset(jw);
    }
    return jw->real->set_buffer_count(jw->real, count);
}

static int jb_set_buffers_geometry(preview_stream_ops_t *w, int width,
        int height, int format)
{
    return to_jb(w)->real->set_buffers_geometry(to_jb(w)->real, width,
            height, format);
}

static int jb_set_crop(preview_stream_ops_t *w, int left, int top,
        int right, int bottom)
{
    return to_jb(w)->real->set_crop(to_jb(w)->real, left, top, right, bottom);
}

static int jb_set_usage(preview_stream_ops_t *w, int usage)
{
    return to_jb(w)->real->set_usage(to_jb(w)->real, usage);
}

static int jb_set_swap_interval(preview_stream_ops_t *w, int interval)
{
    return to_jb(w)->real->set_swap_interval(to_jb(w)->real, interval);
}

static int jb_get_min_undequeued_buffer_count(const preview_stream_ops_t *w,
        int *count)
{
    jb_window *jw = to_jb(const_cast<preview_stream_ops_t *>(w));
    return jw->real->get_min_undequeued_buffer_count(jw->real, count);
}

static int jb_set_timestamp(preview_stream_ops_t *w, int64_t timestamp)
{
    return to_jb(w)->real->set_timestamp(to_jb(w)->real, timestamp);
}

/*
 * The JB HAL sends CAMERA_MSG_PREVIEW_FRAME data callbacks with a NULL
 * camera_memory_t (it never allocated callback buffers through the
 * framework's get_memory), and CameraHardwareInterface::__data_cb
 * dereferences it. Drop those; the preview itself goes to the window.
 */
static camera_data_callback jb_real_data_cb;

static void jb_data_cb(int32_t msg_type, const camera_memory_t *data,
        unsigned int index, camera_frame_metadata_t *metadata, void *user)
{
    if (!data || !data->handle)
        return;
    jb_real_data_cb(msg_type, data, index, metadata, user);
}

static camera_data_callback jb_wrap_data_cb(camera_data_callback cb)
{
    jb_real_data_cb = cb;
    return cb ? jb_data_cb : NULL;
}

/*
 * One proxy per real window, kept alive: when the app restarts the preview on
 * a new surface the HAL still returns buffers of the old window through the
 * old proxy, so its mapping must survive. NULL in, NULL out.
 */
#define JB_MAX_WINDOWS 8

static preview_stream_ops_t *jb_wrap_window(int id, preview_stream_ops_t *real)
{
    static jb_window windows[JB_MAX_WINDOWS];
    static int next;
    (void)id;
    if (!real)
        return real;

    jb_window *jw = NULL;
    for (int i = 0; i < JB_MAX_WINDOWS; i++)
        if (windows[i].real == real)
            jw = &windows[i];
    if (!jw) {
        jw = &windows[next];
        next = (next + 1) % JB_MAX_WINDOWS;
    }
    android::Mutex::Autolock l(jw->lock);
    if (jw->real != real)
        jb_reset(jw);
    jw->real = real;
    jw->ops.dequeue_buffer = jb_dequeue_buffer;
    jw->ops.enqueue_buffer = jb_enqueue_buffer;
    jw->ops.cancel_buffer = jb_cancel_buffer;
    jw->ops.set_buffer_count = jb_set_buffer_count;
    jw->ops.set_buffers_geometry = jb_set_buffers_geometry;
    jw->ops.set_crop = jb_set_crop;
    jw->ops.set_usage = jb_set_usage;
    jw->ops.set_swap_interval = jb_set_swap_interval;
    jw->ops.get_min_undequeued_buffer_count = jb_get_min_undequeued_buffer_count;
    jw->ops.lock_buffer = jb_lock_buffer;
    jw->ops.set_timestamp = jb_set_timestamp;
    return &jw->ops;
}
