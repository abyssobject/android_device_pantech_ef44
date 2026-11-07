#
# LineageOS 19.1 for the Pantech VEGA S5 (IM-A840S/SP, ef44)
#
# Laid out after LineageOS's lineage-18.1 Sony blue-common (another MSM8960
# phone), with the ef44 specifics of the cm-14.1 port.
#
DEVICE_PATH := device/pantech/ef44

TARGET_SPECIFIC_HEADER_PATH := $(DEVICE_PATH)/include

# Platform
TARGET_BOARD_PLATFORM := msm8960
TARGET_BOOTLOADER_BOARD_NAME := MSM8960
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_VARIANT := krait
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true
BOARD_VENDOR := pantech
BOARD_USES_QCOM_HARDWARE := true
QCOM_HARDWARE_VARIANT := msm8960

# Kernel: built by ef44-port/los19/build-kernel19.sh from the lineage-19.1
# branch (3.4 + the Android 12 backports); the source here only provides the
# UAPI headers for the HALs
TARGET_KERNEL_SOURCE := kernel/pantech/ef44
TARGET_KERNEL_CONFIG := lineage_ef44s_defconfig
TARGET_KERNEL_CLANG_COMPILE := false
TARGET_FORCE_PREBUILT_KERNEL := true
TARGET_PREBUILT_KERNEL := $(DEVICE_PATH)/prebuilt/zImage
BOARD_KERNEL_IMAGE_NAME := zImage
BOARD_KERNEL_BASE := 0x80200000
BOARD_KERNEL_PAGESIZE := 2048
BOARD_MKBOOTIMG_ARGS := --ramdisk_offset 0x02000000
# androidboot.boot_devices: first-stage init only creates the by-name links
# (needed to mount /system) for the eMMC controller named here.
# init_fatal_reboot_target: a fatal init error reboots to TWRP (where the
# ef44_ramlog kernel log can be read) instead of the bootloader
# kgsl.mmutype=gpummu (stock): the Pantech Adreno driver finds no EGL config
# with the IOMMU. Per-process gpummu pagetables stay on: one shared 256 MB
# pagetable ran out on Android 12 (IOCTL_KGSL_GPUMEM_ALLOC_ID ENOMEM).
# console=ef44ramlog0: with no console= the kernel enabled the first console
# that has a tty, the msm_serial_hsl UART (ttyHSL0), and printk() wrote
# every kernel message and SELinux denial to it synchronously at 115200 baud
# (~13 ms per line), stalling the kernel boot, init and every process that
# logged to the kernel. The RAM log (ef44_ramlog) stays the only console;
# dmesg is unchanged.
BOARD_KERNEL_CMDLINE := androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x3F \
    kgsl.mmutype=gpummu androidboot.emmc=true vmalloc=0x16000000 \
    androidboot.selinux=permissive subsystem_restart.restart_level=3 \
    androidboot.boot_devices=msm_sdcc.1 \
    androidboot.init_fatal_reboot_target=recovery \
    console=ef44ramlog0 log_buf_len=1M

# Partitions (no vendor partition: vendor lives in /system/vendor)
BOARD_BOOTIMAGE_PARTITION_SIZE := 9437184
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1468006400
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12570292224
BOARD_CACHEIMAGE_PARTITION_SIZE := 340787200
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_FLASH_BLOCK_SIZE := 131072
TARGET_USERIMAGES_USE_EXT4 := true
TARGET_COPY_OUT_VENDOR := system/vendor
BOARD_ROOT_EXTRA_FOLDERS := persist firmware

# Recovery: the phone keeps its own TWRP in the 10 MiB recovery partition.
# The build still needs a recovery image for the OTA package (it reads the
# recovery fstab from it); the Lineage recovery is larger than the real
# partition, so the size here is nominal and recovery.img is never flashed
# (the ROM leaves recovery alone unless "Update recovery" is enabled)
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 33554432
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/fstab.qcom
TARGET_RECOVERY_PIXEL_FORMAT := RGBX_8888

# The cm-14.1 blobs are installed with PRODUCT_COPY_FILES
BUILD_BROKEN_ELF_PREBUILT_PRODUCT_COPY_FILES := true

# Android 12 userspace on a 3.4 kernel
TARGET_USES_64_BIT_BINDER := true
TARGET_NEEDS_PLATFORM_TEXT_RELOCATIONS := true
TARGET_VNDK_USE_CORE_VARIANT := true
PRODUCT_ENFORCE_VINTF_MANIFEST_OVERRIDE := true
DEVICE_MANIFEST_FILE := $(DEVICE_PATH)/manifest.xml
TARGET_PROCESS_SDK_VERSION_OVERRIDE := \
    /system/bin/mediaserver=22 \
    /system/vendor/bin/hw/android.hardware.sensors@1.0-service=22 \
    /system/bin/cameraserver=22

# Graphics: Adreno 225 / MDP4
TARGET_USES_ION := true
TARGET_USES_C2D_COMPOSITION := true
TARGET_ADDITIONAL_GRALLOC_10_USAGE_BITS := 0x02000000U
TARGET_DISABLE_POSTRENDER_CLEANUP := true
TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720

# Audio
BOARD_USES_ALSA_AUDIO := true
USE_XML_AUDIO_POLICY_CONF := 1
AUDIO_FEATURE_ENABLED_PROXY_DEVICE := true

# Bluetooth: Broadcom BCM4334 over UART
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_PATH)/bluetooth
BOARD_CUSTOM_BT_CONFIG := $(DEVICE_PATH)/bluetooth/vnd_ef44.txt

# Wi-Fi: Broadcom BCM4334, bcmdhd built into the kernel
BOARD_WLAN_DEVICE := bcmdhd
# The 2013 bcmdhd lacks the nl80211 vendor commands libwifi-hal-bcm needs;
# link the fallback vendor HAL (setup19.sh patch in libwifi_hal/Android.mk)
EF44_WIFI_HAL_FALLBACK := true
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_bcmdhd
WIFI_DRIVER_FW_PATH_PARAM := "/sys/module/bcmdhd/parameters/firmware_path"
WIFI_DRIVER_FW_PATH_STA := "/system/vendor/firmware/fw_bcmdhd.bin"
WIFI_DRIVER_FW_PATH_AP := "/system/vendor/firmware/fw_bcmdhd_apsta.bin"
WIFI_DRIVER_FW_PATH_P2P := "/system/vendor/firmware/fw_bcmdhd_p2p.bin"
WIFI_HIDL_UNIFIED_SUPPLICANT_SERVICE_RC_ENTRY := true
WIFI_HIDL_FEATURE_DISABLE_AP_MAC_RANDOMIZATION := true

# Camera: Pantech JB HAL1 behind the CameraWrapper
TARGET_HAS_LEGACY_CAMERA_HAL1 := true
USE_DEVICE_SPECIFIC_CAMERA := true
TARGET_USES_MEDIA_EXTENSIONS := true
TARGET_LD_SHIM_LIBS := \
    /system/vendor/lib/hw/camera.vendor.msm8960.so|libshim_camera.so

# Lights
TARGET_PROVIDES_LIBLIGHT := true

# RIL: Pantech qcril on LineageOS 19.1's hardware/ril libril
TARGET_USES_OLD_MNC_FORMAT := true

# SELinux (permissive on the command line while bringing up)
BOARD_VENDOR_SEPOLICY_DIRS += $(DEVICE_PATH)/sepolicy/vendor
SELINUX_IGNORE_NEVERALLOWS := true

# Properties
TARGET_SYSTEM_PROP += $(DEVICE_PATH)/system.prop
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

-include vendor/pantech/ef44/BoardConfigVendor.mk
