#
# LineageOS 19.1 for the Pantech VEGA S5 (ef44)
#
LOCAL_PATH := device/pantech/ef44

PRODUCT_SHIPPING_API_LEVEL := 16
PRODUCT_SOONG_NAMESPACES += $(LOCAL_PATH)

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay
PRODUCT_ENFORCE_RRO_TARGETS := *

PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xhdpi

# The 1024 profile (heapsize 256m), as on cm-14.1: ART reserves twice the
# heap size in one piece, and the 3.4 kernel's bottom-up mmap layout has no
# 1 GiB hole for the 2048 profile's 512m heap (zygote aborts)
$(call inherit-product, frameworks/native/build/phone-xhdpi-1024-dalvik-heap.mk)

# Boot: fstab in the ramdisk for first-stage mount, init scripts
PRODUCT_PACKAGES += \
    fstab.qcom \
    fstab.ramdisk \
    init.qcom.rc \
    init.ef44.usb.rc \
    init.ef44.debug.rc \
    init.ef44.power.rc \
    ef44-logsync.sh \
    ef44-bdaddr.sh \
    ef44-rtc.sh \
    init.qcom.bt.sh \
    init.ef44.modem.sh \
    ueventd.qcom.rc

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml

# Graphics: msm8960 display HAL (hardware/qcom-caf/msm8960/display)
PRODUCT_PACKAGES += \
    copybit.msm8960 \
    gralloc.msm8960 \
    hwcomposer.msm8960 \
    memtrack.msm8960 \
    libgenlock \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.composer@2.1-service \
    android.hardware.graphics.mapper@2.0-impl-2.1 \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service
PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=131072 \
    ro.sf.lcd_density=320 \
    debug.hwui.use_buffer_age=false \
    debug.sf.enable_gl_backpressure=1 \
    debug.sf.latch_unsignaled=1
# Compose with the 3D GPU: with "dyn" the HWC blended layers on the 2D core
# (C2D/copybit) and MDP composition, and scrolling ran at ~13 fps; with GPU
# composition it runs at ~44 fps
PRODUCT_PROPERTY_OVERRIDES += \
    debug.composition.type=gpu \
    persist.hwc.mdpcomp.enable=false
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/firmware/a225_pfp.fw:$(TARGET_COPY_OUT_VENDOR)/firmware/a225_pfp.fw \
    $(LOCAL_PATH)/firmware/a225_pm4.fw:$(TARGET_COPY_OUT_VENDOR)/firmware/a225_pm4.fw \
    $(LOCAL_PATH)/firmware/a225p5_pm4.fw:$(TARGET_COPY_OUT_VENDOR)/firmware/a225p5_pm4.fw \
    $(LOCAL_PATH)/firmware/vidc_1080p.fw:$(TARGET_COPY_OUT_VENDOR)/firmware/vidc_1080p.fw \
    $(LOCAL_PATH)/firmware/vidc.mdt:$(TARGET_COPY_OUT_VENDOR)/firmware/vidc.mdt \
    $(LOCAL_PATH)/firmware/vidc.b00:$(TARGET_COPY_OUT_VENDOR)/firmware/vidc.b00 \
    $(LOCAL_PATH)/firmware/vidc.b01:$(TARGET_COPY_OUT_VENDOR)/firmware/vidc.b01 \
    $(LOCAL_PATH)/firmware/vidc.b02:$(TARGET_COPY_OUT_VENDOR)/firmware/vidc.b02 \
    $(LOCAL_PATH)/firmware/vidc.b03:$(TARGET_COPY_OUT_VENDOR)/firmware/vidc.b03

# Audio: msm8960 audio HAL behind the HIDL wrapper
PRODUCT_PACKAGES += \
    audio.bluetooth.default \
    android.hardware.bluetooth.audio-impl \
    audio.primary.msm8960 \
    audio.r_submix.default \
    audio.usb.default \
    libaudio-resampler \
    libqcomvoiceprocessing \
    tinymix \
    android.hardware.audio.service \
    android.hardware.audio@6.0-impl \
    android.hardware.audio.effect@6.0-impl
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.xml \
    $(LOCAL_PATH)/audio/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
    $(LOCAL_PATH)/configs14/audio_platform_info.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_platform_info.xml \
    $(LOCAL_PATH)/configs14/mixer_paths.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths.xml \
    frameworks/av/services/audiopolicy/config/bluetooth_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/bluetooth_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml
