/*
 * Copyright (C) 2012, The CyanogenMod Project
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

/**
* @file CameraWrapper.cpp
*
* This file wraps a vendor camera module.
*
*/

//#define LOG_NDEBUG 0

#define LOG_TAG "CameraWrapper"
#include <cutils/log.h>

#include <utils/threads.h>
#include <utils/String8.h>
#include <hardware/hardware.h>
#include <hardware/camera.h>
#include <camera/Camera.h>
#include <camera/CameraParameters.h>

static android::Mutex gCameraWrapperLock;
static camera_module_t *gVendorModule = 0;

static int camera_device_open(const hw_module_t *module, const char *name,
        hw_device_t **device);
static int camera_get_number_of_cameras(void);
static int camera_get_camera_info(int camera_id, struct camera_info *info);

static struct hw_module_methods_t camera_module_methods = {
    open: camera_device_open
};

camera_module_t HAL_MODULE_INFO_SYM = {
    common: {
         tag: HARDWARE_MODULE_TAG,
         version_major: 1,
         version_minor: 0,
         id: CAMERA_HARDWARE_MODULE_ID,
         name: "Pantech MSM8960 Camera Wrapper",
         author: "Le Hoang, The CyanogenMod Project",
         methods: &camera_module_methods,
         dso: NULL, /* remove compilation warnings */
         reserved: {0}, /* remove compilation warnings */
    },
    get_number_of_cameras: camera_get_number_of_cameras,
    get_camera_info: camera_get_camera_info,
    set_callbacks: NULL,
};

typedef struct wrapper_camera_device {
    camera_device_t base;
    int id;
    camera_device_t *vendor;
} wrapper_camera_device_t;

#define VENDOR_CALL(device, func, ...) ({ \
    wrapper_camera_device_t *__wrapper_dev = (wrapper_camera_device_t*) device; \
    __wrapper_dev->vendor->ops->func(__wrapper_dev->vendor, ##__VA_ARGS__); \
})

#define CAMERA_ID(device) (((wrapper_camera_device_t *)(device))->id)

#ifdef EF44_JB_GRALLOC_HANDLE_COMPAT
#include "jb_handle_compat.h"

// EF44_CAMERA_KEYS: Qualcomm/Pantech CameraParameters keys. LineageOS 19.1
// no longer extends CameraParameters (CameraParametersExtra.h), so the
// wrapper keeps its own copies (values as in cm-14.1)
namespace ef44_cam {
static const char KEY_HDR[] = "hdr";
static const char HDR_ON[] = "hdr-on";
static const char HDR_OFF[] = "hdr-off";
static const char KEY_QC_ISO_MODE[] = "iso";
static const char KEY_QC_SUPPORTED_ISO_MODES[] = "iso-values";
static const char KEY_QC_ZSL[] = "zsl";
}
#endif

static int check_vendor_module()
{
    int rv = 0;
    ALOGV("%s", __FUNCTION__);

    if (gVendorModule)
        return 0;

    rv = hw_get_module_by_class("camera", "vendor",
            (const hw_module_t**)&gVendorModule);
    if (rv)
        ALOGE("failed to open vendor camera module");
    return rv;
}

const static char * iso_values[] = {"auto,ISO100,ISO200,ISO400,ISO800,ISO1600,ISO3200"};

