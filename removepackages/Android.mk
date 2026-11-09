LOCAL_PATH := $(call my-dir)

# 1 GB of RAM (about 813 MB usable): apps that only cost memory and boot time
# on this phone are left out of the image. Each one was its own process at
# every boot (about 30 MB each), on top of the BOOT_COMPLETED rush.
#  - Updater: OTA updates for official LineageOS builds only
#  - Seedvault, LocalContactsBackup: backup (off, ro.backup.disable)
#  - DynamicSystemInstallationService: GSI installer
#  - ImsServiceEntitlement: VoWiFi entitlement, no IMS here
#  - OneTimeInitializer: launcher shortcut migration
#  - AudioFX: LineageOS's effects app; VEGA MusicFX is the one in use
#  - SecureElement: persistent (always running) OMAPI service; this phone has
#    no eSE and its KitKat RIL no SIM OMAPI
include $(CLEAR_VARS)
LOCAL_MODULE := Ef44RemovePackages
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_TAGS := optional
LOCAL_OVERRIDES_PACKAGES := \
    Updater \
    Seedvault \
    LocalContactsBackup \
    DynamicSystemInstallationService \
    ImsServiceEntitlement \
    OneTimeInitializer \
    AudioFX \
    SecureElement
LOCAL_UNINSTALLABLE_MODULE := true
LOCAL_CERTIFICATE := PRESIGNED
LOCAL_SRC_FILES := /dev/null
include $(BUILD_PREBUILT)
