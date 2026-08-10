#include <nuttx/config.h>

#include "openvela_ui_power.h"

#ifdef CONFIG_ARCH_BOARD_R528S3_DSHANPI

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <nuttx/video/fb.h>

#ifdef CONFIG_UORB
#  include <nuttx/uorb.h>
#  include <sensor/gesture.h>
#endif

#include "openvela_ui.h"

#define UI_STANDBY_TIMEOUT_MS 60000U
#define UI_POWER_POLL_MS       1000U

#ifdef CONFIG_LCD_MAXPOWER
#  define UI_LCD_MAX_POWER CONFIG_LCD_MAXPOWER
#else
#  define UI_LCD_MAX_POWER 1
#endif

struct orb_metadata;

typedef struct {
    lv_display_t *display;
    lv_indev_t *indev;
    lv_obj_t *cover;
    lv_timer_t *timer;
    lv_event_dsc_t *indev_event;
    uint32_t last_activity;
    int fb_fd;
    int saved_power;
    int gesture_fd;
    const struct orb_metadata *gesture_meta;
    const char *gesture_name;
    bool standby;
    bool gesture_armed;
    bool gesture_reported;
    bool fb_error_reported;
} openvela_ui_power_t;

static openvela_ui_power_t g_power;
static bool power_gesture_pending(void);

/* The dshanpi framebuffer is the SPI LCD implementation.  Older BSP
 * revisions expose updatearea but not the standard power ioctl, so retain a
 * board-local backlight fallback for those images. */
#if defined(CONFIG_SPI_LCD_FB) && defined(CONFIG_DRIVERS_SPI)
extern void spi_lcd_set_bl_pin(int val);
#endif

#ifdef CONFIG_UORB

static void power_probe_gesture_sensor(void)
{
    static const struct {
        const char *path;
        const char *name;
        const struct orb_metadata *meta;
    } candidates[] = {
        { "/dev/uorb/sensor_wake_gesture0", "sensor_wake_gesture",
          ORB_ID(sensor_wake_gesture) },
        { "/dev/uorb/sensor_pickup_gesture0", "sensor_pickup_gesture",
          ORB_ID(sensor_pickup_gesture) },
    };
    size_t index;

    for (index = 0; index < sizeof(candidates) / sizeof(candidates[0]);
         index++) {
        if (access(candidates[index].path, F_OK) == 0) {
            g_power.gesture_meta = candidates[index].meta;
            g_power.gesture_name = candidates[index].name;
            fprintf(stderr, "openvela_ui: raise-to-wake source %s\n",
                    g_power.gesture_name);
            return;
        }
    }

    if (!g_power.gesture_reported) {
        fprintf(stderr,
                "openvela_ui: raise-to-wake unavailable "
                "(no wake/pickup sensor node)\n");
        g_power.gesture_reported = true;
    }
}

static void power_gesture_subscribe(void)
{
    if (!g_power.gesture_meta || g_power.gesture_fd >= 0) {
        return;
    }

    g_power.gesture_fd = orb_subscribe_multi(g_power.gesture_meta, 0);
    if (g_power.gesture_fd < 0) {
        fprintf(stderr, "openvela_ui: %s subscribe failed: %d\n",
                g_power.gesture_name, g_power.gesture_fd);
        return;
    }

    /* Drain retained broker samples immediately.  Keeping the low-power
     * gesture subscription open while the screen is lit lets the 1 s
     * watchdog consume active-state events, so the first event after the
     * standby boundary is never mistaken for old data. */
    (void)power_gesture_pending();
}

static void power_gesture_unsubscribe(void)
{
    if (g_power.gesture_fd >= 0) {
        orb_unsubscribe(g_power.gesture_fd);
        g_power.gesture_fd = -1;
    }
    g_power.gesture_armed = false;
}

static bool power_gesture_pending(void)
{
    struct sensor_event event;
    bool wake = false;

    if (g_power.gesture_fd < 0 || !g_power.gesture_meta) {
        return false;
    }

    for (;;) {
        bool updated = false;

        if (orb_check(g_power.gesture_fd, &updated) < 0 || !updated) {
            break;
        }
        if (orb_copy(g_power.gesture_meta, g_power.gesture_fd, &event) < 0) {
            break;
        }
        if (g_power.gesture_armed && event.event != 0U) {
            wake = true;
        }
    }

    g_power.gesture_armed = true;
    return wake;
}

#else

static void power_probe_gesture_sensor(void)
{
    if (!g_power.gesture_reported) {
        fprintf(stderr,
                "openvela_ui: raise-to-wake unavailable "
                "(uORB disabled)\n");
        g_power.gesture_reported = true;
    }
}

static void power_gesture_subscribe(void)
{
}

static void power_gesture_unsubscribe(void)
{
    g_power.gesture_fd = -1;
    g_power.gesture_armed = false;
}

static bool power_gesture_pending(void)
{
    return false;
}

#endif

static void power_set_panel(bool enabled)
{
    int power;
    int ret = -1;

    power = enabled ? g_power.saved_power : 0;
    if (g_power.fb_fd >= 0) {
        ret = ioctl(g_power.fb_fd, FBIOSET_POWER, power);
    }

    if (ret < 0) {
#if defined(CONFIG_SPI_LCD_FB) && defined(CONFIG_DRIVERS_SPI)
        spi_lcd_set_bl_pin(enabled ? 1 : 0);
        if (!g_power.fb_error_reported) {
            fprintf(stderr,
                    "openvela_ui: using SPI backlight standby fallback\n");
            g_power.fb_error_reported = true;
        }
#else
        if (!g_power.fb_error_reported) {
            fprintf(stderr, "openvela_ui: panel power control unavailable\n");
            g_power.fb_error_reported = true;
        }
#endif
    }
}