static char *camera_fixup_getparams(int id, const char *settings)
{
    bool videoMode = false;
   // char *manipBuf;

    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#if !LOG_NDEBUG
    ALOGV("%s: original parameters:", __FUNCTION__);
    params.dump();
#endif

    if (params.get(android::CameraParameters::KEY_RECORDING_HINT)) {
        videoMode = (!strcmp(params.get(android::CameraParameters::KEY_RECORDING_HINT), "true"));
    }

    params.set(ef44_cam::KEY_QC_SUPPORTED_ISO_MODES, iso_values[id]);
#ifdef EF44_JB_GRALLOC_HANDLE_COMPAT
    /* EF44 default scene */
    if (!params.get(android::CameraParameters::KEY_SCENE_MODE))
        params.set(android::CameraParameters::KEY_SCENE_MODE, "auto");
    /* EF44 no ZSL advertised */
    params.set("zsl-values", "off");
    params.set(ef44_cam::KEY_QC_ZSL, "off");
#endif

    /* Set supported scene modes */
  /*  if (params.get(android::CameraParameters::KEY_SUPPORTED_SCENE_MODES)) {
        params.set(android::CameraParameters::KEY_SUPPORTED_SCENE_MODES,
                "auto,portrait,landscape,portrait,snow,beach,sunset,night,backlight,sports,antishake,flowers,candlelight,fireworks,party,theatre,action,ar,indoor,text,hdr");
    }*/
    if (id = 1) {
        params.set(android::CameraParameters::KEY_SUPPORTED_SCENE_MODES, "auto,action,night,sunset,party");
    }

    //params.set(android::CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION, "-4");
    //params.set(android::CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION, "4");
	//Add HDR
/*
    if (!videoMode) {
        manipBuf = strdup(params.get(android::CameraParameters::KEY_SUPPORTED_SCENE_MODES));
        if (manipBuf != NULL && strstr(manipBuf,"hdr") == NULL) {
            strncat(manipBuf,",hdr",4);
            params.set(android::CameraParameters::KEY_SUPPORTED_SCENE_MODES,
                manipBuf);
        }
        free(manipBuf);
    }*/

    /* Face detection runs the OlaWorks engine (libOlaEngine) on preview
     * frames, and on Android 12 it crashes the camera provider while
     * initialising. Report no face detection so nobody turns it on. */
    params.set(android::CameraParameters::KEY_MAX_NUM_DETECTED_FACES_HW, "0");
    params.set(android::CameraParameters::KEY_MAX_NUM_DETECTED_FACES_SW, "0");
    params.set("face-detection-values", "off");
    params.set("face-detection", "off");

#if !LOG_NDEBUG
    ALOGV("%s: fixed parameters:", __FUNCTION__);
    params.dump();
#endif

    android::String8 strParams = params.flatten();
    char *ret = strdup(strParams.string());

    return ret;
}

static char *camera_fixup_setparams(int id, const char *settings)
{
    bool videoMode = false;

    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#if !LOG_NDEBUG
    ALOGV("%s: original parameters:", __FUNCTION__);
    params.dump();
#endif

    if (params.get(android::CameraParameters::KEY_RECORDING_HINT)) {
        videoMode = (!strcmp(params.get(android::CameraParameters::KEY_RECORDING_HINT), "true"));
    }

    if (videoMode) {
       // params.set("dis", "disable");
        params.set(ef44_cam::KEY_QC_ZSL, "off");
    } else {
#ifdef EF44_JB_GRALLOC_HANDLE_COMPAT
        /* EF44 never ZSL */
        params.set(ef44_cam::KEY_QC_ZSL, "off");
        params.set("camera-mode", "0");
#else
        params.set(ef44_cam::KEY_QC_ZSL, "on");
#endif
    }

    if(params.get("iso")) {
        const char* isoMode = params.get(ef44_cam::KEY_QC_ISO_MODE);
        if(strcmp(isoMode, "ISO100") == 0)
            params.set(ef44_cam::KEY_QC_ISO_MODE, "100");
        else if(strcmp(isoMode, "ISO200") == 0)
            params.set(ef44_cam::KEY_QC_ISO_MODE, "200");
        else if(strcmp(isoMode, "ISO400") == 0)
            params.set(ef44_cam::KEY_QC_ISO_MODE, "400");
        else if(strcmp(isoMode, "ISO800") == 0)
            params.set(ef44_cam::KEY_QC_ISO_MODE, "800");
        else if(strcmp(isoMode, "ISO1600") == 0)
            params.set(ef44_cam::KEY_QC_ISO_MODE, "1600");
        else if(strcmp(isoMode, "ISO3200") == 0)
            params.set(ef44_cam::KEY_QC_ISO_MODE, "3200");
    }

    /* See camera_fixup_getparams: keep the OlaWorks face detector off */
    params.set("face-detection", "off");

	//HDR
  //  if (!videoMode && !strncmp(params.get(android::CameraParameters::KEY_SCENE_MODE),"hdr",3)) {
    //if (!videoMode) {
    //    params.set(ef44_cam::KEY_HDR,
    //            ef44_cam::HDR_ON);
      //  params.set("hdr-values", "1");
    //} else {
    //    params.set(ef44_cam::KEY_HDR,
     //           ef44_cam::HDR_OFF);
    //    params.set("hdr-values", "0");
  //  }

#if !LOG_NDEBUG
    ALOGV("%s: fixed parameters:", __FUNCTION__);
    params.dump();
#endif

    android::String8 strParams = params.flatten();
    char *ret = strdup(strParams.string());

    return ret;
}

/*******************************************************************
 * implementation of camera_device_ops functions
 *******************************************************************/

