LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	camera.c \
	camera_keys.c \
	camera_pantech.c \
	camera_kk.cpp

LOCAL_SHARED_LIBRARIES := libbinder libcamera_client libutils

LOCAL_MODULE := libshim_camera
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := rmt_mmap.c rmt_select.c

LOCAL_MODULE := libshim_rmt
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := mmcamera_daemon.c
LOCAL_SHARED_LIBRARIES := libdl liblog

LOCAL_MODULE := libshim_mmcamera
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)