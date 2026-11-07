$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)
$(call inherit-product, device/pantech/ef44/device.mk)

PRODUCT_DEVICE := ef44
PRODUCT_NAME := lineage_ef44
PRODUCT_BRAND := VEGA
PRODUCT_MODEL := IM-A840S
PRODUCT_MANUFACTURER := Pantech

TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720