static int camera_set_preview_window(struct camera_device *device,
        struct preview_stream_ops *window)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

#ifdef EF44_JB_GRALLOC_HANDLE_COMPAT
    window = jb_wrap_window(CAMERA_ID(device), window);
#endif
    return VENDOR_CALL(device, set_preview_window, window);
}

/*
 * The JB QCamera HAL calls get_memory(fd, size, n, this) with its own
 * QCameraHardwareInterface as the last argument instead of the cookie it got
 * in set_callbacks. JB's CameraService ignored that argument; the HIDL
 * camera.device@1.0 impl casts it to its CameraDevice and crashed in
 * startPreview. Hand the HAL a get_memory that substitutes the real cookie.
 * The QCamera HAL only runs one camera at a time, so one slot is enough.
 */
static camera_request_memory gOrigGetMemory;
static void *gGetMemoryCookie;

static camera_memory_t *camera_get_memory_fixup(int fd, size_t buf_size,
        unsigned int num_bufs, void *user __unused)
{
    if (!gOrigGetMemory)
        return NULL;
    return gOrigGetMemory(fd, buf_size, num_bufs, gGetMemoryCookie);
}

/*
 * Video recording with metadata buffers.
 *
 * For each recording frame the JB QCamera HAL passes a metadata buffer
 * {type, native_handle_t *} whose handle holds the frame's ion fd, offset
 * and size, with the JB type kMetadataBufferTypeCameraSource (0). The
 * HIDL camera.device@1.0 impl only takes the handle out of
 * kMetadataBufferTypeNativeHandleSource (3) metadata and sends it across
 * processes with handleCallbackTimestamp(); anything else goes up as raw
 * bytes, so mediaserver got a pointer that is only valid in this process
 * and crashed in the encoder path (SIGSEGV in writeEmbeddedToParcel), and
 * no video file was written. Mark the metadata as NativeHandleSource; the
 * layout is the same (OMX.qcom venc reads data[0..2] = fd, offset, size).
 *
 * When the frame comes back, the HIDL impl stores a clone of the handle
 * in the metadata and leaves closing it to the HAL, which the JB HAL does
 * not know about: put the HAL's own handle back and free the clone (one fd
 * per frame would leak otherwise).
 */
#include <cutils/native_handle.h>

#define EF44_META_CAMERA_SOURCE 0        /* kMetadataBufferTypeCameraSource */
#define EF44_META_NATIVE_HANDLE_SOURCE 3 /* kMetadataBufferTypeNativeHandleSource */
#define EF44_MAX_REC_BUFS 32

struct ef44_video_meta {
    int32_t type;
    native_handle_t *handle;
};

static android::Mutex gRecLock;
static camera_data_timestamp_callback gOrigDataCbTimestamp;
static bool gMetaMode;
static struct {
    struct ef44_video_meta *meta;
    native_handle_t *orig;
} gRecBufs[EF44_MAX_REC_BUFS];

static void rec_bufs_reset_locked(void)
{
    memset(gRecBufs, 0, sizeof(gRecBufs));
}

static void camera_data_cb_timestamp_fixup(nsecs_t timestamp, int32_t msg_type,
        const camera_memory_t *data, unsigned int index, void *user)
{
    if (gMetaMode && (msg_type & CAMERA_MSG_VIDEO_FRAME) && data && data->data &&
            data->size >= (index + 1) * sizeof(struct ef44_video_meta)) {
        struct ef44_video_meta *md = (struct ef44_video_meta *)
                ((uint8_t *)data->data + index * sizeof(struct ef44_video_meta));
        android::Mutex::Autolock _l(gRecLock);
        if (md->type == EF44_META_CAMERA_SOURCE && md->handle) {
            int i, slot = -1;
            for (i = 0; i < EF44_MAX_REC_BUFS; i++) {
                if (gRecBufs[i].meta == md) {
                    slot = i;
                    break;
                }
                if (slot < 0 && !gRecBufs[i].meta)
                    slot = i;
            }
            if (slot >= 0) {
                gRecBufs[slot].meta = md;
                gRecBufs[slot].orig = md->handle;
            }
            md->type = EF44_META_NATIVE_HANDLE_SOURCE;
        }
    }
    if (gOrigDataCbTimestamp)
        gOrigDataCbTimestamp(timestamp, msg_type, data, index, user);
}

