LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),ef44)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