# Bluetooth profiles: Android 13's Bluetooth app starts only the profiles
# these properties enable (Android 12 had them on in its config.xml); without
# them only GATT ran, and a headset paired with no profiles ("No profiles.
# Maybe we will connect later") and never connected
PRODUCT_VENDOR_PROPERTIES += \
    bluetooth.profile.a2dp.source.enabled=true \
    bluetooth.profile.avrcp.target.enabled=true \
    bluetooth.profile.bas.client.enabled=true \
    bluetooth.profile.gatt.enabled=true \
    bluetooth.profile.hfp.ag.enabled=true \
    bluetooth.profile.hid.device.enabled=true \
    bluetooth.profile.hid.host.enabled=true \
    bluetooth.profile.map.server.enabled=true \
    bluetooth.profile.opp.enabled=true \
    bluetooth.profile.pan.nap.enabled=true \
    bluetooth.profile.pan.panu.enabled=true \
    bluetooth.profile.pbap.server.enabled=true
PRODUCT_PROPERTY_OVERRIDES += \
    ro.bluetooth.a2dp_offload.supported=false \
    persist.bluetooth.a2dp_offload.disabled=true \
    audio.offload.disable=1 \
    ro.config.media_vol_steps=25 \
    ro.config.vc_call_vol_steps=7

# Media: msm8960 OMX (hardware/qcom-caf/msm8960/media)
PRODUCT_PACKAGES += \
    libOmxAacEnc \
    libOmxAmrEnc \
    libOmxCore \
    libOmxEvrcEnc \
    libOmxQcelp13Enc \
    libOmxVdec \
    libOmxVenc \
    libc2dcolorconvert \
    libstagefrighthw \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
    $(LOCAL_PATH)/configs14/media_profiles.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video.xml
PRODUCT_PROPERTY_OVERRIDES += \
    debug.stagefright.ccodec=0

# Bluetooth: BCM4334
PRODUCT_PACKAGES += \
    libbt-vendor \
    android.hardware.bluetooth@1.0-impl \
    android.hardware.bluetooth@1.0-service

# Camera
PRODUCT_PACKAGES += \
    camera.msm8960 \
    libshim_camera \
    android.hardware.camera.provider@2.4-impl \
    android.hardware.camera.provider@2.4-service
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir14/CE150F00.bin:$(TARGET_COPY_OUT_VENDOR)/etc/CE150F00.bin \
    $(LOCAL_PATH)/rootdir14/CE150F01.bin:$(TARGET_COPY_OUT_VENDOR)/etc/CE150F01.bin \
    $(LOCAL_PATH)/rootdir14/CE150F02_89.bin:$(TARGET_COPY_OUT_VENDOR)/etc/CE150F02_89.bin \
    $(LOCAL_PATH)/rootdir14/CE150F03_89.bin:$(TARGET_COPY_OUT_VENDOR)/etc/CE150F03_89.bin

# Health, keymaster, gatekeeper, USB, power
PRODUCT_PACKAGES += \
    android.hardware.health@2.1-impl \
    android.hardware.health@2.1-service \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service \
    android.hardware.gatekeeper@1.0-service.software \
    android.hardware.usb@1.0-service.basic

# Lights
PRODUCT_PACKAGES += \
    lights.msm8960 \
    android.hardware.light@2.0-impl \
    android.hardware.light@2.0-service

# Sensors (KitKat-era Pantech sensor HAL in the vendor blobs). The blob is a
# version 0.1 HAL; sensors.ef44 presents it as the 1.3 HAL the 1.0 impl needs.
PRODUCT_PACKAGES += \
    android.hardware.sensors@1.0-impl \
    android.hardware.sensors@1.0-service \
    sensors.ef44
PRODUCT_PROPERTY_OVERRIDES += \
    ro.hardware.sensors=ef44

# Vibrator (timed_output pm8xxx-vib)
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.0-impl \
    android.hardware.vibrator@1.0-service

# Wi-Fi: BCM4334 (bcmdhd). No vendor Wi-Fi HAL (android.hardware.wifi): the
# 2013 bcmdhd has none of the nl80211 vendor commands libwifi-hal-bcm uses
# and the fallback HAL cannot start, so the framework runs on wificond and
# wpa_supplicant alone; init.qcom.rc sets the firmware and nvram paths.
PRODUCT_PACKAGES += \
    hostapd \
    libwpa_client \
    wificond \
    wpa_supplicant \
    wpa_supplicant.conf
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wifi/fw_bcmdhd.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/fw_bcmdhd.bin \
    $(LOCAL_PATH)/wifi/fw_bcmdhd_apsta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/fw_bcmdhd_apsta.bin \
    $(LOCAL_PATH)/wifi/fw_bcmdhd_p2p.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/fw_bcmdhd_p2p.bin \
    $(LOCAL_PATH)/wifi/nvram_4334.txt:$(TARGET_COPY_OUT_SYSTEM)/etc/nvram_4334.txt \
    vendor/pantech/ef44/proprietary/system/bin/BCM4334B0_002.001.013.0360.0458_RC.hcd:$(TARGET_COPY_OUT_VENDOR)/firmware/BCM4334B0.hcd \
    $(LOCAL_PATH)/bluetooth/bt_vendor.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/bluetooth/bt_vendor.conf \
    $(LOCAL_PATH)/wifi14/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf \
    $(LOCAL_PATH)/wifi14/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0