static void rec_buf_restore(const void *opaque)
{
    android::Mutex::Autolock _l(gRecLock);
    for (int i = 0; i < EF44_MAX_REC_BUFS; i++) {
        if (gRecBufs[i].meta != opaque)
            continue;
        struct ef44_video_meta *md = gRecBufs[i].meta;
        if (md->handle && md->handle != gRecBufs[i].orig) {
            native_handle_close(md->handle);
            native_handle_delete(md->handle);
        }
        md->handle = gRecBufs[i].orig;
        md->type = EF44_META_CAMERA_SOURCE;
        return;
    }
}

static void camera_set_callbacks(struct camera_device *device,
        camera_notify_callback notify_cb,
        camera_data_callback data_cb,
        camera_data_timestamp_callback data_cb_timestamp,
        camera_request_memory get_memory,
        void *user)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

#ifdef EF44_JB_GRALLOC_HANDLE_COMPAT
    data_cb = jb_wrap_data_cb(data_cb);
#endif
    if (data_cb_timestamp) {
        gOrigDataCbTimestamp = data_cb_timestamp;
        data_cb_timestamp = camera_data_cb_timestamp_fixup;
    }
    if (get_memory) {
        gOrigGetMemory = get_memory;
        gGetMemoryCookie = user;
        get_memory = camera_get_memory_fixup;
    }
    VENDOR_CALL(device, set_callbacks, notify_cb, data_cb, data_cb_timestamp,
            get_memory, user);
}

static void camera_enable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, enable_msg_type, msg_type);
}

static void camera_disable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, disable_msg_type, msg_type);
}

static int camera_msg_type_enabled(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return 0;

    return VENDOR_CALL(device, msg_type_enabled, msg_type);
}

static int camera_start_preview(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, start_preview);
}

static void camera_stop_preview(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, stop_preview);
}

static int camera_preview_enabled(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, preview_enabled);
}

static int camera_store_meta_data_in_buffers(struct camera_device *device,
        int enable)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    int ret = VENDOR_CALL(device, store_meta_data_in_buffers, enable);
    if (ret == 0) {
        android::Mutex::Autolock _l(gRecLock);
        gMetaMode = enable;
    }
    return ret;
}

static int camera_start_recording(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return EINVAL;

    {
        /* the HAL allocates new metadata buffers for each recording */
        android::Mutex::Autolock _l(gRecLock);
        rec_bufs_reset_locked();
    }
    return VENDOR_CALL(device, start_recording);
}

static void camera_stop_recording(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;


    VENDOR_CALL(device, stop_recording);
}

static int camera_recording_enabled(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, recording_enabled);
}

static void camera_release_recording_frame(struct camera_device *device,
        const void *opaque)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    rec_buf_restore(opaque);
    VENDOR_CALL(device, release_recording_frame, opaque);
}

static int camera_auto_focus(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, auto_focus);
}

static int camera_cancel_auto_focus(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_auto_focus);
}

static int camera_take_picture(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, take_picture);
}

static int camera_cancel_picture(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_picture);
}

static int camera_set_parameters(struct camera_device *device,
        const char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    char *tmp = NULL;
    tmp = camera_fixup_setparams(CAMERA_ID(device), params);

    int ret = VENDOR_CALL(device, set_parameters, tmp);
    return ret;
}

static char *camera_get_parameters(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return NULL;

    char *params = VENDOR_CALL(device, get_parameters);

    char *tmp = camera_fixup_getparams(CAMERA_ID(device), params);
    VENDOR_CALL(device, put_parameters, params);
    params = tmp;

    return params;
}

static void camera_put_parameters(struct camera_device *device, char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (params)
        free(params);
}

static int camera_send_command(struct camera_device *device,
        int32_t cmd, int32_t arg1, int32_t arg2)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    /* See camera_fixup_getparams: face detection crashes the HAL */
    if (cmd == CAMERA_CMD_START_FACE_DETECTION)
        return -EINVAL;
    if (cmd == CAMERA_CMD_STOP_FACE_DETECTION)
        return 0;

    return VENDOR_CALL(device, send_command, cmd, arg1, arg2);
}

static void camera_release(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, release);
}

static int camera_dump(struct camera_device *device, int fd)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, dump, fd);
}

extern "C" void heaptracker_free_leaked_memory(void);

static int camera_device_close(hw_device_t *device)
{
    int ret = 0;
    wrapper_camera_device_t *wrapper_dev = NULL;

    ALOGV("%s", __FUNCTION__);

    android::Mutex::Autolock lock(gCameraWrapperLock);

    if (!device) {
        ret = -EINVAL;
        goto done;
    }

    wrapper_dev = (wrapper_camera_device_t*) device;

    wrapper_dev->vendor->common.close((hw_device_t*)wrapper_dev->vendor);
    if (wrapper_dev->base.ops)
        free(wrapper_dev->base.ops);
    free(wrapper_dev);
done:
#ifdef HEAPTRACKER
    heaptracker_free_leaked_memory();
#endif
    return ret;
}

