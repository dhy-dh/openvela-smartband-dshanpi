MY_COMMON_PATH := vendor/vela/apps/netutils

PRODUCT_PACKAGES += rpmsg_tun_server

BOARD_SEPOLICY_DIRS += \
    $(MY_COMMON_PATH)/rpmsg_tun/sepolicy