# Radio (Pantech qcril)
PRODUCT_PACKAGES += \
    libril \
    librilutils \
    rild \
    libshim_rmt \
    libshim_mmcamera
# vendor.rild.libpath/libargs are in vendor.prop (the args contain a space)
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libpath=/system/lib/libril-qc-qmi-1.so \
    ro.telephony.default_network=9 \
    keyguard.no_require_sim=true \
    ro.com.android.mobiledata=false

# GPS: the stock gps.msm8960 HAL is not in the vendor blobs yet. Without it
# gnss@1.0-service exits, and a declared-but-missing IGnss made system_server
# wait forever in GnssNative (watchdog kill), so no gnss HAL for now.
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs14/gps.conf:$(TARGET_COPY_OUT_VENDOR)/etc/gps.conf \
    $(LOCAL_PATH)/configs14/sec_config:$(TARGET_COPY_OUT_VENDOR)/etc/sec_config

# Input
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keylayout/keypad_8960.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/keypad_8960.kl \
    $(LOCAL_PATH)/keylayout/qt602240_ts_input.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/qt602240_ts_input.kl \
    $(LOCAL_PATH)/keylayout/Button_Jack.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/msm8960-snd-card_Button_Jack.kl \
    $(LOCAL_PATH)/idc/qt602240_ts_input.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/qt602240_ts_input.idc

# Debugging during bring-up: adb on from boot (userdebug allows adb root);
# logcat is mirrored to the kernel log (init.ef44.debug.rc), so ef44_ramlog
# keeps it across resets
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp,adb
# FunctionFS on the 3.4 kernel has no AIO; adbd's io_submit path fails with
# EINVAL, so use its blocking read/write path
PRODUCT_PROPERTY_OVERRIDES += \
    persist.adb.nonblocking_ffs=0

# On-screen navigation bar (the capacitive keys alone are not enough on 12)
PRODUCT_PROPERTY_OVERRIDES += \
    qemu.hw.mainkeys=0

# VEGA Iron 2 home (VEGA Home), its icon styles and icon packs, Design Home,
# the wallpaper popup and the VEGA keyboard (launcher/Android.mk, made by
# los19/launcher/build-launcher19.sh). Trebuchet stays for Recents only;
# VEGAIME overrides LatinIME, VEGAMusicPlayer overrides Eleven, VEGADialer
# overrides LineageOS's Dialer, VEGAContacts LineageOS's Contacts and VEGAMms
# LineageOS's Messaging.
PRODUCT_PACKAGES += \
    PantechLauncher \
    IconStyleAgent \
    HomeDeco \
    WallpaperPopup \
    VegaHomeThemeFacade \
    VegaHomeThemeLimited \
    VegaHomeThemeMiroo \
    VegaHomeThemeUnique \
    VEGAIME \
    VEGAIMEProvider \
    VEGAMusicPlayer \
    VEGAMusicFX \
    libvegabundlewrapper \
    FileManager \
    PantechRes \
    VEGAClock \
    WallpaperChooser \
    VEGACalendar \
    VEGACalculator \
    VEGAVoiceRecorder \
    VEGACamera \
    VEGADialer \
    PantechIspRes \
    VEGAContacts \
    VEGAMms \
    VEGAMmsSmsProvider

# VEGA Iron 2 sounds (iron2_audio/, from its firmware by los19/launcher/iron2-audio.sh;
# audio/ holds the audio HAL configs)
# in place of LineageOS's ringtones, notification sounds and alarms (setup19.sh
# EF44_IRON2_SOUNDS drops those from vendor/lineage). Installed with the Iron
# 2's layout under /system/media/audio: its apps name these paths (VEGA
# Messages' messagenotify/, VEGA Clock's alarms/Alarm_Beep_03.ogg), and
# Android 12 takes every file under a ringtones/, notifications/ or alarms/
# directory as that kind of sound (ringtones/sky_ringtone and
# ringtones/android_default are ringtones).
PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/iron2_audio,$(TARGET_COPY_OUT_SYSTEM)/media/audio)
# The Iron 2's UI sounds also replace LineageOS's of the same name in
# /product/media/audio/ui, where Android 12 looks first and where its lock,
# unlock, dock and low battery sound settings point; LineageOS's that the Iron
# 2 has no counterpart for (charging, NFC, trusted...) stay.
EF44_IRON2_UI_SOUNDS := Dock Effect_Tick KeypressDelete KeypressReturn KeypressSpacebar \
    KeypressStandard LowBattery Undock VideoRecord camera_click