/*******************************************************************
 * implementation of camera_module functions
 *******************************************************************/

/* open device handle to one of the cameras
 *
 * assume camera service will keep singleton of each camera
 * so this function will always only be called once per camera instance
 */

static int camera_device_open(const hw_module_t *module, const char *name,
        hw_device_t **device)
{
    int rv = 0;
    int num_cameras = 0;
    int cameraid;
    wrapper_camera_device_t *camera_device = NULL;
    camera_device_ops_t *camera_ops = NULL;

    android::Mutex::Autolock lock(gCameraWrapperLock);

    ALOGV("%s", __FUNCTION__);

    if (name != NULL) {
        if (check_vendor_module())
            return -EINVAL;

        cameraid = atoi(name);
        num_cameras = gVendorModule->get_number_of_cameras();

        if (cameraid > num_cameras) {
            ALOGE("camera service provided cameraid out of bounds, "
                    "cameraid = %d, num supported = %d",
                    cameraid, num_cameras);
            rv = -EINVAL;
            goto fail;
        }

        camera_device = (wrapper_camera_device_t*)malloc(sizeof(*camera_device));
        if (!camera_device) {
            ALOGE("camera_device allocation fail");
            rv = -ENOMEM;
            goto fail;
        }
        memset(camera_device, 0, sizeof(*camera_device));
        camera_device->id = cameraid;

        rv = gVendorModule->common.methods->open(
                (const hw_module_t*)gVendorModule, name,
                (hw_device_t**)&(camera_device->vendor));
        if (rv) {
            ALOGE("vendor camera open fail");
            goto fail;
        }
        ALOGV("%s: got vendor camera device 0x%08X",
                __FUNCTION__, (uintptr_t)(camera_device->vendor));

        camera_ops = (camera_device_ops_t*)malloc(sizeof(*camera_ops));
        if (!camera_ops) {
            ALOGE("camera_ops allocation fail");
            rv = -ENOMEM;
            goto fail;
        }

        memset(camera_ops, 0, sizeof(*camera_ops));

        camera_device->base.common.tag = HARDWARE_DEVICE_TAG;
        camera_device->base.common.version = 0;
        camera_device->base.common.module = (hw_module_t *)(module);
        camera_device->base.common.close = camera_device_close;
        camera_device->base.ops = camera_ops;

        camera_ops->set_preview_window = camera_set_preview_window;
        camera_ops->set_callbacks = camera_set_callbacks;
        camera_ops->enable_msg_type = camera_enable_msg_type;
        camera_ops->disable_msg_type = camera_disable_msg_type;
        camera_ops->msg_type_enabled = camera_msg_type_enabled;
        camera_ops->start_preview = camera_start_preview;
        camera_ops->stop_preview = camera_stop_preview;
        camera_ops->preview_enabled = camera_preview_enabled;
        camera_ops->store_meta_data_in_buffers = camera_store_meta_data_in_buffers;
        camera_ops->start_recording = camera_start_recording;
        camera_ops->stop_recording = camera_stop_recording;
        camera_ops->recording_enabled = camera_recording_enabled;
        camera_ops->release_recording_frame = camera_release_recording_frame;
        camera_ops->auto_focus = camera_auto_focus;
        camera_ops->cancel_auto_focus = camera_cancel_auto_focus;
        camera_ops->take_picture = camera_take_picture;
        camera_ops->cancel_picture = camera_cancel_picture;
        camera_ops->set_parameters = camera_set_parameters;
        camera_ops->get_parameters = camera_get_parameters;
        camera_ops->put_parameters = camera_put_parameters;
        camera_ops->send_command = camera_send_command;
        camera_ops->release = camera_release;
        camera_ops->dump = camera_dump;

        *device = &camera_device->base.common;
    }

    return rv;

fail:
    if (camera_device) {
        free(camera_device);
        camera_device = NULL;
    }
    if (camera_ops) {
        free(camera_ops);
        camera_ops = NULL;
    }
    *device = NULL;
    return rv;
}

static int camera_get_number_of_cameras(void)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->get_number_of_cameras();
}

static int camera_get_camera_info(int camera_id, struct camera_info *info)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->get_camera_info(camera_id, info);
}