static void power_wake(bool gesture)
{
    if (!g_power.standby) {
        g_power.last_activity = lv_tick_get();
        return;
    }

    g_power.standby = false;
    g_power.last_activity = lv_tick_get();
    power_set_panel(true);
    openvela_ui_set_low_power(false);

    if (g_power.cover) {
        lv_obj_add_flag(g_power.cover, LV_OBJ_FLAG_HIDDEN);
    }

    if (g_power.display) {
        lv_obj_invalidate(lv_display_get_screen_active(g_power.display));
    }

    fprintf(stderr, "openvela_ui: display wake (%s)\n",
            gesture ? "gesture" : "touch");
}

static void power_enter_standby(void)
{
    if (g_power.standby) {
        return;
    }

    /* Consume an event that happened while the display was still active. */
    (void)power_gesture_pending();
    g_power.standby = true;
    openvela_ui_set_low_power(true);

    if (g_power.cover) {
        lv_obj_clear_flag(g_power.cover, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(g_power.cover);
        lv_obj_invalidate(g_power.cover);
    }

    power_set_panel(false);
    fprintf(stderr,
            "openvela_ui: display standby (background services remain active)\n");
}

static void power_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    if (!g_power.gesture_meta) {
        power_probe_gesture_sensor();
    }
    if (g_power.gesture_fd < 0) {
        power_gesture_subscribe();
    }

    if (g_power.standby) {
        if (power_gesture_pending()) {
            power_wake(true);
        }
        return;
    }

    /* Keep the broker cursor current while lit. */
    (void)power_gesture_pending();

    if (lv_tick_elaps(g_power.last_activity) >= UI_STANDBY_TIMEOUT_MS) {
        power_enter_standby();
    }
}

static void power_indev_cb(lv_event_t *event)
{
    if (lv_event_get_code(event) != LV_EVENT_PRESSED) {
        return;
    }

    if (g_power.standby) {
        power_wake(false);
        /* Input-device events run before widget events.  Consume the wake
         * touch so it cannot also activate the control below it. */
        lv_indev_stop_processing(g_power.indev);
        lv_indev_wait_release(g_power.indev);
    } else {
        g_power.last_activity = lv_tick_get();
    }
}

void openvela_ui_power_init(lv_display_t *display, lv_indev_t *indev)
{
    int current_power = 0;

    lv_memzero(&g_power, sizeof(g_power));
    g_power.display = display;
    g_power.indev = indev;
    g_power.fb_fd = -1;
    g_power.gesture_fd = -1;
    g_power.saved_power = UI_LCD_MAX_POWER;
    g_power.last_activity = lv_tick_get();

    g_power.fb_fd = open("/dev/fb0", O_RDWR);
    if (g_power.fb_fd >= 0 &&
        ioctl(g_power.fb_fd, FBIOGET_POWER, &current_power) == 0 &&
        current_power > 0) {
        g_power.saved_power = current_power;
    }

    g_power.cover = lv_obj_create(
        lv_display_get_layer_top(display ? display : lv_display_get_default()));
    lv_obj_remove_style_all(g_power.cover);
    lv_obj_set_size(g_power.cover, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(g_power.cover, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_power.cover, LV_OPA_COVER, 0);
    lv_obj_add_flag(g_power.cover, LV_OBJ_FLAG_CLICKABLE |
                    LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_power.cover, LV_OBJ_FLAG_SCROLLABLE);

    if (g_power.indev) {
        uint32_t event_count;

        lv_indev_add_event_cb(g_power.indev, power_indev_cb,
                              LV_EVENT_PRESSED, NULL);
        event_count = lv_indev_get_event_count(g_power.indev);
        if (event_count > 0U) {
            g_power.indev_event = lv_indev_get_event_dsc(
                g_power.indev, event_count - 1U);
        }
    }

    power_probe_gesture_sensor();
    power_gesture_subscribe();
    g_power.timer = lv_timer_create(power_timer_cb, UI_POWER_POLL_MS, NULL);
}

void openvela_ui_power_deinit(void)
{
    uint32_t index;

    if (g_power.standby) {
        power_wake(false);
    }

    if (g_power.timer) {
        lv_timer_delete(g_power.timer);
        g_power.timer = NULL;
    }

    power_gesture_unsubscribe();

    if (g_power.indev && g_power.indev_event) {
        for (index = 0; index < lv_indev_get_event_count(g_power.indev);
             index++) {
            if (lv_indev_get_event_dsc(g_power.indev, index) ==
                g_power.indev_event) {
                lv_indev_remove_event(g_power.indev, index);
                break;
            }
        }
        g_power.indev_event = NULL;
    }

    if (g_power.cover) {
        lv_obj_delete(g_power.cover);
        g_power.cover = NULL;
    }

    if (g_power.fb_fd >= 0) {
        close(g_power.fb_fd);
        g_power.fb_fd = -1;
    }
}

#else

void openvela_ui_power_init(lv_display_t *display, lv_indev_t *indev)
{
    LV_UNUSED(display);
    LV_UNUSED(indev);
}

void openvela_ui_power_deinit(void)
{
}

#endif
