ifneq ($(CONFIG_UORB_CASE_TEST),)

    ifneq ($(CONFIG_UORB_TEST_DEMO),)
        PROGNAME += uorb_test_demo
        MAINSRC += $(CURDIR)/uorb_test/demo.c

        PROGNAME += uorb_test_media_sender
        MAINSRC += $(CURDIR)/uorb_test/media_sender.c

        PROGNAME += uorb_test_media_receiver
        MAINSRC += $(CURDIR)/uorb_test/media_receiver.c
    endif

    ifneq ($(CONFIG_UORB_ADVERTISE_DEMO),)
        PROGNAME += uorb_advertise_demo
        MAINSRC += $(CURDIR)/uorb_test/orb_advertise_main.c
    endif

    ifneq ($(CONFIG_UORB_RPMSG_TEST),)
        PROGNAME += uorb_rpmsg_test
        MAINSRC += $(CURDIR)/uorb_test/rpmsg_test.c
    endif

    ifneq ($(CONFIG_UORB_DOWNSAMPLING_TEST),)
        PROGNAME += uorb_downsample
        MAINSRC += $(CURDIR)/uorb_test/downsampling_test.c
        CFLAGS += -I$(APPDIR)/system/uorb/test
    endif

endif