# The Iron 2's lock (screen off) and unlock sounds are not its ui/Lock.ogg and
# Unlock.ogg (unused there) but those of its lock screen's "focus" effect, the
# one VegaLockView ports (its touch sounds are the same effect's
# hold_focus_down/drag/up): Keyguard.apk res/raw/hold_focus_lock.ogg and
# hold_focus_unlock.ogg (iron2_lock_sounds/, by los20/launcher/iron2-audio.sh).
# They go where Android's lock and unlock sound settings point, so phones set
# up before this change get them too.
PRODUCT_COPY_FILES += \
    $(foreach s,$(EF44_IRON2_UI_SOUNDS),$(LOCAL_PATH)/iron2_audio/ui/$(s).ogg:$(TARGET_COPY_OUT_PRODUCT)/media/audio/ui/$(s).ogg) \
    $(LOCAL_PATH)/iron2_audio/ui/Camera_Focus.ogg:$(TARGET_COPY_OUT_PRODUCT)/media/audio/ui/camera_focus.ogg \
    $(LOCAL_PATH)/iron2_lock_sounds/hold_focus_lock.ogg:$(TARGET_COPY_OUT_PRODUCT)/media/audio/ui/Lock.ogg \
    $(LOCAL_PATH)/iron2_lock_sounds/hold_focus_unlock.ogg:$(TARGET_COPY_OUT_PRODUCT)/media/audio/ui/Unlock.ogg
# The Iron 2's defaults (its build.prop)
PRODUCT_PRODUCT_PROPERTIES += \
    ro.config.ringtone=VEGA_Ringtone_Jumping.ogg \
    ro.config.notification_sound=Jumping.ogg \
    ro.config.alarm_alert=Heavens_Morning.ogg

# The VEGA Iron 2 lock screen (SystemUI's VegaLockView) counts new missed calls
# and unread messages for its buttons
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/default-permissions-ef44.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/default-permissions/default-permissions-ef44.xml

# Settings names androidx.window.extensions/sidecar as optional
# <uses-library> (its odex is compiled with them in the class loader context).
# Without them installed the context on the phone differed and ART dropped
# Settings' odex at every start ("shared library size mismatch").
PRODUCT_PACKAGES += \
    androidx.window.extensions \
    androidx.window.sidecar
# With the extensions present Settings takes the phone for a two-pane large
# screen (SplitController.isSplitSupported) and sends every deep link from
# another app (e.g. the USB debugging notification) to DeepLinkHomepageActivity,
# which it keeps disabled on a phone: ActivityNotFoundException, crash. This
# phone has no large screen.
PRODUCT_SYSTEM_PROPERTIES += \
    persist.sys.fflag.override.settings_support_large_screen=false

# VEGA Home and the VEGA keyboard are always running: compiled ahead of time
# with the system image (the other VEGA apps "verify"), like SystemUI and
# Trebuchet (LineageOS common.mk)
PRODUCT_DEXPREOPT_SPEED_APPS += \
    PantechLauncher \
    VEGAIME

# VEGA wallpapers WallpaperPopup restores and saves from (the chooser has its
# own; the system default is in overlay/, drawable-nodpi/default_wallpaper.jpg)
PRODUCT_COPY_FILES += \
    $(foreach f,$(wildcard $(LOCAL_PATH)/launcher/home_default/*.jpg),\
        $(f):$(TARGET_COPY_OUT_SYSTEM)/media/images/hold/home_default/$(notdir $(f)))

# VEGA Iron 2 fonts (VEGA, VEGA brand, Sandol, Seoul) for Settings > Display >
# Font (com.android.settings.ef44.VegaFontTypeActivity), where the Iron 2 kept
# them; not in fonts.xml, only the chosen one is used (FontManagerService,
# Ef44FontSwitcher)
PRODUCT_COPY_FILES += \
    $(foreach f,$(wildcard $(LOCAL_PATH)/fonts/*.ttf),\
        $(f):$(TARGET_COPY_OUT_SYSTEM)/fonts/$(notdir $(f)))

# Memory
PRODUCT_PROPERTY_OVERRIDES += \
    ro.lmk.use_minfree_levels=true \
    ro.lmk.use_psi=false \
    ro.config.low_ram=false

# Apps left out of the image to save memory and boot time (each was a process
# at every boot): see removepackages/Android.mk
PRODUCT_PACKAGES += \
    Ef44RemovePackages
# No backup service (Seedvault is left out): no backup transport process,
# no key/value backup jobs after boot
PRODUCT_SYSTEM_PROPERTIES += \
    ro.backup.disable=true

$(call inherit-product, vendor/pantech/ef44/ef44-vendor.mk)
