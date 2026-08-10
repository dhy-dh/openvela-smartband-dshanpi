#include <nuttx/config.h>

#include <lvgl/lvgl.h>
#include <nuttx/audio/audio.h>
#include <system/nxplayer.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "openvela_ui.h"
#include "openvela_ui_city_data.h"
#include "openvela_ui_sport.h"
#include "openvela_ui_sync.h"
#include "openvela_ui_weather.h"

#ifdef CONFIG_ARCH_SUN8IW20
/* Exported by the R528 tiny-ALSA control library.  Keep this declaration
 * local instead of adding a private Allwinner include path to a public app. */
extern int snd_ctl_set_bynum(const char *name, unsigned int elem_num,
                             unsigned int value);
#endif

#define UI_DESIGN_WIDTH 432
#define UI_DESIGN_HEIGHT 514
#if defined(CONFIG_LV_FONT_MONTSERRAT_32)
#  define UI_SYMBOL_FONT (&lv_font_montserrat_32)
#elif defined(CONFIG_LV_FONT_MONTSERRAT_30)
#  define UI_SYMBOL_FONT (&lv_font_montserrat_30)
#else
#  define UI_SYMBOL_FONT LV_FONT_DEFAULT
#endif
#define UI_PAGE_COUNT 6
#define UI_TILE_COUNT 8
#define UI_TILE_FIRST_REAL 1
#define UI_TILE_LAST_REAL 6
#define UI_DATA_ROOT CONFIG_OPENVELA_UI_DATA_ROOT
#define UI_FONT UI_DATA_ROOT "/ui-font.ttf"
#define UI_ICON_WEATHER UI_DATA_ROOT "/icons/weather.png"
#define UI_ICON_SPORT UI_DATA_ROOT "/icons/sport.png"
#define UI_ICON_HEALTH UI_DATA_ROOT "/icons/health.png"
#define UI_ICON_MUSIC UI_DATA_ROOT "/icons/music.png"
#define UI_ICON_NOTIFICATIONS UI_DATA_ROOT "/icons/notifications.png"
#define UI_WEATHER_ROOT UI_DATA_ROOT "/weather-icons"
#define UI_WEATHER_CITY_STORE "/data/etc/openvela_ui/cities.conf"
#define UI_WEATHER_CITY_STORE_TMP "/data/etc/openvela_ui/cities.tmp"
#define UI_MUSIC_ROOT UI_DATA_ROOT "/music"
#define UI_MUSIC_LOGO UI_MUSIC_ROOT "/player-logo.png"
#define UI_MUSIC_ICON_PREV UI_MUSIC_ROOT "/icons/prev.png"
#define UI_MUSIC_ICON_NEXT UI_MUSIC_ROOT "/icons/next.png"
#define UI_MUSIC_ICON_PLAY UI_MUSIC_ROOT "/icons/play.png"
#define UI_MUSIC_ICON_PAUSE UI_MUSIC_ROOT "/icons/pause.png"
#define UI_MUSIC_ICON_VOLUME UI_MUSIC_ROOT "/icons/volume.png"
#define UI_MUSIC_ICON_LIST UI_MUSIC_ROOT "/icons/play-list.png"
#define UI_MUSIC_ICON_MINUS UI_MUSIC_ROOT "/icons/minus.png"
#define UI_MUSIC_ICON_PLUS UI_MUSIC_ROOT "/icons/plus.png"
#define UI_MUSIC_ICON_CANCEL UI_MUSIC_ROOT "/icons/cancel.png"
#define UI_MUSIC_BADGE_1 UI_MUSIC_ROOT "/icons/track-01.png"
#define UI_MUSIC_BADGE_2 UI_MUSIC_ROOT "/icons/track-02.png"
#define UI_MUSIC_BADGE_3 UI_MUSIC_ROOT "/icons/track-03.png"
#define UI_BACKGROUND_COUNT 5
#define UI_ACTION_COUNT 6
#define UI_MAX_ACTION_FRAMES 96
#define UI_IMAGE_CACHE_SIZE (8U * 1024U * 1024U)
#define UI_FRAME_PATH_SIZE 128
#define UI_SWIPE_THRESHOLD 52
#define UI_MAX_THEME_LABELS 32
#define UI_MAX_THEME_SHAPES 24
#define UI_HEALTH_HISTORY_COUNT 7
#define UI_HEALTH_MEASURE_TICKS 12
#define UI_MUSIC_TRACK_COUNT 3
#define UI_MUSIC_FADE_STEPS 5
#define UI_MUSIC_START_TIMEOUT_TICKS 75
#define UI_MUSIC_PCM_RATE 24000
#define UI_MUSIC_PCM_CHANNELS 1
#define UI_MUSIC_PCM_BITS 16
#define UI_MUSIC_DAC_MAX 180U
#define UI_MUSIC_PCM_BYTES_PER_SECOND \
    (UI_MUSIC_PCM_RATE * UI_MUSIC_PCM_CHANNELS * (UI_MUSIC_PCM_BITS / 8))
#define UI_WEATHER_BASE_CITY_COUNT 5
#define UI_WEATHER_CITIES_PER_PAGE 4
#define UI_WEATHER_BASE_PAGE_COUNT 2
#define UI_WEATHER_ADD_PAGE_INDEX UI_WEATHER_BASE_PAGE_COUNT
#define UI_WEATHER_CUSTOM_FIRST_PAGE (UI_WEATHER_ADD_PAGE_INDEX + 1)
#define UI_WEATHER_MAX_CUSTOM_CITIES 12
#define UI_WEATHER_CITY_NAME_SIZE 64
#define UI_WEATHER_AREA_SIZE 96
#define UI_WEATHER_ROLLER_OPTIONS_SIZE 3072
#define UI_WEATHER_REFRESH_TICKS 600
#define UI_WEATHER_RETRY_TICKS 30

/* NxPlayer's public context exposes state, but its state constants are
 * private to nxplayer.c.  Keep these values local so completion can be
 * observed without adding a second player thread or a media daemon. */
#define UI_NXPLAYER_IDLE 0
#define UI_NXPLAYER_PLAYING 1
#define UI_NXPLAYER_PAUSED 2

enum ui_overlay_page {
    UI_OVERLAY_NONE = 0,
    UI_OVERLAY_CUSTOMIZE,
    UI_OVERLAY_BACKGROUNDS,
    UI_OVERLAY_ACTIONS,
    UI_OVERLAY_HEALTH,
    UI_OVERLAY_MUSIC,
    UI_OVERLAY_WEATHER,
    UI_OVERLAY_SPORT,
};

enum ui_health_page {
    UI_HEALTH_HEART_RATE = 0,
    UI_HEALTH_HEART_HISTORY,
    UI_HEALTH_BLOOD_PRESSURE,
    UI_HEALTH_PRESSURE_HISTORY,
    UI_HEALTH_PAGE_COUNT,
};

enum ui_music_page {
    UI_MUSIC_PLAYER = 0,
    UI_MUSIC_VOLUME,
    UI_MUSIC_LIST,
};

enum ui_music_state {
    UI_MUSIC_IDLE = 0,
    UI_MUSIC_PREPARING,
    UI_MUSIC_PLAYING,
    UI_MUSIC_PAUSING,
    UI_MUSIC_PAUSED,
    UI_MUSIC_ERROR,
};

enum ui_music_fade {
    UI_MUSIC_FADE_NONE = 0,
    UI_MUSIC_FADE_OUT_PAUSE,
    UI_MUSIC_FADE_IN_RESUME,
};

enum ui_page_id {
    UI_PAGE_HOME = 0,
    UI_PAGE_WEATHER,
    UI_PAGE_SPORT,
    UI_PAGE_HEALTH,
    UI_PAGE_MUSIC,
    UI_PAGE_NOTIFICATIONS,
};

enum ui_weather_stage {
    UI_WEATHER_CITIES = 0,
    UI_WEATHER_DETAIL,
};

enum ui_weather_detail_page {
    UI_WEATHER_NOW = 0,
    UI_WEATHER_FORECAST,
};

enum ui_weather_switch_target {
    UI_WEATHER_SWITCH_CITIES = 0,
    UI_WEATHER_SWITCH_NOW,
    UI_WEATHER_SWITCH_FORECAST,
};

typedef struct {
    const char *folder;
    uint8_t frame_count;
    uint16_t duration;
    uint8_t preview_frame;
    const uint16_t *timing;
    uint8_t timing_count;
    uint8_t timing_tick;
} ui_action_t;

typedef struct {
    int16_t primary;
    int16_t secondary;
    int16_t pulse;
    time_t measured_at;
} ui_health_record_t;

typedef struct {
    const char *name;
    const char *artist;
    const char *path;
} ui_music_track_t;

typedef struct {
    char location_id[16];
    char name[UI_WEATHER_CITY_NAME_SIZE];
    char administrative_area[UI_WEATHER_AREA_SIZE];
} ui_weather_city_t;

typedef struct {
    lv_obj_t *viewport;
    lv_obj_t *background_img;
    lv_obj_t *tileview;
    lv_obj_t *tile[UI_TILE_COUNT];
    lv_obj_t *cat_layer;
    lv_obj_t *cat_anim;
    lv_obj_t *overlay;
    lv_obj_t *overlay_time_label;
    lv_obj_t *health_view;
    lv_obj_t *health_value_label;
    lv_obj_t *health_secondary_label;
    lv_obj_t *health_pulse_label;
    lv_obj_t *health_state_label;
    lv_obj_t *health_status_label;
    lv_obj_t *health_hint_label;
    lv_obj_t *health_heart_icon;
    lv_obj_t *music_view;
    lv_obj_t *music_title_label;
    lv_obj_t *music_artist_label;
    lv_obj_t *music_status_label;
    lv_obj_t *music_progress_label;
    lv_obj_t *music_progress_bar;
    lv_obj_t *music_play_icon;
    lv_obj_t *music_volume_label;
    lv_obj_t *music_volume_bar;
    openvela_ui_sport_t *sport;
    lv_obj_t *background_radios[UI_BACKGROUND_COUNT];
    lv_obj_t *action_radios[UI_ACTION_COUNT];
    lv_obj_t *theme_labels[UI_MAX_THEME_LABELS];
    lv_obj_t *theme_fills[UI_MAX_THEME_SHAPES];
    lv_obj_t *theme_borders[UI_MAX_THEME_SHAPES];
    lv_obj_t *dots[UI_PAGE_COUNT];
    lv_obj_t *time_label[2];
    lv_obj_t *date_label[2];
    lv_timer_t *clock_timer;
    lv_timer_t *health_measure_timer;
    lv_timer_t *music_timer;
    lv_font_t *font_small;
    lv_font_t *font_body;
    lv_font_t *font_title;
    lv_font_t *font_clock;
    int32_t width;
    int32_t height;
    int32_t scale_1000;
    int8_t current_page;
    int8_t overlay_page;
    int8_t selected_background;
    int8_t selected_action;
    int8_t health_page;
    int8_t music_page;
    int8_t music_track;
    uint8_t health_measure_ticks;
    uint8_t heart_history_count;
    uint8_t pressure_history_count;
    bool health_measuring;
    struct nxplayer_s *music_player;
    uint8_t music_state;
    uint8_t music_volume;
    uint8_t music_applied_volume;
    uint8_t music_fade;
    uint8_t music_fade_step;
    uint8_t music_poll_ticks;
    unsigned int music_position_ms;
    unsigned int music_duration_ms;
    uint64_t music_last_clock_ms;
    int16_t heart_rate;
    int16_t target_heart_rate;
    int16_t systolic;
    int16_t diastolic;
    int16_t pulse;
    int16_t target_systolic;
    int16_t target_diastolic;
    int16_t target_pulse;
    uint16_t health_heart_scale;
    ui_health_record_t heart_history[UI_HEALTH_HISTORY_COUNT];
    ui_health_record_t pressure_history[UI_HEALTH_HISTORY_COUNT];
    uint8_t theme_label_count;
    uint8_t theme_fill_count;
    uint8_t theme_border_count;
    lv_point_t touch_start;
    bool touch_tracking;
    const void *active_frames[UI_MAX_ACTION_FRAMES];
    char frame_paths[UI_MAX_ACTION_FRAMES][UI_FRAME_PATH_SIZE];
    char preview_path[UI_FRAME_PATH_SIZE];
    bool assets_ready;
    lv_obj_t *home_weather_image[2];
    lv_obj_t *home_weather_temp_label[2];
    lv_obj_t *home_weather_desc_label[2];
    lv_obj_t *weather_view;
    lv_obj_t *weather_header_name_label;
    lv_obj_t *weather_header_metadata_label;
    lv_obj_t *weather_icon_image;
    lv_obj_t *weather_temperature_label;
    lv_obj_t *weather_condition_label;
    lv_obj_t *weather_feels_label;
    lv_obj_t *weather_humidity_label;
    lv_obj_t *weather_visibility_label;
    lv_obj_t *weather_update_label;
    lv_obj_t *weather_forecast_range_label;
    lv_obj_t *weather_forecast_weekday_label[
        OPENVELA_UI_WEATHER_FORECAST_DAYS];
    lv_obj_t *weather_forecast_icon_image[
        OPENVELA_UI_WEATHER_FORECAST_DAYS];
    lv_obj_t *weather_forecast_text_label[
        OPENVELA_UI_WEATHER_FORECAST_DAYS];
    lv_obj_t *weather_forecast_temperature_label[
        OPENVELA_UI_WEATHER_FORECAST_DAYS];
    lv_obj_t *weather_roller_province;
    lv_obj_t *weather_roller_city;
    lv_obj_t *weather_roller_district;
    lv_obj_t *weather_picker_summary_label;
    lv_obj_t *weather_picker_status_label;
    lv_obj_t *weather_delete_modal;
    lv_timer_t *weather_timer;
    uint32_t weather_seen_revision;
    uint16_t weather_poll_ticks;
    int8_t weather_stage;
    int8_t weather_detail_page;
    int8_t weather_city_page;
    int16_t weather_selected_city;
    int16_t weather_request_city;
    int16_t weather_delete_city;
    uint16_t weather_picker_province;
    uint16_t weather_picker_city;
    uint16_t weather_picker_district;
    uint8_t weather_custom_city_count;
    bool weather_switch_pending;
    bool weather_transitioning;
    bool weather_picker_updating;
    bool weather_modal_active;
    bool sport_transitioning;
    bool sport_home_visible;
    bool low_power;
    bool home_weather_valid;
    char weather_province_options[UI_WEATHER_ROLLER_OPTIONS_SIZE];
    char weather_city_options[UI_WEATHER_ROLLER_OPTIONS_SIZE];
    char weather_district_options[UI_WEATHER_ROLLER_OPTIONS_SIZE];
    ui_weather_city_t weather_custom_cities[UI_WEATHER_MAX_CUSTOM_CITIES];
    struct openvela_ui_weather_snapshot_s weather_snapshot;
    struct openvela_ui_weather_snapshot_s home_weather_snapshot;
} ui_context_t;

static ui_context_t g_ui;

static const ui_music_track_t g_music_tracks[UI_MUSIC_TRACK_COUNT] = {
    {"一路向北", "周杰伦", UI_MUSIC_ROOT "/tracks/track-01.pcm"},
    {"稻香", "周杰伦", UI_MUSIC_ROOT "/tracks/track-02.pcm"},
    {"等你下课", "周杰伦、杨瑞代",
     UI_MUSIC_ROOT "/tracks/track-03.pcm"},
};

static const char *g_music_badges[UI_MUSIC_TRACK_COUNT] = {
    UI_MUSIC_BADGE_1,
    UI_MUSIC_BADGE_2,
    UI_MUSIC_BADGE_3,
};

static const ui_weather_city_t
g_weather_base_cities[UI_WEATHER_BASE_CITY_COUNT] = {
    {"101010100", "北京", "北京"},
    {"101020100", "上海", "上海"},
    {"101280101", "广州", "广东"},
    {"101280601", "深圳", "广东"},
    {"101200101", "武汉", "湖北"},
};

static const int8_t g_tile_page[UI_TILE_COUNT] = {
    UI_PAGE_NOTIFICATIONS,
    UI_PAGE_HOME,
    UI_PAGE_WEATHER,
    UI_PAGE_SPORT,
    UI_PAGE_HEALTH,
    UI_PAGE_MUSIC,
    UI_PAGE_NOTIFICATIONS,
    UI_PAGE_HOME,
};

static const char *g_background_paths[UI_BACKGROUND_COUNT] = {
    UI_DATA_ROOT "/backgrounds/sky-blue.png",
    UI_DATA_ROOT "/backgrounds/mint-green.png",
    UI_DATA_ROOT "/backgrounds/warm-beige.png",
    UI_DATA_ROOT "/backgrounds/sunset-coral.png",
    UI_DATA_ROOT "/backgrounds/dark-purple.png",
};

static const uint16_t g_balloon_timing[] = {
    120, 10, 110, 240, 10, 230, 10, 110, 120, 120, 120,
    120, 120, 120, 120, 240, 120, 10, 110, 240, 120, 10,
    110, 120, 120, 240, 10, 110, 120, 120, 120, 120, 120,
};

static const uint16_t g_laugh_timing[] = {
    40, 80, 40, 80, 40, 80, 40, 80, 40, 80, 710, 40, 40,
    80, 40, 80, 40, 80, 40, 80, 40, 80, 40, 40, 40, 40,
    40, 40, 40, 30, 40, 40,
};

static const ui_action_t g_actions[UI_ACTION_COUNT] = {
    {"cat", 34, 3400, 18, NULL, 0, 0},
    {"shy-wave", 19, 2300, 10, NULL, 0, 0},
    {"phone-rest", 35, 4200, 18, NULL, 0, 0},
    {"balloon-rise", 33, 0, 17, g_balloon_timing,
     sizeof(g_balloon_timing) / sizeof(g_balloon_timing[0]), 60},
    {"laugh", 32, 0, 17, g_laugh_timing,
     sizeof(g_laugh_timing) / sizeof(g_laugh_timing[0]), 50},
    {"toilet-break", 46, 2300, 23, NULL, 0, 0},
};

static int32_t sx(int32_t value)
{
    return (value * g_ui.scale_1000 + 500) / 1000;
}

static lv_obj_t *make_box(lv_obj_t *parent, int32_t x, int32_t y,
                          int32_t width, int32_t height, uint32_t color,
                          int32_t radius, lv_opa_t opacity)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, sx(x), sx(y));
    lv_obj_set_size(obj, sx(width), sx(height));
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, opacity, 0);
    lv_obj_set_style_radius(obj, sx(radius), 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return obj;
}

static lv_obj_t *make_label(lv_obj_t *parent, const char *text,
                            const lv_font_t *font, uint32_t color,
                            int32_t x, int32_t y, int32_t width,
                            lv_text_align_t align)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_remove_style_all(label);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, sx(x), sx(y));
    lv_obj_set_width(label, sx(width));
    lv_obj_set_style_text_font(label, font ? font : LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, align, 0);
    return label;
}

static void register_theme_label(lv_obj_t *obj)
{
    if (g_ui.theme_label_count < UI_MAX_THEME_LABELS) {
        g_ui.theme_labels[g_ui.theme_label_count++] = obj;
    }
}

static void register_theme_fill(lv_obj_t *obj)
{
    if (g_ui.theme_fill_count < UI_MAX_THEME_SHAPES) {
        g_ui.theme_fills[g_ui.theme_fill_count++] = obj;
    }
}

static void register_theme_border(lv_obj_t *obj)
{
    if (g_ui.theme_border_count < UI_MAX_THEME_SHAPES) {
        g_ui.theme_borders[g_ui.theme_border_count++] = obj;
    }
}

static uint32_t theme_foreground(void)
{
    static const uint32_t colors[UI_BACKGROUND_COUNT] = {
        0x142a65, 0x5b362c, 0x5b362c, 0x7a3156, 0xffffff
    };

    return colors[g_ui.selected_background];
}

static void apply_theme_foreground(void)
{
    lv_color_t color = lv_color_hex(theme_foreground());
    int index;

    for (index = 0; index < g_ui.theme_label_count; index++) {
        lv_obj_set_style_text_color(g_ui.theme_labels[index], color, 0);
    }
    for (index = 0; index < g_ui.theme_fill_count; index++) {
        lv_obj_set_style_bg_color(g_ui.theme_fills[index], color, 0);
    }
    for (index = 0; index < g_ui.theme_border_count; index++) {
        lv_obj_set_style_border_color(g_ui.theme_borders[index], color, 0);
    }
}

static lv_obj_t *make_asset_image(lv_obj_t *parent, const char *path,
                                  int32_t x, int32_t y,
                                  int32_t source_size,
                                  int32_t display_size)
{
    lv_obj_t *image;
    uint32_t scale;

    if (access(path, R_OK) != 0) {
        return NULL;
    }

    image = lv_image_create(parent);
    lv_image_set_src(image, path);
    lv_image_set_pivot(image, 0, 0);
    scale = (uint32_t)((sx(display_size) * 256 + source_size / 2) /
                       source_size);
    lv_image_set_scale(image, scale);
    lv_obj_set_pos(image, sx(x), sx(y));
    lv_obj_clear_flag(image, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    return image;
}

static lv_obj_t *make_round_preview(lv_obj_t *parent, const char *path,
                                    int32_t x, int32_t y,
                                    int32_t source_width, int32_t size,
                                    uint32_t fallback_color)
{
    lv_obj_t *clip = make_box(parent, x, y, size, size,
                              fallback_color, size / 2, LV_OPA_COVER);

    lv_obj_set_style_clip_corner(clip, true, 0);
    if (access(path, R_OK) == 0) {
        lv_obj_t *image = lv_image_create(clip);
        lv_image_set_src(image, path);
        lv_image_set_pivot(image, 0, 0);
        lv_image_set_scale(image,
            (uint32_t)(sx(size) * 256 / source_width));
        lv_obj_set_pos(image, 0, 0);
        lv_obj_clear_flag(image,
                          LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    }

    return clip;
}

static const char *page_title(int page)
{
    static const char *titles_cn[UI_PAGE_COUNT] = {
        "主页", "天气", "运动", "健康", "音乐", "通知"
    };
    static const char *titles_en[UI_PAGE_COUNT] = {
        "HOME", "WEATHER", "SPORT", "HEALTH", "MUSIC", "NOTICES"
    };

    return g_ui.font_title ? titles_cn[page] : titles_en[page];
}

static void create_night_fallback(lv_obj_t *page)
{
    lv_obj_t *window = make_box(page, 274, 46, 116, 224, 0x231951, 54, LV_OPA_80);
    lv_obj_set_style_border_color(window, lv_color_hex(0x5d3f8f), 0);
    lv_obj_set_style_border_width(window, sx(5), 0);
    make_box(window, 71, 24, 28, 28, 0xffd978, 20, LV_OPA_COVER);
    make_box(window, 15, 113, 48, 24, 0x7656b7, 18, LV_OPA_COVER);
    make_box(window, 54, 151, 54, 26, 0x7656b7, 18, LV_OPA_COVER);
    make_box(page, 28, 385, 376, 72, 0x38205e, 36, LV_OPA_COVER);
}

static void create_background(lv_obj_t *page)
{
    lv_obj_set_style_bg_color(page, lv_color_hex(0x07163d), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);

    if (g_ui.assets_ready) {
        g_ui.background_img = lv_image_create(page);
        lv_image_set_src(g_ui.background_img,
                         g_background_paths[g_ui.selected_background]);
        lv_image_set_pivot(g_ui.background_img, 0, 0);
        lv_image_set_scale(g_ui.background_img,
                           (uint32_t)(g_ui.width * 256 / UI_DESIGN_WIDTH));
        lv_obj_set_pos(g_ui.background_img, 0, 0);
        lv_obj_clear_flag(g_ui.background_img, LV_OBJ_FLAG_CLICKABLE);
    } else {
        create_night_fallback(page);
    }
}

static void update_clock(lv_timer_t *timer)
{
    time_t now;
    struct tm local;
    char time_text[16];
    char date_text[48];
    int i;
    static const char *week_cn[] = {
        "周日", "周一", "周二", "周三", "周四", "周五", "周六"
    };
    static const char *week_en[] = {
        "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
    };

    LV_UNUSED(timer);
    now = time(NULL);
    localtime_r(&now, &local);
    snprintf(time_text, sizeof(time_text), "%02d:%02d", local.tm_hour, local.tm_min);
    if (g_ui.font_body) {
        snprintf(date_text, sizeof(date_text), "%02d月%02d日  %s",
                 local.tm_mon + 1, local.tm_mday, week_cn[local.tm_wday]);
    } else {
        snprintf(date_text, sizeof(date_text), "%02d/%02d  %s",
                 local.tm_mon + 1, local.tm_mday, week_en[local.tm_wday]);
    }

    for (i = 0; i < 2; i++) {
        if (g_ui.time_label[i] && lv_obj_is_valid(g_ui.time_label[i])) {
            lv_label_set_text(g_ui.time_label[i], time_text);
        }
        if (g_ui.date_label[i] && lv_obj_is_valid(g_ui.date_label[i])) {
            lv_label_set_text(g_ui.date_label[i], date_text);
        }
    }

    if (g_ui.overlay_time_label &&
        lv_obj_is_valid(g_ui.overlay_time_label)) {
        lv_label_set_text(g_ui.overlay_time_label, time_text);
    }
}

static void create_battery(lv_obj_t *page)
{
    lv_obj_t *shell = make_box(page, 320, 51, 42, 22, 0xffffff, 5, LV_OPA_TRANSP);
    lv_obj_t *level;
    lv_obj_t *cap;
    lv_obj_t *label;

    lv_obj_set_style_border_color(shell, lv_color_white(), 0);
    lv_obj_set_style_border_width(shell, sx(3), 0);
    register_theme_border(shell);
    level = make_box(shell, 4, 4, 27, 8, 0xffffff, 2, LV_OPA_COVER);
    cap = make_box(page, 364, 57, 4, 10, 0xffffff, 2, LV_OPA_COVER);
    label = make_label(page, "82%", g_ui.font_small, 0xffffff,
                       371, 50, 55, LV_TEXT_ALIGN_LEFT);
    register_theme_fill(level);
    register_theme_fill(cap);
    register_theme_label(label);
}

static void create_home_page(lv_obj_t *page, int home_slot)
{
    lv_obj_t *weather_icon;

    g_ui.time_label[home_slot] = make_label(page, "--:--", g_ui.font_clock,
                                            0xffffff, 30, 42, 260,
                                            LV_TEXT_ALIGN_LEFT);
    g_ui.date_label[home_slot] = make_label(page, "", g_ui.font_body,
                                            0xffffff, 34, 132, 240,
                                            LV_TEXT_ALIGN_LEFT);
    register_theme_label(g_ui.time_label[home_slot]);
    register_theme_label(g_ui.date_label[home_slot]);
    create_battery(page);

    weather_icon = make_asset_image(page, UI_ICON_WEATHER,
                                    255, 96, 128, 72);
    g_ui.home_weather_image[home_slot] = weather_icon;
    if (!weather_icon) {
        lv_obj_t *weather = make_box(page, 276, 103, 62, 62,
                                     0x4b7fd9, 31, LV_OPA_90);
        make_label(weather, LV_SYMBOL_GPS, UI_SYMBOL_FONT, 0xffd24d,
                   0, 13, 62, LV_TEXT_ALIGN_CENTER);
    }
    g_ui.home_weather_temp_label[home_slot] =
        make_label(page, "--°", g_ui.font_title, 0xffffff,
                   337, 103, 88, LV_TEXT_ALIGN_CENTER);
    register_theme_label(g_ui.home_weather_temp_label[home_slot]);
    g_ui.home_weather_desc_label[home_slot] = make_label(page,
        g_ui.font_body ? "北京 · 更新中" : "BEIJING · LOADING",
        g_ui.font_small, 0xffffff, 274, 166, 150,
        LV_TEXT_ALIGN_CENTER);
    register_theme_label(g_ui.home_weather_desc_label[home_slot]);
    update_clock(NULL);
}

static void create_marker_fallback(lv_obj_t *page, int page_id)
{
    static const uint32_t accents[UI_PAGE_COUNT] = {
        0x496fe3, 0x4c87ed, 0xff8a35, 0xf05262, 0x2b9de0, 0x416df1
    };
    static const char *symbols[UI_PAGE_COUNT] = {
        LV_SYMBOL_HOME, LV_SYMBOL_GPS, LV_SYMBOL_PLAY,
        LV_SYMBOL_EYE_OPEN, LV_SYMBOL_AUDIO, LV_SYMBOL_BELL
    };
    lv_obj_t *card;

    card = make_box(page, 142, 105, 148, 148, accents[page_id], 48, LV_OPA_COVER);
    lv_obj_set_style_shadow_color(card, lv_color_hex(accents[page_id]), 0);
    lv_obj_set_style_shadow_opa(card, LV_OPA_40, 0);
    lv_obj_set_style_shadow_width(card, sx(24), 0);
    make_label(card, symbols[page_id], UI_SYMBOL_FONT, 0xffffff,
               0, 48, 148, LV_TEXT_ALIGN_CENTER);

    if (page_id == UI_PAGE_WEATHER) {
        make_box(card, 28, 30, 42, 42, 0xffd24d, 22, LV_OPA_COVER);
        make_box(card, 55, 74, 72, 30, 0xf8fbff, 18, LV_OPA_COVER);
    } else if (page_id == UI_PAGE_HEALTH) {
        make_label(card, "♥", g_ui.font_title, 0xffffff,
                   0, 37, 148, LV_TEXT_ALIGN_CENTER);
    }
}

static void create_module_icon(lv_obj_t *page, int page_id)
{
    lv_obj_t *card;
    lv_obj_t *marker = NULL;
    lv_obj_t *label;

    label = make_label(page, page_title(page_id), g_ui.font_title, 0xffffff,
                       76, 34, 280, LV_TEXT_ALIGN_CENTER);
    register_theme_label(label);

    switch (page_id) {
    case UI_PAGE_WEATHER:
        marker = make_asset_image(page, UI_ICON_WEATHER,
                                  150, 96, 128, 132);
        break;

    case UI_PAGE_SPORT:
        marker = make_asset_image(page, UI_ICON_SPORT,
                                  141, 96, 256, 150);
        break;

    case UI_PAGE_HEALTH:
        marker = make_asset_image(page, UI_ICON_HEALTH,
                                  133, 92, 256, 166);
        break;

    case UI_PAGE_MUSIC:
        card = make_box(page, 150, 105, 132, 132,
                        0xffffff, 66, LV_OPA_90);
        lv_obj_set_style_border_color(card, lv_color_hex(0x2b9de0), 0);
        lv_obj_set_style_border_width(card, sx(4), 0);
        marker = make_asset_image(card, UI_ICON_MUSIC,
                                  12, 12, 144, 100);
        if (!marker) {
            lv_obj_delete(card);
        }
        break;

    case UI_PAGE_NOTIFICATIONS:
        card = make_box(page, 142, 105, 148, 148,
                        0x416df1, 46, LV_OPA_COVER);
        marker = make_asset_image(card, UI_ICON_NOTIFICATIONS,
                                  19, 19, 96, 110);
        if (!marker) {
            lv_obj_delete(card);
        }
        break;

    default:
        break;
    }

    if (!marker) {
        create_marker_fallback(page, page_id);
    }

    label = make_label(page,
        page_id == UI_PAGE_WEATHER ?
        (g_ui.font_body ? "下滑选择城市" : "SWIPE DOWN FOR CITIES") :
        page_id == UI_PAGE_SPORT ?
        (g_ui.font_body ? "上下滑动进入" : "SWIPE VERTICALLY") :
        (g_ui.font_body ? "左右滑动切换" : "SWIPE LEFT OR RIGHT"),
        g_ui.font_small, 0xffffff, 96, 462, 240, LV_TEXT_ALIGN_CENTER);
    register_theme_label(label);
}

static void populate_tile(lv_obj_t *tile, int page_id, int home_slot)
{
    if (page_id == UI_PAGE_HOME) {
        create_home_page(tile, home_slot);
    } else {
        create_module_icon(tile, page_id);
    }
}

static void create_vector_cat(lv_obj_t *parent)
{
    lv_obj_t *body = make_box(parent, 34, 58, 132, 132, 0xf7f3e8, 58, LV_OPA_COVER);
    lv_obj_set_style_border_color(body, lv_color_hex(0x6d4034), 0);
    lv_obj_set_style_border_width(body, sx(5), 0);
    lv_obj_t *head = make_box(parent, 42, 23, 116, 94, 0xd9b68f, 42, LV_OPA_COVER);
    lv_obj_set_style_border_color(head, lv_color_hex(0x6d4034), 0);
    lv_obj_set_style_border_width(head, sx(5), 0);
    make_box(parent, 40, 17, 34, 42, 0xd9b68f, 8, LV_OPA_COVER);
    make_box(parent, 128, 17, 34, 42, 0xd9b68f, 8, LV_OPA_COVER);
    make_box(parent, 70, 61, 12, 30, 0x3154a4, 6, LV_OPA_COVER);
    make_box(parent, 118, 61, 12, 30, 0x3154a4, 6, LV_OPA_COVER);
    make_box(parent, 43, 124, 48, 22, 0xffffff, 12, LV_OPA_COVER);
    make_box(parent, 109, 124, 48, 22, 0xffffff, 12, LV_OPA_COVER);
}

static int build_action_frames(int action_index, uint32_t *duration)
{
    const ui_action_t *action = &g_actions[action_index];
    int output = 0;
    int frame;

    if (action->timing) {
        for (frame = 0; frame < action->timing_count; frame++) {
            int repeat = (action->timing[frame] + action->timing_tick / 2) /
                         action->timing_tick;
            int index;

            if (repeat < 1) {
                repeat = 1;
            }

            for (index = 0;
                 index < repeat && output < UI_MAX_ACTION_FRAMES;
                 index++, output++) {
                snprintf(g_ui.frame_paths[output], UI_FRAME_PATH_SIZE,
                         UI_DATA_ROOT "/actions/%s/frame-%02d.png",
                         action->folder, frame + 1);
                g_ui.active_frames[output] = g_ui.frame_paths[output];
            }
        }
        *duration = output * action->timing_tick;
    } else {
        for (frame = 0;
             frame < action->frame_count && output < UI_MAX_ACTION_FRAMES;
             frame++, output++) {
            if (action_index == 0) {
                snprintf(g_ui.frame_paths[output], UI_FRAME_PATH_SIZE,
                         UI_DATA_ROOT "/cat/frame-%02d.png", frame + 1);
            } else {
                snprintf(g_ui.frame_paths[output], UI_FRAME_PATH_SIZE,
                         UI_DATA_ROOT "/actions/%s/frame-%02d.png",
                         action->folder, frame + 1);
            }
            g_ui.active_frames[output] = g_ui.frame_paths[output];
        }
        *duration = action->duration;
    }

    return output;
}

static void apply_action(int action_index)
{
    uint32_t duration;
    int frame_count;

    if (!g_ui.cat_anim || action_index < 0 ||
        action_index >= UI_ACTION_COUNT) {
        return;
    }

    frame_count = build_action_frames(action_index, &duration);
    if (frame_count == 0) {
        return;
    }

    lv_animimg_set_src(g_ui.cat_anim, g_ui.active_frames, frame_count);
    lv_animimg_set_duration(g_ui.cat_anim, duration);
    lv_animimg_set_repeat_count(g_ui.cat_anim, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(g_ui.cat_anim);
}

static void create_cat_layer(void)
{
    int32_t cat_width = sx(200);

    g_ui.cat_layer = lv_obj_create(g_ui.viewport);
    lv_obj_remove_style_all(g_ui.cat_layer);
    lv_obj_set_pos(g_ui.cat_layer, sx(106), sx(245));
    lv_obj_set_size(g_ui.cat_layer, cat_width, sx(205));
    lv_obj_clear_flag(g_ui.cat_layer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    if (g_ui.assets_ready) {
        g_ui.cat_anim = lv_animimg_create(g_ui.cat_layer);
        lv_image_set_pivot(g_ui.cat_anim, 0, 0);
        lv_image_set_scale(g_ui.cat_anim, (uint32_t)(cat_width * 256 / 144));
        lv_obj_set_pos(g_ui.cat_anim, 0, 0);
        apply_action(g_ui.selected_action);
    } else {
        create_vector_cat(g_ui.cat_layer);
    }
}

static void show_overlay(int page);
static void update_page_dots(void);

static void show_overlay_async(void *data)
{
    show_overlay((int)(intptr_t)data);
}

static const char *background_name(int index)
{
    static const char *names_cn[UI_BACKGROUND_COUNT] = {
        "天空蓝", "薄荷绿", "暖米色", "橙粉日落", "暗黑紫"
    };
    static const char *names_en[UI_BACKGROUND_COUNT] = {
        "SKY BLUE", "MINT GREEN", "WARM BEIGE", "SUNSET", "DARK PURPLE"
    };

    return g_ui.font_body ? names_cn[index] : names_en[index];
}

static const char *action_name(int index)
{
    static const char *names_cn[UI_ACTION_COUNT] = {
        "捂鼻摆手", "害羞挥手", "躺平刷手机",
        "气球升空", "仰头大笑", "马桶摸鱼"
    };
    static const char *names_en[UI_ACTION_COUNT] = {
        "COVER DANCE", "SHY WAVE", "PHONE REST",
        "BALLOON", "LAUGH", "TOILET BREAK"
    };

    return g_ui.font_body ? names_cn[index] : names_en[index];
}

static lv_obj_t *make_radio(lv_obj_t *parent, int32_t x, int32_t y,
                            int32_t size, bool selected)
{
    lv_obj_t *radio = make_box(parent, x, y, size, size,
                               0xffffff, size / 2, LV_OPA_TRANSP);
    lv_obj_set_style_border_color(radio,
        lv_color_hex(selected ? 0x1677e8 : 0xaaa9a7), 0);
    lv_obj_set_style_border_width(radio, sx(3), 0);
    if (selected) {
        make_box(radio, (size - 16) / 2, (size - 16) / 2,
                 16, 16, 0x1677e8, 8, LV_OPA_COVER);
    }
    return radio;
}

static lv_obj_t *make_selector_row(lv_obj_t *parent, int32_t top,
                                   int32_t height)
{
    lv_obj_t *row = make_box(parent, 30, top, 372, height,
                             0xffffff, height > 60 ? 26 : 18, LV_OPA_90);
    lv_obj_set_style_border_color(row, lv_color_hex(0xb2aea8), 0);
    lv_obj_set_style_border_opa(row, LV_OPA_60, 0);
    lv_obj_set_style_border_width(row, sx(1), 0);
    lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    return row;
}

static void overlay_header(const char *title)
{
    make_label(g_ui.overlay, title, g_ui.font_body, 0x202020,
               30, 27, 245, LV_TEXT_ALIGN_LEFT);
    g_ui.overlay_time_label = make_label(g_ui.overlay, "--:--",
                                          g_ui.font_small, 0x262626,
                                          294, 34, 70,
                                          LV_TEXT_ALIGN_RIGHT);
    update_clock(NULL);
}

static void customize_row_clicked(lv_event_t *event)
{
    int page = (int)(intptr_t)lv_event_get_user_data(event);
    lv_async_call(show_overlay_async, (void *)(intptr_t)page);
}

static void background_clicked(lv_event_t *event)
{
    int index = (int)(intptr_t)lv_event_get_user_data(event);

    if (index < 0 || index >= UI_BACKGROUND_COUNT) {
        return;
    }

    g_ui.selected_background = index;
    if (g_ui.background_img) {
        lv_image_set_src(g_ui.background_img, g_background_paths[index]);
    }
    apply_theme_foreground();
    show_overlay(UI_OVERLAY_NONE);
}

static void action_clicked(lv_event_t *event)
{
    int index = (int)(intptr_t)lv_event_get_user_data(event);

    if (index < 0 || index >= UI_ACTION_COUNT) {
        return;
    }

    g_ui.selected_action = index;
    show_overlay(UI_OVERLAY_NONE);
}

static void create_customize_page(void)
{
    lv_obj_t *row;
    const ui_action_t *action = &g_actions[g_ui.selected_action];

    overlay_header(g_ui.font_body ? "外观选择" : "APPEARANCE");

    row = make_selector_row(g_ui.overlay, 130, 106);
    make_round_preview(row, g_background_paths[g_ui.selected_background],
                       24, 21, 432, 64, 0x7467a9);
    make_label(row, g_ui.font_body ? "背景选择" : "BACKGROUND",
               g_ui.font_body, 0x242424, 112, 31, 210,
               LV_TEXT_ALIGN_LEFT);
    make_label(row, ">", g_ui.font_title, 0x9c9a96,
               321, 19, 34, LV_TEXT_ALIGN_CENTER);
    lv_obj_add_event_cb(row, customize_row_clicked, LV_EVENT_CLICKED,
                        (void *)(intptr_t)UI_OVERLAY_BACKGROUNDS);

    if (g_ui.selected_action == 0) {
        snprintf(g_ui.preview_path, UI_FRAME_PATH_SIZE,
                 UI_DATA_ROOT "/cat/frame-%02d.png", action->preview_frame);
    } else {
        snprintf(g_ui.preview_path, UI_FRAME_PATH_SIZE,
                 UI_DATA_ROOT "/actions/%s/frame-%02d.png",
                 action->folder, action->preview_frame);
    }

    row = make_selector_row(g_ui.overlay, 258, 106);
    make_round_preview(row, g_ui.preview_path, 24, 21, 144, 64, 0xf1ece5);
    make_label(row, g_ui.font_body ? "动作选择" : "ACTION",
               g_ui.font_body, 0x242424, 112, 31, 210,
               LV_TEXT_ALIGN_LEFT);
    make_label(row, ">", g_ui.font_title, 0x9c9a96,
               321, 19, 34, LV_TEXT_ALIGN_CENTER);
    lv_obj_add_event_cb(row, customize_row_clicked, LV_EVENT_CLICKED,
                        (void *)(intptr_t)UI_OVERLAY_ACTIONS);
}

static void create_backgrounds_page(void)
{
    int index;

    overlay_header(g_ui.font_body ? "风格选择" : "STYLE");
    for (index = 0; index < UI_BACKGROUND_COUNT; index++) {
        int32_t top = 88 + index * 76;
        lv_obj_t *row = make_selector_row(g_ui.overlay, top, 68);

        make_round_preview(row, g_background_paths[index],
                           20, 11, 432, 46, 0x7467a9);
        make_label(row, background_name(index), g_ui.font_body,
                   0x242424, 84, 15, 218, LV_TEXT_ALIGN_LEFT);
        g_ui.background_radios[index] = make_radio(row, 318, 17, 34,
            index == g_ui.selected_background);
        lv_obj_add_event_cb(row, background_clicked, LV_EVENT_CLICKED,
                            (void *)(intptr_t)index);
    }
}

static void create_actions_page(void)
{
    int index;

    overlay_header(g_ui.font_body ? "动作选择" : "ACTION");
    for (index = 0; index < UI_ACTION_COUNT; index++) {
        int32_t top = 76 + index * 56;
        lv_obj_t *row = make_selector_row(g_ui.overlay, top, 49);

        make_label(row, action_name(index), g_ui.font_body,
                   0x242424, 28, 9, 260, LV_TEXT_ALIGN_LEFT);
        g_ui.action_radios[index] = make_radio(row, 322, 9, 30,
            index == g_ui.selected_action);
        lv_obj_add_event_cb(row, action_clicked, LV_EVENT_CLICKED,
                            (void *)(intptr_t)index);
    }
}

static void render_weather_page(void);

static void weather_icon_path(char *path, size_t size, const char *icon)
{
    const char *code = icon && *icon ? icon : "999";

    snprintf(path, size, UI_WEATHER_ROOT "/%s.png", code);
    if (access(path, R_OK) != 0) {
        snprintf(path, size, UI_WEATHER_ROOT "/999.png");
    }
}

static void weather_add_suffix(char *buffer, size_t size,
                               const char *value, const char *suffix)
{
    if (value == NULL || *value == '\0' || strcmp(value, "--") == 0) {
        snprintf(buffer, size, "--%s", suffix);
    } else {
        snprintf(buffer, size, "%s%s", value, suffix);
    }
}

static void create_weather_background(lv_obj_t *parent)
{
    /* Keep weather independent from the full-screen animated background.
     * Re-decoding and scaling a second 432x514 ARGB PNG while the R528
     * framebuffer is being hardware-rotated can overload the shared G2D
     * path.  A native LVGL gradient preserves the dark weather treatment
     * without another full-screen image buffer or transform. */
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x102a56), 0);
    lv_obj_set_style_bg_grad_color(parent, lv_color_hex(0x050d26), 0);
    lv_obj_set_style_bg_grad_dir(parent, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
}

static void weather_reset_detail_objects(void)
{
    int index;

    g_ui.weather_header_name_label = NULL;
    g_ui.weather_header_metadata_label = NULL;
    g_ui.weather_icon_image = NULL;
    g_ui.weather_temperature_label = NULL;
    g_ui.weather_condition_label = NULL;
    g_ui.weather_feels_label = NULL;
    g_ui.weather_humidity_label = NULL;
    g_ui.weather_visibility_label = NULL;
    g_ui.weather_update_label = NULL;
    g_ui.weather_forecast_range_label = NULL;
    for (index = 0; index < OPENVELA_UI_WEATHER_FORECAST_DAYS; index++) {
        g_ui.weather_forecast_weekday_label[index] = NULL;
        g_ui.weather_forecast_icon_image[index] = NULL;
        g_ui.weather_forecast_text_label[index] = NULL;
        g_ui.weather_forecast_temperature_label[index] = NULL;
    }
}

static void weather_reset_page_objects(void)
{
    g_ui.overlay_time_label = NULL;
    g_ui.weather_roller_province = NULL;
    g_ui.weather_roller_city = NULL;
    g_ui.weather_roller_district = NULL;
    g_ui.weather_picker_summary_label = NULL;
    g_ui.weather_picker_status_label = NULL;
    g_ui.weather_delete_modal = NULL;
    g_ui.weather_delete_city = -1;
    g_ui.weather_modal_active = false;
    g_ui.weather_picker_updating = false;
    weather_reset_detail_objects();
}

static void weather_set_label(lv_obj_t *label, const char *text)
{
    if (label && lv_obj_is_valid(label)) {
        lv_label_set_text(label, text ? text : "");
    }
}

static void weather_set_image(lv_obj_t *image, const char *path)
{
    if (image && lv_obj_is_valid(image) && path &&
        access(path, R_OK) == 0) {
        lv_image_set_src(image, path);
    }
}

static lv_obj_t *weather_card(lv_obj_t *parent, int32_t x, int32_t y,
                              int32_t width, int32_t height,
                              bool selected)
{
    lv_obj_t *card = make_box(parent, x, y, width, height,
                              0xf8fbff, 25, LV_OPA_90);

    lv_obj_set_style_border_color(card,
        lv_color_hex(selected ? 0x3995ff : 0x75aeea), 0);
    lv_obj_set_style_border_width(card, sx(selected ? 4 : 2), 0);
    return card;
}

static void weather_add_page_dots(lv_obj_t *parent, int active, int count,
                                  int32_t y)
{
    int index;
    int32_t start = 216 - ((count * 20 - 8) / 2);

    for (index = 0; index < count; index++) {
        make_box(parent, start + index * 20, y,
                 index == active ? 16 : 8, 7,
                 0xffffff, 4,
                 index == active ? LV_OPA_COVER : LV_OPA_40);
    }
}

static void weather_selected_preview_path(char *path, size_t size)
{
    const ui_action_t *action = &g_actions[g_ui.selected_action];

    if (g_ui.selected_action == 0) {
        snprintf(path, size, UI_DATA_ROOT "/cat/frame-%02d.png",
                 action->preview_frame);
    } else {
        snprintf(path, size, UI_DATA_ROOT "/actions/%s/frame-%02d.png",
                 action->folder, action->preview_frame);
    }
}

static bool weather_snapshot_matches_city(
    const struct openvela_ui_weather_snapshot_s *snapshot, int city_index)
{
    return snapshot && snapshot->has_data && city_index >= 0 &&
           city_index == g_ui.weather_request_city;
}

static int weather_city_count(void)
{
    return UI_WEATHER_BASE_CITY_COUNT + g_ui.weather_custom_city_count;
}

static const ui_weather_city_t *weather_city_at(int city_index)
{
    int custom_index;

    if (city_index >= 0 && city_index < UI_WEATHER_BASE_CITY_COUNT) {
        return &g_weather_base_cities[city_index];
    }

    custom_index = city_index - UI_WEATHER_BASE_CITY_COUNT;
    if (custom_index < 0 ||
        custom_index >= g_ui.weather_custom_city_count) {
        return NULL;
    }
    return &g_ui.weather_custom_cities[custom_index];
}

static bool weather_city_is_custom(int city_index)
{
    return city_index >= UI_WEATHER_BASE_CITY_COUNT &&
           city_index < weather_city_count();
}

static int weather_city_page_count(void)
{
    int custom_pages =
        (g_ui.weather_custom_city_count + UI_WEATHER_CITIES_PER_PAGE - 1) /
        UI_WEATHER_CITIES_PER_PAGE;

    return UI_WEATHER_CUSTOM_FIRST_PAGE + custom_pages;
}

static void weather_copy_text(char *destination, size_t size,
                              const char *source)
{
    if (size > 0) {
        snprintf(destination, size, "%s", source ? source : "");
    }
}

static bool weather_city_duplicate(const char *location_id,
                                   const char *name,
                                   const char *administrative_area)
{
    int index;

    for (index = 0; index < weather_city_count(); index++) {
        const ui_weather_city_t *city = weather_city_at(index);

        if (city) {
            if (location_id && *location_id && city->location_id[0] &&
                strcmp(city->location_id, location_id) == 0) {
                return true;
            }
            if (strcmp(city->name, name) == 0 &&
                strcmp(city->administrative_area,
                       administrative_area) == 0) {
                return true;
            }
        }
    }
    return false;
}

static void weather_load_custom_cities(void)
{
    FILE *file = fopen(UI_WEATHER_CITY_STORE, "r");
    char line[256];

    g_ui.weather_custom_city_count = 0;
    if (!file) {
        return;
    }

    while (g_ui.weather_custom_city_count < UI_WEATHER_MAX_CUSTOM_CITIES &&
           fgets(line, sizeof(line), file)) {
        ui_weather_city_t *city;
        char *save = NULL;
        char *location_id = strtok_r(line, "\t\r\n", &save);
        char *name = strtok_r(NULL, "\t\r\n", &save);
        char *area = strtok_r(NULL, "\t\r\n", &save);

        /* An empty location id is persisted as '-'.  This also makes each
         * line robust against strtok_r collapsing leading delimiters. */
        if (!location_id || !name || !area || !*name || !*area) {
            continue;
        }
        if (strcmp(location_id, "-") == 0) {
            location_id = "";
        }
        if (weather_city_duplicate(location_id, name, area)) {
            continue;
        }

        city = &g_ui.weather_custom_cities[
            g_ui.weather_custom_city_count++];
        weather_copy_text(city->location_id, sizeof(city->location_id),
                          location_id);
        weather_copy_text(city->name, sizeof(city->name), name);
        weather_copy_text(city->administrative_area,
                          sizeof(city->administrative_area), area);
    }
    fclose(file);
}

static int weather_save_custom_cities(void)
{
    FILE *file;
    int index;

    mkdir("/data/etc/openvela_ui", 0777);
    file = fopen(UI_WEATHER_CITY_STORE_TMP, "w");
    if (!file) {
        return -errno;
    }

    for (index = 0; index < g_ui.weather_custom_city_count; index++) {
        const ui_weather_city_t *city = &g_ui.weather_custom_cities[index];

        if (fprintf(file, "%s\t%s\t%s\n",
                    city->location_id[0] ? city->location_id : "-",
                    city->name, city->administrative_area) < 0) {
            int error = errno ? -errno : -EIO;

            fclose(file);
            unlink(UI_WEATHER_CITY_STORE_TMP);
            return error;
        }
    }

    if (fclose(file) != 0) {
        int error = errno ? -errno : -EIO;

        unlink(UI_WEATHER_CITY_STORE_TMP);
        return error;
    }
    if (rename(UI_WEATHER_CITY_STORE_TMP, UI_WEATHER_CITY_STORE) != 0) {
        int error = -errno;

        unlink(UI_WEATHER_CITY_STORE_TMP);
        return error;
    }
    return 0;
}

static void weather_request_city(int city_index)
{
    const ui_weather_city_t *city = weather_city_at(city_index);
    const char *location_id;
    int result;

    if (!city) {
        return;
    }

    g_ui.weather_request_city = city_index;
    g_ui.weather_poll_ticks = 0;
    /* The local proxy deliberately trusts only the five built-in IDs.
     * User-picked cities always use the authenticated name + adm lookup;
     * retain their data-set ID only for duplicate detection/persistence. */
    location_id = city_index < UI_WEATHER_BASE_CITY_COUNT ?
                  city->location_id : "";
    result = openvela_ui_weather_request(location_id, city->name,
                                         city->administrative_area);
    if (result < 0) {
        memset(&g_ui.weather_snapshot, 0,
               sizeof(g_ui.weather_snapshot));
        g_ui.weather_snapshot.state = OPENVELA_UI_WEATHER_ERROR;
        g_ui.weather_snapshot.error = result;
        return;
    }
    openvela_ui_weather_snapshot(&g_ui.weather_snapshot);
}

static void weather_open_city_async(void *argument)
{
    int city_index = (int)(intptr_t)argument;

    g_ui.weather_switch_pending = false;
    if (g_ui.overlay_page != UI_OVERLAY_WEATHER ||
        g_ui.weather_transitioning ||
        !g_ui.weather_view || !lv_obj_is_valid(g_ui.weather_view) ||
        city_index < 0 || city_index >= weather_city_count()) {
        return;
    }

    g_ui.weather_selected_city = city_index;
    g_ui.weather_stage = UI_WEATHER_DETAIL;
    g_ui.weather_detail_page = UI_WEATHER_NOW;
    weather_request_city(city_index);
    render_weather_page();
}

static void weather_city_clicked(lv_event_t *event)
{
    lv_indev_t *indev = lv_indev_active();
    int city_index = (int)(intptr_t)lv_event_get_user_data(event);

    if ((indev && lv_indev_get_press_moved(indev)) ||
        g_ui.weather_transitioning ||
        g_ui.weather_switch_pending ||
        g_ui.weather_stage != UI_WEATHER_CITIES) {
        return;
    }

    g_ui.weather_switch_pending = true;
    if (lv_async_call(weather_open_city_async,
                      (void *)(intptr_t)city_index) != LV_RESULT_OK) {
        g_ui.weather_switch_pending = false;
    }
}

static bool weather_options_append(char *buffer, size_t size,
                                   const char *option)
{
    size_t used = strlen(buffer);
    size_t length = strlen(option ? option : "");

    if (used + (used ? 1 : 0) + length + 1 > size) {
        return false;
    }
    if (used) {
        buffer[used++] = '\n';
    }
    memcpy(buffer + used, option ? option : "", length);
    buffer[used + length] = '\0';
    return true;
}

static void weather_picker_update_summary(void)
{
    const char *province = openvela_ui_city_province_name(
        g_ui.weather_picker_province);
    const char *city = openvela_ui_city_city_short_name(
        g_ui.weather_picker_province, g_ui.weather_picker_city);
    const char *district = "";
    char summary[160];

    if (g_ui.weather_picker_district > 0) {
        district = openvela_ui_city_district_name(
            g_ui.weather_picker_province, g_ui.weather_picker_city,
            g_ui.weather_picker_district - 1);
    }
    if (*district) {
        snprintf(summary, sizeof(summary), "已选择：%s · %s · %s",
                 province, city, district);
    } else {
        snprintf(summary, sizeof(summary), "已选择：%s · %s",
                 province, city);
    }
    weather_set_label(g_ui.weather_picker_summary_label, summary);
}

static void weather_picker_set_district_options(void)
{
    size_t count = openvela_ui_city_district_count(
        g_ui.weather_picker_province, g_ui.weather_picker_city);
    size_t index;

    weather_copy_text(g_ui.weather_district_options,
                      sizeof(g_ui.weather_district_options), "不选择");
    for (index = 0; index < count; index++) {
        if (!weather_options_append(g_ui.weather_district_options,
                                    sizeof(g_ui.weather_district_options),
                                    openvela_ui_city_district_name(
                                        g_ui.weather_picker_province,
                                        g_ui.weather_picker_city, index))) {
            break;
        }
    }
    g_ui.weather_picker_district = 0;
    if (g_ui.weather_roller_district &&
        lv_obj_is_valid(g_ui.weather_roller_district)) {
        lv_roller_set_options(g_ui.weather_roller_district,
                              g_ui.weather_district_options,
                              LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(g_ui.weather_roller_district, 0,
                               LV_ANIM_OFF);
    }
}

static void weather_picker_set_city_options(void)
{
    size_t count = openvela_ui_city_city_count(
        g_ui.weather_picker_province);
    size_t index;

    g_ui.weather_city_options[0] = '\0';
    for (index = 0; index < count; index++) {
        if (!weather_options_append(g_ui.weather_city_options,
                                    sizeof(g_ui.weather_city_options),
                                    openvela_ui_city_city_short_name(
                                        g_ui.weather_picker_province,
                                        index))) {
            break;
        }
    }
    g_ui.weather_picker_city = 0;
    if (g_ui.weather_roller_city &&
        lv_obj_is_valid(g_ui.weather_roller_city)) {
        lv_roller_set_options(g_ui.weather_roller_city,
                              g_ui.weather_city_options,
                              LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(g_ui.weather_roller_city, 0, LV_ANIM_OFF);
    }
    weather_picker_set_district_options();
}

static void weather_picker_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    int picker = (int)(intptr_t)lv_event_get_user_data(event);

    lv_event_stop_bubbling(event);
    if (code != LV_EVENT_VALUE_CHANGED || g_ui.weather_picker_updating) {
        return;
    }

    g_ui.weather_picker_updating = true;
    if (picker == 0) {
        g_ui.weather_picker_province = (uint16_t)lv_roller_get_selected(
            g_ui.weather_roller_province);
        weather_picker_set_city_options();
    } else if (picker == 1) {
        g_ui.weather_picker_city = (uint16_t)lv_roller_get_selected(
            g_ui.weather_roller_city);
        weather_picker_set_district_options();
    } else {
        g_ui.weather_picker_district = (uint16_t)lv_roller_get_selected(
            g_ui.weather_roller_district);
    }
    weather_picker_update_summary();
    g_ui.weather_picker_updating = false;
}

static lv_obj_t *weather_picker_roller(lv_obj_t *parent, int32_t x,
                                       const char *options, int picker)
{
    lv_obj_t *roller = lv_roller_create(parent);

    lv_obj_set_pos(roller, sx(x), sx(101));
    lv_obj_set_size(roller, sx(122), sx(225));
    lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_style_bg_color(roller, lv_color_hex(0xf8fbff), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(roller, LV_OPA_90, LV_PART_MAIN);
    lv_obj_set_style_border_color(roller, lv_color_hex(0x75aeea),
                                  LV_PART_MAIN);
    lv_obj_set_style_border_width(roller, sx(2), LV_PART_MAIN);
    lv_obj_set_style_radius(roller, sx(20), LV_PART_MAIN);
    lv_obj_set_style_text_color(roller, lv_color_hex(0x5f7191),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(roller,
        g_ui.font_small ? g_ui.font_small : LV_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roller, lv_color_hex(0xdcecff),
                              LV_PART_SELECTED);
    lv_obj_set_style_bg_opa(roller, LV_OPA_COVER, LV_PART_SELECTED);
    lv_obj_set_style_text_color(roller, lv_color_hex(0x176fd1),
                                LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller,
        g_ui.font_small ? g_ui.font_small : LV_FONT_DEFAULT,
        LV_PART_SELECTED);
    lv_obj_set_scrollbar_mode(roller, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(roller, weather_picker_event, LV_EVENT_ALL,
                        (void *)(intptr_t)picker);
    return roller;
}

static void weather_add_city_async(void *argument)
{
    const char *province;
    const char *city_name;
    const char *location_id;
    const char *district = "";
    const char *name;
    const char *area;
    ui_weather_city_t *city;
    int custom_index;

    LV_UNUSED(argument);
    g_ui.weather_switch_pending = false;
    if (g_ui.overlay_page != UI_OVERLAY_WEATHER ||
        g_ui.weather_transitioning ||
        g_ui.weather_stage != UI_WEATHER_CITIES ||
        g_ui.weather_city_page != UI_WEATHER_ADD_PAGE_INDEX) {
        return;
    }

    province = openvela_ui_city_province_name(
        g_ui.weather_picker_province);
    city_name = openvela_ui_city_city_short_name(
        g_ui.weather_picker_province, g_ui.weather_picker_city);
    location_id = openvela_ui_city_city_location_id(
        g_ui.weather_picker_province, g_ui.weather_picker_city);
    if (g_ui.weather_picker_district > 0) {
        district = openvela_ui_city_district_name(
            g_ui.weather_picker_province, g_ui.weather_picker_city,
            g_ui.weather_picker_district - 1);
    }
    name = *district ? district : city_name;
    area = *district ? city_name : province;
    if (!*name || !*area) {
        weather_set_label(g_ui.weather_picker_status_label,
                          "城市数据无效，请重新选择");
        return;
    }
    if (weather_city_duplicate(*district ? "" : location_id, name, area)) {
        weather_set_label(g_ui.weather_picker_status_label,
                          "该城市已经在列表中");
        return;
    }
    if (g_ui.weather_custom_city_count >= UI_WEATHER_MAX_CUSTOM_CITIES) {
        weather_set_label(g_ui.weather_picker_status_label,
                          "最多添加 12 个城市");
        return;
    }

    custom_index = g_ui.weather_custom_city_count;
    city = &g_ui.weather_custom_cities[g_ui.weather_custom_city_count++];
    weather_copy_text(city->location_id, sizeof(city->location_id),
                      *district ? "" : location_id);
    weather_copy_text(city->name, sizeof(city->name), name);
    weather_copy_text(city->administrative_area,
                      sizeof(city->administrative_area), area);
    if (weather_save_custom_cities() < 0) {
        g_ui.weather_custom_city_count--;
        memset(city, 0, sizeof(*city));
        weather_set_label(g_ui.weather_picker_status_label,
                          "保存失败，请稍后重试");
        return;
    }

    g_ui.weather_selected_city = UI_WEATHER_BASE_CITY_COUNT + custom_index;
    g_ui.weather_city_page = UI_WEATHER_CUSTOM_FIRST_PAGE +
        custom_index / UI_WEATHER_CITIES_PER_PAGE;
    render_weather_page();
}

static void weather_add_city_clicked(lv_event_t *event)
{
    lv_indev_t *indev = lv_indev_active();

    lv_event_stop_bubbling(event);
    if ((indev && lv_indev_get_press_moved(indev)) ||
        g_ui.weather_switch_pending || g_ui.weather_modal_active) {
        return;
    }
    g_ui.weather_switch_pending = true;
    if (lv_async_call(weather_add_city_async, NULL) != LV_RESULT_OK) {
        g_ui.weather_switch_pending = false;
    }
}

static void create_weather_city_picker(void)
{
    size_t index;
    lv_obj_t *button;

    make_label(g_ui.weather_view, "添加城市", g_ui.font_title, 0xffffff,
               45, 19, 342, LV_TEXT_ALIGN_CENTER);
    make_label(g_ui.weather_view, "省份", g_ui.font_small, 0xdbe8ff,
               22, 72, 122, LV_TEXT_ALIGN_CENTER);
    make_label(g_ui.weather_view, "城市", g_ui.font_small, 0xdbe8ff,
               155, 72, 122, LV_TEXT_ALIGN_CENTER);
    make_label(g_ui.weather_view, "区县（可选）", g_ui.font_small, 0xdbe8ff,
               288, 72, 122, LV_TEXT_ALIGN_CENTER);

    g_ui.weather_province_options[0] = '\0';
    for (index = 0; index < openvela_ui_city_province_count(); index++) {
        if (!weather_options_append(g_ui.weather_province_options,
                                    sizeof(g_ui.weather_province_options),
                                    openvela_ui_city_province_name(index))) {
            break;
        }
    }
    g_ui.weather_picker_province = 0;
    g_ui.weather_picker_city = 0;
    g_ui.weather_picker_district = 0;
    g_ui.weather_picker_updating = true;
    g_ui.weather_roller_province = weather_picker_roller(
        g_ui.weather_view, 22, g_ui.weather_province_options, 0);

    g_ui.weather_city_options[0] = '\0';
    for (index = 0; index < openvela_ui_city_city_count(0); index++) {
        if (!weather_options_append(g_ui.weather_city_options,
                                    sizeof(g_ui.weather_city_options),
                                    openvela_ui_city_city_short_name(
                                        0, index))) {
            break;
        }
    }
    g_ui.weather_roller_city = weather_picker_roller(
        g_ui.weather_view, 155, g_ui.weather_city_options, 1);

    weather_copy_text(g_ui.weather_district_options,
                      sizeof(g_ui.weather_district_options), "不选择");
    for (index = 0; index < openvela_ui_city_district_count(0, 0); index++) {
        if (!weather_options_append(g_ui.weather_district_options,
                                    sizeof(g_ui.weather_district_options),
                                    openvela_ui_city_district_name(
                                        0, 0, index))) {
            break;
        }
    }
    g_ui.weather_roller_district = weather_picker_roller(
        g_ui.weather_view, 288, g_ui.weather_district_options, 2);
    g_ui.weather_picker_updating = false;

    g_ui.weather_picker_summary_label = make_label(
        g_ui.weather_view, "", g_ui.font_small, 0xdbe8ff,
        31, 337, 370, LV_TEXT_ALIGN_CENTER);
    weather_picker_update_summary();
    button = make_box(g_ui.weather_view, 94, 374, 244, 52,
                      0x2f8cf4, 26, LV_OPA_COVER);
    lv_obj_set_style_border_color(button, lv_color_hex(0xa9d4ff), 0);
    lv_obj_set_style_border_width(button, sx(2), 0);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    make_label(button, "添加到天气", g_ui.font_body, 0xffffff,
               0, 7, 244, LV_TEXT_ALIGN_CENTER);
    lv_obj_add_event_cb(button, weather_add_city_clicked,
                        LV_EVENT_CLICKED, NULL);
    g_ui.weather_picker_status_label = make_label(
        g_ui.weather_view, "", g_ui.font_small, 0xffd1d1,
        41, 434, 350, LV_TEXT_ALIGN_CENTER);
    weather_add_page_dots(g_ui.weather_view, g_ui.weather_city_page,
                          weather_city_page_count(), 462);
    make_label(g_ui.weather_view, "左右翻页 · 上滑返回", g_ui.font_small,
               0xdbe8ff, 51, 480, 330, LV_TEXT_ALIGN_CENTER);
}

static void weather_delete_modal_close_async(void *argument)
{
    lv_obj_t *modal = g_ui.weather_delete_modal;

    LV_UNUSED(argument);
    g_ui.weather_delete_modal = NULL;
    g_ui.weather_delete_city = -1;
    g_ui.weather_modal_active = false;
    if (modal && lv_obj_is_valid(modal)) {
        lv_obj_delete(modal);
    }
}

static void weather_delete_modal_forget(void)
{
    /* render_weather_page() will delete the complete old tree.  Forget the
     * modal first so the confirmation path causes only one invalidation and
     * one framebuffer update instead of deleting the modal and immediately
     * cleaning its parent as a second operation. */
    g_ui.weather_delete_modal = NULL;
    g_ui.weather_delete_city = -1;
    g_ui.weather_modal_active = false;
}

static void weather_request_base_async(void *argument)
{
    LV_UNUSED(argument);
    if (g_ui.weather_request_city < 0) {
        weather_request_city(0);
    }
}

static void weather_delete_cancel_clicked(lv_event_t *event)
{
    lv_event_stop_bubbling(event);
    lv_async_call(weather_delete_modal_close_async, NULL);
}

static void weather_delete_confirm_async(void *argument)
{
    int city_index = g_ui.weather_delete_city;
    int custom_index = city_index - UI_WEATHER_BASE_CITY_COUNT;
    ui_weather_city_t removed;
    int remaining;
    int save_result;
    bool request_base = false;

    LV_UNUSED(argument);
    if (g_ui.overlay_page != UI_OVERLAY_WEATHER ||
        g_ui.weather_stage != UI_WEATHER_CITIES ||
        !g_ui.weather_view || !lv_obj_is_valid(g_ui.weather_view) ||
        !g_ui.weather_modal_active || custom_index < 0 ||
        custom_index >= g_ui.weather_custom_city_count) {
        g_ui.weather_switch_pending = false;
        return;
    }

    removed = g_ui.weather_custom_cities[custom_index];
    remaining = g_ui.weather_custom_city_count - custom_index - 1;
    if (remaining > 0) {
        memmove(&g_ui.weather_custom_cities[custom_index],
                &g_ui.weather_custom_cities[custom_index + 1],
                (size_t)remaining * sizeof(ui_weather_city_t));
    }
    g_ui.weather_custom_city_count--;
    memset(&g_ui.weather_custom_cities[g_ui.weather_custom_city_count],
           0, sizeof(ui_weather_city_t));
    save_result = weather_save_custom_cities();
    if (save_result < 0) {
        int shifted = g_ui.weather_custom_city_count - custom_index;

        if (shifted > 0) {
            memmove(&g_ui.weather_custom_cities[custom_index + 1],
                    &g_ui.weather_custom_cities[custom_index],
                    (size_t)shifted * sizeof(ui_weather_city_t));
        }
        g_ui.weather_custom_cities[custom_index] = removed;
        g_ui.weather_custom_city_count++;
        syslog(LOG_WARNING,
               "openvela_ui: failed to persist city deletion: %d\n",
               save_result);
        weather_delete_modal_forget();
        render_weather_page();
        g_ui.weather_switch_pending = false;
        make_label(g_ui.weather_view, "删除失败，请稍后重试",
                   g_ui.font_small, 0xffc5c5,
                   76, 431, 280, LV_TEXT_ALIGN_CENTER);
        return;
    }

    if (g_ui.weather_selected_city == city_index) {
        g_ui.weather_selected_city = 0;
    } else if (g_ui.weather_selected_city > city_index) {
        g_ui.weather_selected_city--;
    }
    if (g_ui.weather_request_city == city_index) {
        g_ui.weather_request_city = -1;
        memset(&g_ui.weather_snapshot, 0, sizeof(g_ui.weather_snapshot));
        g_ui.weather_snapshot.state = OPENVELA_UI_WEATHER_LOADING;
        request_base = true;
    } else if (g_ui.weather_request_city > city_index) {
        g_ui.weather_request_city--;
    }
    weather_delete_modal_forget();
    g_ui.weather_city_page = LV_MIN(g_ui.weather_city_page,
                                    weather_city_page_count() - 1);
    render_weather_page();
    g_ui.weather_switch_pending = false;
    if (request_base &&
        lv_async_call(weather_request_base_async, NULL) != LV_RESULT_OK) {
        weather_request_city(0);
    }
}

static void weather_delete_confirm_clicked(lv_event_t *event)
{
    lv_event_stop_bubbling(event);
    if (g_ui.weather_switch_pending) {
        return;
    }
    g_ui.weather_switch_pending = true;
    if (lv_async_call(weather_delete_confirm_async, NULL) != LV_RESULT_OK) {
        g_ui.weather_switch_pending = false;
    }
}

static void weather_show_delete_modal(int city_index)
{
    const ui_weather_city_t *city = weather_city_at(city_index);
    lv_obj_t *dialog;
    lv_obj_t *button;
    char question[128];

    if (!city || !weather_city_is_custom(city_index) ||
        g_ui.weather_modal_active) {
        return;
    }
    g_ui.weather_modal_active = true;
    g_ui.weather_delete_city = city_index;
    g_ui.weather_delete_modal = make_box(g_ui.weather_view, 0, 0,
                                         UI_DESIGN_WIDTH, UI_DESIGN_HEIGHT,
                                         0x000000, 0, LV_OPA_50);
    lv_obj_add_flag(g_ui.weather_delete_modal, LV_OBJ_FLAG_CLICKABLE);
    dialog = make_box(g_ui.weather_delete_modal, 49, 142, 334, 184,
                      0xf8fbff, 22, LV_OPA_COVER);
    make_label(dialog, "删除城市", g_ui.font_body, 0x172f5c,
               36, 18, 262, LV_TEXT_ALIGN_CENTER);
    snprintf(question, sizeof(question), "确定删除“%s”吗？", city->name);
    make_label(dialog, question, g_ui.font_small, 0x61728f,
               24, 68, 286, LV_TEXT_ALIGN_CENTER);

    button = make_box(dialog, 20, 121, 137, 46,
                      0xe5e9f0, 23, LV_OPA_COVER);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    make_label(button, "取消", g_ui.font_small, 0x35445f,
               0, 10, 137, LV_TEXT_ALIGN_CENTER);
    lv_obj_add_event_cb(button, weather_delete_cancel_clicked,
                        LV_EVENT_CLICKED, NULL);
    button = make_box(dialog, 177, 121, 137, 46,
                      0xe34d59, 23, LV_OPA_COVER);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    make_label(button, "删除", g_ui.font_small, 0xffffff,
               0, 10, 137, LV_TEXT_ALIGN_CENTER);
    lv_obj_add_event_cb(button, weather_delete_confirm_clicked,
                        LV_EVENT_CLICKED, NULL);
}

static void weather_delete_clicked(lv_event_t *event)
{
    lv_indev_t *indev = lv_indev_active();
    int city_index = (int)(intptr_t)lv_event_get_user_data(event);

    lv_event_stop_bubbling(event);
    if (indev && lv_indev_get_press_moved(indev)) {
        return;
    }
    weather_show_delete_modal(city_index);
}

static void create_weather_city_list(void)
{
    int first;
    int last;
    int index;

    if (g_ui.weather_city_page == UI_WEATHER_ADD_PAGE_INDEX) {
        create_weather_city_picker();
        return;
    }
    if (g_ui.weather_city_page < UI_WEATHER_BASE_PAGE_COUNT) {
        first = g_ui.weather_city_page * UI_WEATHER_CITIES_PER_PAGE;
        last = LV_MIN(first + UI_WEATHER_CITIES_PER_PAGE,
                      UI_WEATHER_BASE_CITY_COUNT);
    } else {
        first = UI_WEATHER_BASE_CITY_COUNT +
            (g_ui.weather_city_page - UI_WEATHER_CUSTOM_FIRST_PAGE) *
            UI_WEATHER_CITIES_PER_PAGE;
        last = LV_MIN(first + UI_WEATHER_CITIES_PER_PAGE,
                      weather_city_count());
    }

    make_label(g_ui.weather_view,
               g_ui.font_body ? "城市选择" : "SELECT CITY",
               g_ui.font_title, 0xffffff,
               45, 27, 342, LV_TEXT_ALIGN_CENTER);
    g_ui.overlay_time_label = make_label(g_ui.weather_view, "--:--",
                                         g_ui.font_small, 0xffffff,
                                         322, 37, 76,
                                         LV_TEXT_ALIGN_RIGHT);
    update_clock(NULL);

    for (index = first; index < last; index++) {
        const ui_weather_city_t *city = weather_city_at(index);
        int slot = index - first;
        int32_t top = 84 + slot * 84;
        lv_obj_t *card;

        if (!city) {
            continue;
        }
        card = weather_card(g_ui.weather_view, 30, top,
                                      372, 70,
                                      index == g_ui.weather_selected_city);

        make_label(card, city->name, g_ui.font_body, 0x173366,
                   28, 10, weather_city_is_custom(index) ? 190 : 230,
                   LV_TEXT_ALIGN_LEFT);
        make_label(card, city->administrative_area, g_ui.font_small,
                   0x6780a8, 218, 17,
                   weather_city_is_custom(index) ? 76 : 96,
                   LV_TEXT_ALIGN_RIGHT);
        if (weather_city_is_custom(index)) {
            lv_obj_t *remove = make_box(card, 300, 17, 58, 36,
                                        0xe34d59, 18, LV_OPA_COVER);

            lv_obj_add_flag(remove, LV_OBJ_FLAG_CLICKABLE);
            make_label(remove, "删除", g_ui.font_small, 0xffffff,
                       0, 6, 58, LV_TEXT_ALIGN_CENTER);
            lv_obj_add_event_cb(remove, weather_delete_clicked,
                                LV_EVENT_CLICKED,
                                (void *)(intptr_t)index);
        } else {
            make_label(card, ">", g_ui.font_body, 0x3995ff,
                       329, 9, 30, LV_TEXT_ALIGN_CENTER);
        }
        lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE |
                              LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_event_cb(card, weather_city_clicked,
                            LV_EVENT_CLICKED,
                            (void *)(intptr_t)index);
    }

    if (first >= last) {
        make_label(g_ui.weather_view,
                   g_ui.font_body ? "暂无城市" : "NO CITY",
                   g_ui.font_body, 0xffffff,
                   76, 220, 280, LV_TEXT_ALIGN_CENTER);
    }

    weather_add_page_dots(g_ui.weather_view, g_ui.weather_city_page,
                          weather_city_page_count(), 455);
    make_label(g_ui.weather_view,
               g_ui.font_small ? "左右翻页 · 上滑返回" :
                                 "SWIPE PAGES · UP TO RETURN",
               g_ui.font_small, 0xdbe8ff,
               51, 475, 330, LV_TEXT_ALIGN_CENTER);
}

static const struct openvela_ui_weather_snapshot_s *
weather_detail_snapshot(void)
{
    if (g_ui.weather_snapshot.has_data &&
        weather_snapshot_matches_city(&g_ui.weather_snapshot,
                                      g_ui.weather_selected_city)) {
        return &g_ui.weather_snapshot;
    }
    return NULL;
}

static void weather_detail_header(
    const struct openvela_ui_weather_snapshot_s *snapshot)
{
    const ui_weather_city_t *city =
        weather_city_at(g_ui.weather_selected_city);
    const char *name = snapshot && *snapshot->location_name ?
                       snapshot->location_name :
                       (city ? city->name : "--");
    const char *area = snapshot && *snapshot->administrative_area ?
                       snapshot->administrative_area :
                       (city ? city->administrative_area : "--");
    char metadata[160];

    snprintf(metadata, sizeof(metadata), "%s · %s", area,
             snapshot && *snapshot->country ? snapshot->country : "中国");
    g_ui.weather_header_name_label =
        make_label(g_ui.weather_view, name, g_ui.font_title, 0xffffff,
                   36, 24, 360, LV_TEXT_ALIGN_CENTER);
    g_ui.weather_header_metadata_label =
        make_label(g_ui.weather_view, metadata, g_ui.font_small, 0xd7e5ff,
                   46, 72, 340, LV_TEXT_ALIGN_CENTER);
}

static void create_weather_now_page(void)
{
    const struct openvela_ui_weather_snapshot_s *snapshot =
        weather_detail_snapshot();
    char icon_path[UI_FRAME_PATH_SIZE];
    char temperature[24];
    char feels_like[24];
    char humidity[24];
    char visibility[24];
    char update[64];
    char preview[UI_FRAME_PATH_SIZE];
    lv_obj_t *metrics;

    weather_detail_header(snapshot);
    weather_icon_path(icon_path, sizeof(icon_path),
                      snapshot ? snapshot->icon : "999");
    g_ui.weather_icon_image =
        make_asset_image(g_ui.weather_view, icon_path,
                         51, 102, 128, 116);

    weather_add_suffix(temperature, sizeof(temperature),
                       snapshot ? snapshot->temperature : "--", "°");
    g_ui.weather_temperature_label =
        make_label(g_ui.weather_view, temperature, g_ui.font_clock,
                   0xffffff, 176, 98, 210, LV_TEXT_ALIGN_CENTER);
    g_ui.weather_condition_label =
        make_label(g_ui.weather_view,
                   snapshot ? snapshot->text :
                   (g_ui.font_body ? "天气获取中" : "LOADING"),
                   g_ui.font_body, 0xe9f2ff,
                   181, 171, 198, LV_TEXT_ALIGN_CENTER);

    metrics = weather_card(g_ui.weather_view, 32, 230, 368, 106, false);
    weather_add_suffix(feels_like, sizeof(feels_like),
                       snapshot ? snapshot->feels_like : "--", "°");
    weather_add_suffix(humidity, sizeof(humidity),
                       snapshot ? snapshot->humidity : "--", "%");
    weather_add_suffix(visibility, sizeof(visibility),
                       snapshot ? snapshot->visibility : "--", "km");
    g_ui.weather_feels_label =
        make_label(metrics, feels_like, g_ui.font_body, 0x18396f,
                   7, 18, 112, LV_TEXT_ALIGN_CENTER);
    g_ui.weather_humidity_label =
        make_label(metrics, humidity, g_ui.font_body, 0x18396f,
                   126, 18, 112, LV_TEXT_ALIGN_CENTER);
    g_ui.weather_visibility_label =
        make_label(metrics, visibility, g_ui.font_body, 0x18396f,
                   247, 18, 112, LV_TEXT_ALIGN_CENTER);
    make_label(metrics, g_ui.font_small ? "体感温度" : "FEELS",
               g_ui.font_small, 0x6b7d9d,
               7, 62, 112, LV_TEXT_ALIGN_CENTER);
    make_label(metrics, g_ui.font_small ? "湿度" : "HUMIDITY",
               g_ui.font_small, 0x6b7d9d,
               126, 62, 112, LV_TEXT_ALIGN_CENTER);
    make_label(metrics, g_ui.font_small ? "能见度" : "VISIBILITY",
               g_ui.font_small, 0x6b7d9d,
               247, 62, 112, LV_TEXT_ALIGN_CENTER);
    make_box(metrics, 121, 16, 2, 72, 0x78a9df, 1, LV_OPA_50);
    make_box(metrics, 242, 16, 2, 72, 0x78a9df, 1, LV_OPA_50);

    if (g_ui.weather_snapshot.state == OPENVELA_UI_WEATHER_ERROR) {
        snprintf(update, sizeof(update), "%s",
                 g_ui.font_small ? "缓存数据 · 网络重试中" :
                                   "NETWORK RETRYING");
    } else if (snapshot && *snapshot->updated_at) {
        snprintf(update, sizeof(update), "%s 更新", snapshot->updated_at);
    } else {
        snprintf(update, sizeof(update), "%s",
                 g_ui.font_small ? "正在获取实时天气" :
                                   "LOADING LIVE WEATHER");
    }
    g_ui.weather_update_label =
        make_label(g_ui.weather_view, update, g_ui.font_small, 0xdbe8ff,
                   66, 351, 300, LV_TEXT_ALIGN_CENTER);

    weather_selected_preview_path(preview, sizeof(preview));
    make_asset_image(g_ui.weather_view, preview, 158, 371, 144, 104);
    weather_add_page_dots(g_ui.weather_view, UI_WEATHER_NOW, 2, 460);
    make_label(g_ui.weather_view,
               g_ui.font_small ? "左滑看预报 · 上下滑返回" :
                                 "LEFT: FORECAST · VERTICAL: BACK",
               g_ui.font_small, 0xdbe8ff,
               37, 479, 358, LV_TEXT_ALIGN_CENTER);
}

static void create_weather_forecast_page(void)
{
    const struct openvela_ui_weather_snapshot_s *snapshot =
        weather_detail_snapshot();
    int highest = -100;
    int lowest = 100;
    int index;
    char range[80];

    weather_detail_header(snapshot);
    make_label(g_ui.weather_view,
               g_ui.font_body ? "未来3天预报" : "3-DAY FORECAST",
               g_ui.font_body, 0xffffff,
               55, 100, 322, LV_TEXT_ALIGN_CENTER);

    if (snapshot) {
        for (index = 0; index < OPENVELA_UI_WEATHER_FORECAST_DAYS;
             index++) {
            int maximum = atoi(snapshot->forecast[index].maximum);
            int minimum = atoi(snapshot->forecast[index].minimum);

            if (strcmp(snapshot->forecast[index].maximum, "--") != 0 &&
                maximum > highest) {
                highest = maximum;
            }
            if (strcmp(snapshot->forecast[index].minimum, "--") != 0 &&
                minimum < lowest) {
                lowest = minimum;
            }
        }
    }
    if (highest > -100 && lowest < 100) {
        snprintf(range, sizeof(range), "最高%d°  最低%d°", highest, lowest);
    } else {
        snprintf(range, sizeof(range), "%s",
                 g_ui.font_small ? "天气数据获取中" : "LOADING");
    }
    g_ui.weather_forecast_range_label =
        make_label(g_ui.weather_view, range, g_ui.font_small, 0xdbe8ff,
                   66, 141, 300, LV_TEXT_ALIGN_CENTER);

    for (index = 0; index < OPENVELA_UI_WEATHER_FORECAST_DAYS; index++) {
        const struct openvela_ui_weather_day_s *day =
            snapshot ? &snapshot->forecast[index] : NULL;
        int32_t top = 177 + index * 88;
        char icon_path[UI_FRAME_PATH_SIZE];
        char temperature[40];
        lv_obj_t *card = weather_card(g_ui.weather_view, 30, top,
                                      372, 76, false);

        weather_icon_path(icon_path, sizeof(icon_path),
                          day ? day->icon : "999");
        g_ui.weather_forecast_weekday_label[index] =
            make_label(card, day ? day->weekday : "--",
                       g_ui.font_small, 0x173366,
                       16, 25, 64, LV_TEXT_ALIGN_CENTER);
        g_ui.weather_forecast_icon_image[index] =
            make_asset_image(card, icon_path, 86, 8, 128, 58);
        g_ui.weather_forecast_text_label[index] =
            make_label(card, day ? day->text : "--",
                       g_ui.font_small, 0x425d86,
                       151, 25, 92, LV_TEXT_ALIGN_CENTER);
        snprintf(temperature, sizeof(temperature), "%s°~%s°",
                 day ? day->minimum : "--",
                 day ? day->maximum : "--");
        g_ui.weather_forecast_temperature_label[index] =
            make_label(card, temperature, g_ui.font_small, 0x173366,
                       245, 25, 112, LV_TEXT_ALIGN_CENTER);
    }

    weather_add_page_dots(g_ui.weather_view, UI_WEATHER_FORECAST, 2, 455);
    make_label(g_ui.weather_view,
               g_ui.font_small ? "右滑看当前 · 上下滑返回" :
                                 "RIGHT: NOW · VERTICAL: BACK",
               g_ui.font_small, 0xdbe8ff,
               37, 477, 358, LV_TEXT_ALIGN_CENTER);
}

static void weather_refresh_detail_objects(void)
{
    const struct openvela_ui_weather_snapshot_s *snapshot;
    const ui_weather_city_t *city;
    char text[160];
    char path[UI_FRAME_PATH_SIZE];
    int index;

    if (g_ui.overlay_page != UI_OVERLAY_WEATHER ||
        g_ui.weather_stage != UI_WEATHER_DETAIL ||
        g_ui.weather_transitioning ||
        !g_ui.weather_view || !lv_obj_is_valid(g_ui.weather_view) ||
        g_ui.weather_selected_city < 0 ||
        g_ui.weather_selected_city >= weather_city_count()) {
        return;
    }

    city = weather_city_at(g_ui.weather_selected_city);
    if (!city) {
        return;
    }
    snapshot = weather_detail_snapshot();
    weather_set_label(g_ui.weather_header_name_label,
                      snapshot && *snapshot->location_name ?
                      snapshot->location_name : city->name);
    snprintf(text, sizeof(text), "%s · %s",
             snapshot && *snapshot->administrative_area ?
             snapshot->administrative_area : city->administrative_area,
             snapshot && *snapshot->country ? snapshot->country : "中国");
    weather_set_label(g_ui.weather_header_metadata_label, text);

    if (g_ui.weather_detail_page == UI_WEATHER_NOW) {
        weather_icon_path(path, sizeof(path),
                          snapshot ? snapshot->icon : "999");
        weather_set_image(g_ui.weather_icon_image, path);
        weather_add_suffix(text, sizeof(text),
                           snapshot ? snapshot->temperature : "--", "°");
        weather_set_label(g_ui.weather_temperature_label, text);

        if (snapshot) {
            weather_set_label(g_ui.weather_condition_label, snapshot->text);
        } else if (g_ui.weather_snapshot.state ==
                   OPENVELA_UI_WEATHER_ERROR) {
            weather_set_label(g_ui.weather_condition_label,
                              g_ui.font_body ? "网络连接失败" :
                                               "NETWORK ERROR");
        } else {
            weather_set_label(g_ui.weather_condition_label,
                              g_ui.font_body ? "天气获取中" : "LOADING");
        }

        weather_add_suffix(text, sizeof(text),
                           snapshot ? snapshot->feels_like : "--", "°");
        weather_set_label(g_ui.weather_feels_label, text);
        weather_add_suffix(text, sizeof(text),
                           snapshot ? snapshot->humidity : "--", "%");
        weather_set_label(g_ui.weather_humidity_label, text);
        weather_add_suffix(text, sizeof(text),
                           snapshot ? snapshot->visibility : "--", "km");
        weather_set_label(g_ui.weather_visibility_label, text);

        if (g_ui.weather_snapshot.state == OPENVELA_UI_WEATHER_ERROR) {
            snprintf(text, sizeof(text), "%s",
                     g_ui.font_small ? "代理不可达 · 自动重试中" :
                                       "PROXY UNREACHABLE · RETRYING");
        } else if (snapshot && *snapshot->updated_at) {
            snprintf(text, sizeof(text), "%s 更新", snapshot->updated_at);
        } else {
            snprintf(text, sizeof(text), "%s",
                     g_ui.font_small ? "正在获取实时天气" :
                                       "LOADING LIVE WEATHER");
        }
        weather_set_label(g_ui.weather_update_label, text);
        return;
    }

    {
        int highest = -100;
        int lowest = 100;

        if (snapshot) {
            for (index = 0;
                 index < OPENVELA_UI_WEATHER_FORECAST_DAYS; index++) {
                int maximum = atoi(snapshot->forecast[index].maximum);
                int minimum = atoi(snapshot->forecast[index].minimum);

                if (strcmp(snapshot->forecast[index].maximum, "--") != 0 &&
                    maximum > highest) {
                    highest = maximum;
                }
                if (strcmp(snapshot->forecast[index].minimum, "--") != 0 &&
                    minimum < lowest) {
                    lowest = minimum;
                }
            }
        }
        if (highest > -100 && lowest < 100) {
            snprintf(text, sizeof(text), "最高%d°  最低%d°",
                     highest, lowest);
        } else if (g_ui.weather_snapshot.state ==
                   OPENVELA_UI_WEATHER_ERROR) {
            snprintf(text, sizeof(text), "%s",
                     g_ui.font_small ? "代理不可达 · 自动重试中" :
                                       "PROXY UNREACHABLE · RETRYING");
        } else {
            snprintf(text, sizeof(text), "%s",
                     g_ui.font_small ? "天气数据获取中" : "LOADING");
        }
        weather_set_label(g_ui.weather_forecast_range_label, text);
    }

    for (index = 0; index < OPENVELA_UI_WEATHER_FORECAST_DAYS; index++) {
        const struct openvela_ui_weather_day_s *day =
            snapshot ? &snapshot->forecast[index] : NULL;

        weather_set_label(g_ui.weather_forecast_weekday_label[index],
                          day ? day->weekday : "--");
        weather_icon_path(path, sizeof(path), day ? day->icon : "999");
        weather_set_image(g_ui.weather_forecast_icon_image[index], path);
        weather_set_label(g_ui.weather_forecast_text_label[index],
                          day ? day->text : "--");
        snprintf(text, sizeof(text), "%s°~%s°",
                 day ? day->minimum : "--",
                 day ? day->maximum : "--");
        weather_set_label(g_ui.weather_forecast_temperature_label[index],
                          text);
    }
}

static void render_weather_page(void)
{
    if (!g_ui.weather_view || !lv_obj_is_valid(g_ui.weather_view)) {
        return;
    }

    weather_reset_page_objects();
    lv_obj_clean(g_ui.weather_view);
    create_weather_background(g_ui.weather_view);
    if (g_ui.weather_stage == UI_WEATHER_CITIES) {
        create_weather_city_list();
    } else if (g_ui.weather_detail_page == UI_WEATHER_FORECAST) {
        create_weather_forecast_page();
    } else {
        create_weather_now_page();
    }
}

static void create_weather_page(void)
{
    g_ui.weather_view = lv_obj_create(g_ui.overlay);
    lv_obj_remove_style_all(g_ui.weather_view);
    lv_obj_set_size(g_ui.weather_view, g_ui.width, g_ui.height);
    lv_obj_set_pos(g_ui.weather_view, 0, 0);
    lv_obj_set_style_bg_color(g_ui.weather_view,
                              lv_color_hex(0x07122f), 0);
    lv_obj_set_style_bg_opa(g_ui.weather_view, LV_OPA_COVER, 0);
    lv_obj_clear_flag(g_ui.weather_view, LV_OBJ_FLAG_CLICKABLE |
                                         LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(g_ui.weather_view, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static void weather_switch_async(void *argument)
{
    int target = (int)(intptr_t)argument;

    g_ui.weather_switch_pending = false;
    if (g_ui.overlay_page != UI_OVERLAY_WEATHER ||
        g_ui.weather_transitioning ||
        !g_ui.weather_view || !lv_obj_is_valid(g_ui.weather_view)) {
        return;
    }

    if (target == UI_WEATHER_SWITCH_CITIES) {
        g_ui.weather_stage = UI_WEATHER_CITIES;
    } else {
        g_ui.weather_stage = UI_WEATHER_DETAIL;
        g_ui.weather_detail_page = target == UI_WEATHER_SWITCH_FORECAST ?
                                   UI_WEATHER_FORECAST : UI_WEATHER_NOW;
    }
    render_weather_page();
}

static void weather_queue_switch(int target)
{
    if (g_ui.weather_transitioning || g_ui.weather_switch_pending) {
        return;
    }

    g_ui.weather_switch_pending = true;
    if (lv_async_call(weather_switch_async,
                      (void *)(intptr_t)target) != LV_RESULT_OK) {
        g_ui.weather_switch_pending = false;
    }
}

static void weather_city_page_async(void *argument)
{
    int page = (int)(intptr_t)argument;

    g_ui.weather_switch_pending = false;
    if (g_ui.overlay_page != UI_OVERLAY_WEATHER ||
        g_ui.weather_transitioning ||
        g_ui.weather_stage != UI_WEATHER_CITIES ||
        !g_ui.weather_view || !lv_obj_is_valid(g_ui.weather_view)) {
        return;
    }

    g_ui.weather_city_page = LV_MAX(0, LV_MIN(page,
        weather_city_page_count() - 1));
    render_weather_page();
}

static void weather_queue_city_page(int page)
{
    page = LV_MAX(0, LV_MIN(page, weather_city_page_count() - 1));
    if (g_ui.weather_transitioning || g_ui.weather_switch_pending ||
        page == g_ui.weather_city_page) {
        return;
    }

    g_ui.weather_switch_pending = true;
    if (lv_async_call(weather_city_page_async,
                      (void *)(intptr_t)page) != LV_RESULT_OK) {
        g_ui.weather_switch_pending = false;
    }
}

static void weather_update_home_labels(void)
{
    char temperature[24];
    char description[96];
    char icon_path[UI_FRAME_PATH_SIZE];
    int index;

    if (g_ui.home_weather_valid) {
        weather_add_suffix(temperature, sizeof(temperature),
                           g_ui.home_weather_snapshot.temperature, "°");
        snprintf(description, sizeof(description), "%s · %s",
                 g_ui.home_weather_snapshot.location_name,
                 g_ui.home_weather_snapshot.text);
        weather_icon_path(icon_path, sizeof(icon_path),
                          g_ui.home_weather_snapshot.icon);
    } else {
        snprintf(temperature, sizeof(temperature), "--°");
        snprintf(description, sizeof(description), "%s",
                 g_ui.font_small ? "北京 · 更新中" :
                                   "BEIJING · LOADING");
        weather_icon_path(icon_path, sizeof(icon_path), "999");
    }

    for (index = 0; index < 2; index++) {
        if (g_ui.home_weather_temp_label[index] &&
            lv_obj_is_valid(g_ui.home_weather_temp_label[index])) {
            lv_label_set_text(g_ui.home_weather_temp_label[index],
                              temperature);
        }
        if (g_ui.home_weather_desc_label[index] &&
            lv_obj_is_valid(g_ui.home_weather_desc_label[index])) {
            lv_label_set_text(g_ui.home_weather_desc_label[index],
                              description);
        }
        if (g_ui.home_weather_image[index] &&
            lv_obj_is_valid(g_ui.home_weather_image[index]) &&
            access(icon_path, R_OK) == 0) {
            lv_image_set_src(g_ui.home_weather_image[index], icon_path);
        }
    }
}

static void weather_timer_tick(lv_timer_t *timer)
{
    struct openvela_ui_weather_snapshot_s snapshot;
    bool changed = false;
    int retry_limit;

    LV_UNUSED(timer);
    if (openvela_ui_weather_snapshot(&snapshot) == 0 &&
        snapshot.revision != g_ui.weather_seen_revision) {
        changed = snapshot.revision != g_ui.weather_snapshot.revision &&
                  snapshot.state != OPENVELA_UI_WEATHER_LOADING;
        g_ui.weather_seen_revision = snapshot.revision;
        g_ui.weather_snapshot = snapshot;
        g_ui.weather_poll_ticks = 0;
        if (snapshot.has_data &&
            weather_snapshot_matches_city(&snapshot, 0)) {
            g_ui.home_weather_snapshot = snapshot;
            g_ui.home_weather_valid = true;
            weather_update_home_labels();
        }
    }

    /* Network completion must never destroy and rebuild the weather object
     * tree from this timer.  On R528 that used to submit a second full-screen
     * PNG/G2D workload immediately after a socket timeout and could corrupt
     * the framebuffer update path.  Keep the tree stable and update only the
     * persistent labels/images in place. */
    if (changed && g_ui.overlay_page == UI_OVERLAY_WEATHER &&
        g_ui.weather_stage == UI_WEATHER_DETAIL &&
        !g_ui.weather_transitioning &&
        !g_ui.weather_switch_pending &&
        g_ui.weather_view && lv_obj_is_valid(g_ui.weather_view) &&
        weather_snapshot_matches_city(&g_ui.weather_snapshot,
                                      g_ui.weather_selected_city)) {
        weather_refresh_detail_objects();
    }

    if (g_ui.weather_poll_ticks < UINT16_MAX) {
        g_ui.weather_poll_ticks++;
    }
    retry_limit = UI_WEATHER_REFRESH_TICKS;
    if (g_ui.weather_snapshot.state == OPENVELA_UI_WEATHER_ERROR &&
        g_ui.weather_snapshot.error != -EINVAL &&
        g_ui.weather_snapshot.error != -EACCES &&
        g_ui.weather_snapshot.error != -ENOENT &&
        g_ui.weather_snapshot.error != -EPROTONOSUPPORT) {
        retry_limit = UI_WEATHER_RETRY_TICKS;
    }
    if (g_ui.weather_snapshot.state != OPENVELA_UI_WEATHER_LOADING &&
        g_ui.weather_poll_ticks >= retry_limit) {
        int city_index = g_ui.overlay_page == UI_OVERLAY_WEATHER &&
                         g_ui.weather_stage == UI_WEATHER_DETAIL ?
                         g_ui.weather_selected_city : 0;

        weather_request_city(city_index);
    }
}

static int random_range(int minimum, int maximum)
{
    return minimum + rand() % (maximum - minimum + 1);
}

static int random_triangle(int minimum, int maximum)
{
    return (random_range(minimum, maximum) +
            random_range(minimum, maximum) + 1) / 2;
}

static int realistic_heart_rate(void)
{
    return random_triangle(60, 100);
}

static void realistic_blood_pressure(int *systolic, int *diastolic,
                                     int *pulse)
{
    int attempt;

    for (attempt = 0; attempt < 12; attempt++) {
        int candidate_diastolic = random_triangle(65, 89);
        int pulse_pressure = random_triangle(35, 52);
        int candidate_systolic = candidate_diastolic + pulse_pressure;

        if (candidate_systolic >= 105 && candidate_systolic <= 139) {
            *systolic = candidate_systolic;
            *diastolic = candidate_diastolic;
            *pulse = realistic_heart_rate();
            return;
        }
    }

    *systolic = 122;
    *diastolic = 78;
    *pulse = realistic_heart_rate();
}

static const char *pressure_status(int systolic, int diastolic)
{
    if (systolic >= 130 || diastolic >= 85) {
        return g_ui.font_body ? "正常偏高" : "HIGH NORMAL";
    }
    if (systolic < 110 || diastolic < 70) {
        return g_ui.font_body ? "正常偏低" : "LOW NORMAL";
    }
    return g_ui.font_body ? "正常血压" : "NORMAL";
}

static void add_health_history(ui_health_record_t *history, uint8_t *count,
                               int primary, int secondary, int pulse)
{
    int index;

    for (index = UI_HEALTH_HISTORY_COUNT - 1; index > 0; index--) {
        history[index] = history[index - 1];
    }

    history[0].primary = primary;
    history[0].secondary = secondary;
    history[0].pulse = pulse;
    history[0].measured_at = time(NULL);
    if (*count < UI_HEALTH_HISTORY_COUNT) {
        (*count)++;
    }
}

static void health_navigation_hint(void)
{
    int index;
    int32_t start = 155;

    for (index = 0; index < UI_HEALTH_PAGE_COUNT; index++) {
        make_box(g_ui.health_view, start + index * 31, 479,
                 index == g_ui.health_page ? 20 : 8, 7,
                 0xffffff, 4,
                 index == g_ui.health_page ? LV_OPA_COVER : LV_OPA_40);
    }
}

static void create_health_disclaimer(void)
{
    make_label(g_ui.health_view,
               g_ui.font_small ? "模拟数据，仅供演示" : "SIMULATED DATA",
               g_ui.font_small, 0xb9bdc8,
               56, 448, 320, LV_TEXT_ALIGN_CENTER);
}

static void create_heart_rate_page(void)
{
    char value[16];

    make_label(g_ui.health_view,
               g_ui.font_title ? "心率" : "HEART RATE",
               g_ui.font_title, 0xffffff,
               76, 24, 280, LV_TEXT_ALIGN_CENTER);

    g_ui.health_heart_icon = make_asset_image(g_ui.health_view,
                                               UI_ICON_HEALTH,
                                               121, 78, 256, 190);
    if (!g_ui.health_heart_icon) {
        lv_obj_t *heart = make_box(g_ui.health_view, 132, 88, 168, 168,
                                   0xc92035, 84, LV_OPA_COVER);
        make_label(heart, "♥", g_ui.font_title, 0xffffff,
                   0, 48, 168, LV_TEXT_ALIGN_CENTER);
    } else {
        g_ui.health_heart_scale =
            (uint16_t)((sx(190) * 256 + 128) / 256);
    }

    g_ui.health_state_label = make_label(g_ui.health_view,
        g_ui.health_measuring ?
            (g_ui.font_body ? "正在测量" : "MEASURING") :
            (g_ui.font_body ? "模拟心率" : "SIMULATED RATE"),
        g_ui.font_body, 0xd7d9df,
        73, 285, 286, LV_TEXT_ALIGN_CENTER);

    snprintf(value, sizeof(value), "%d", g_ui.heart_rate);
    g_ui.health_value_label = make_label(g_ui.health_view, value,
        g_ui.font_clock, 0xff3030,
        82, 322, 210, LV_TEXT_ALIGN_RIGHT);
    make_label(g_ui.health_view, "BPM", g_ui.font_body, 0xd7d9df,
               302, 355, 80, LV_TEXT_ALIGN_LEFT);

    g_ui.health_hint_label = make_label(g_ui.health_view,
        g_ui.health_measuring ?
            (g_ui.font_small ? "请保持静止" : "KEEP STILL") :
            (g_ui.font_small ? "点击任意位置开始测量" : "TAP TO MEASURE"),
        g_ui.font_small, 0xffffff,
        46, 420, 340, LV_TEXT_ALIGN_CENTER);
    create_health_disclaimer();
}

static void create_blood_pressure_page(void)
{
    char value[16];
    lv_obj_t *card;

    make_label(g_ui.health_view,
               g_ui.font_title ? "血压" : "BLOOD PRESSURE",
               g_ui.font_title, 0xffffff,
               66, 22, 300, LV_TEXT_ALIGN_CENTER);

    card = make_box(g_ui.health_view, 23, 88, 386, 304,
                    0xffffff, 30, LV_OPA_90);
    g_ui.health_state_label = make_label(card,
        g_ui.health_measuring ?
            (g_ui.font_body ? "正在测量" : "MEASURING") :
            (g_ui.font_body ? "模拟血压" : "SIMULATED"),
        g_ui.font_body, 0x17366f,
        63, 18, 260, LV_TEXT_ALIGN_CENTER);

    snprintf(value, sizeof(value), "%d", g_ui.systolic);
    g_ui.health_value_label = make_label(card, value,
        g_ui.font_title, 0xff3b30,
        24, 72, 132, LV_TEXT_ALIGN_CENTER);
    make_label(card, "/", g_ui.font_title, 0x17366f,
               166, 72, 54, LV_TEXT_ALIGN_CENTER);
    snprintf(value, sizeof(value), "%d", g_ui.diastolic);
    g_ui.health_secondary_label = make_label(card, value,
        g_ui.font_title, 0x2f80ed,
        226, 72, 132, LV_TEXT_ALIGN_CENTER);
    make_label(card, g_ui.font_small ? "收缩压" : "SYS",
               g_ui.font_small, 0x64748b,
               24, 137, 132, LV_TEXT_ALIGN_CENTER);
    make_label(card, g_ui.font_small ? "舒张压" : "DIA",
               g_ui.font_small, 0x64748b,
               226, 137, 132, LV_TEXT_ALIGN_CENTER);
    make_label(card, "mmHg", g_ui.font_small, 0x64748b,
               133, 168, 120, LV_TEXT_ALIGN_CENTER);

    g_ui.health_status_label = make_label(card,
        pressure_status(g_ui.systolic, g_ui.diastolic),
        g_ui.font_small, 0x14804a,
        93, 205, 200, LV_TEXT_ALIGN_CENTER);
    make_label(card, g_ui.font_small ? "脉搏" : "PULSE",
               g_ui.font_small, 0x64748b,
               78, 254, 70, LV_TEXT_ALIGN_LEFT);
    snprintf(value, sizeof(value), "%d", g_ui.pulse);
    g_ui.health_pulse_label = make_label(card, value,
        g_ui.font_body, 0x17366f,
        150, 246, 78, LV_TEXT_ALIGN_CENTER);
    make_label(card, "BPM", g_ui.font_small, 0x64748b,
               238, 254, 70, LV_TEXT_ALIGN_LEFT);

    g_ui.health_hint_label = make_label(g_ui.health_view,
        g_ui.health_measuring ?
            (g_ui.font_small ? "请保持静止" : "KEEP STILL") :
            (g_ui.font_small ? "点击任意位置开始测量" : "TAP TO MEASURE"),
        g_ui.font_small, 0xffffff,
        46, 414, 340, LV_TEXT_ALIGN_CENTER);
    create_health_disclaimer();
}

static void create_health_history_page(bool pressure)
{
    ui_health_record_t *history = pressure ?
        g_ui.pressure_history : g_ui.heart_history;
    uint8_t count = pressure ?
        g_ui.pressure_history_count : g_ui.heart_history_count;
    const char *title;
    lv_obj_t *card;
    int index;

    title = pressure ?
        (g_ui.font_title ? "血压历史" : "PRESSURE HISTORY") :
        (g_ui.font_title ? "心率历史" : "HEART HISTORY");
    make_label(g_ui.health_view, title, g_ui.font_title, 0xffffff,
               46, 22, 340, LV_TEXT_ALIGN_CENTER);

    if (count == 0) {
        make_label(g_ui.health_view,
                   g_ui.font_body ? "暂无测量记录" : "NO RECORDS",
                   g_ui.font_body, 0xffffff,
                   56, 224, 320, LV_TEXT_ALIGN_CENTER);
        create_health_disclaimer();
        return;
    }

    card = make_box(g_ui.health_view, 18, 88, 396, 326,
                    0xffffff, 26, LV_OPA_90);
    for (index = 0; index < count; index++) {
        struct tm local;
        char date_text[24];
        char value_text[32];
        int32_t top = 10 + index * 44;

        localtime_r(&history[index].measured_at, &local);
        snprintf(date_text, sizeof(date_text), "%02d-%02d %02d:%02d",
                 local.tm_mon + 1, local.tm_mday,
                 local.tm_hour, local.tm_min);
        if (pressure) {
            snprintf(value_text, sizeof(value_text), "%d/%d  %d BPM",
                     history[index].primary, history[index].secondary,
                     history[index].pulse);
        } else {
            snprintf(value_text, sizeof(value_text), "%d BPM",
                     history[index].primary);
        }

        make_box(card, 18, top + 13, 10, 10,
                 pressure ? 0x2f80ed : 0xff3030,
                 5, LV_OPA_COVER);
        make_label(card, date_text, g_ui.font_small, 0x68758c,
                   42, top + 6, 150, LV_TEXT_ALIGN_LEFT);
        make_label(card, value_text, g_ui.font_small,
                   pressure ? 0x17366f : 0xff3030,
                   187, top + 6, 190, LV_TEXT_ALIGN_RIGHT);
        if (index < count - 1) {
            make_box(card, 20, top + 42, 356, 1,
                     0xd7e5f7, 0, LV_OPA_COVER);
        }
    }
    create_health_disclaimer();
}

static void render_health_page(void)
{
    if (g_ui.health_view && lv_obj_is_valid(g_ui.health_view)) {
        lv_obj_delete(g_ui.health_view);
    }

    g_ui.health_view = lv_obj_create(g_ui.overlay);
    lv_obj_remove_style_all(g_ui.health_view);
    lv_obj_set_size(g_ui.health_view, g_ui.width, g_ui.height);
    lv_obj_set_pos(g_ui.health_view, 0, 0);
    lv_obj_clear_flag(g_ui.health_view, LV_OBJ_FLAG_CLICKABLE |
                                        LV_OBJ_FLAG_SCROLLABLE);
    g_ui.health_value_label = NULL;
    g_ui.health_secondary_label = NULL;
    g_ui.health_pulse_label = NULL;
    g_ui.health_state_label = NULL;
    g_ui.health_status_label = NULL;
    g_ui.health_hint_label = NULL;
    g_ui.health_heart_icon = NULL;

    if (g_ui.health_page == UI_HEALTH_HEART_RATE) {
        create_heart_rate_page();
    } else if (g_ui.health_page == UI_HEALTH_HEART_HISTORY) {
        create_health_history_page(false);
    } else if (g_ui.health_page == UI_HEALTH_BLOOD_PRESSURE) {
        create_blood_pressure_page();
    } else {
        create_health_history_page(true);
    }
    health_navigation_hint();
}

static void create_health_page(void)
{
    render_health_page();
}

static void stop_health_measurement(void)
{
    if (g_ui.health_measure_timer) {
        lv_timer_delete(g_ui.health_measure_timer);
        g_ui.health_measure_timer = NULL;
    }
    g_ui.health_measuring = false;
    g_ui.health_measure_ticks = 0;
}

static void finish_health_measurement(void)
{
    char value[16];

    if (g_ui.health_page == UI_HEALTH_HEART_RATE) {
        g_ui.heart_rate = g_ui.target_heart_rate;
        snprintf(value, sizeof(value), "%d", g_ui.heart_rate);
        if (g_ui.health_value_label) {
            lv_label_set_text(g_ui.health_value_label, value);
        }
        add_health_history(g_ui.heart_history,
                           &g_ui.heart_history_count,
                           g_ui.heart_rate, 0, 0);
        openvela_ui_sync_record_heart_rate((uint16_t)g_ui.heart_rate);
        if (g_ui.health_heart_icon) {
            lv_image_set_scale(g_ui.health_heart_icon,
                               g_ui.health_heart_scale);
        }
    } else {
        g_ui.systolic = g_ui.target_systolic;
        g_ui.diastolic = g_ui.target_diastolic;
        g_ui.pulse = g_ui.target_pulse;
        if (g_ui.health_value_label) {
            lv_label_set_text_fmt(g_ui.health_value_label,
                                  "%d", g_ui.systolic);
        }
        if (g_ui.health_secondary_label) {
            lv_label_set_text_fmt(g_ui.health_secondary_label,
                                  "%d", g_ui.diastolic);
        }
        if (g_ui.health_pulse_label) {
            lv_label_set_text_fmt(g_ui.health_pulse_label,
                                  "%d", g_ui.pulse);
        }
        if (g_ui.health_status_label) {
            lv_label_set_text(g_ui.health_status_label,
                              pressure_status(g_ui.systolic,
                                              g_ui.diastolic));
        }
        add_health_history(g_ui.pressure_history,
                           &g_ui.pressure_history_count,
                           g_ui.systolic, g_ui.diastolic, g_ui.pulse);
        openvela_ui_sync_record_blood_pressure(
            (uint16_t)g_ui.systolic, (uint16_t)g_ui.diastolic,
            (uint16_t)g_ui.pulse);
    }

    if (g_ui.health_state_label) {
        lv_label_set_text(g_ui.health_state_label,
                          g_ui.font_body ? "测量结果" : "RESULT");
    }
    if (g_ui.health_hint_label) {
        lv_label_set_text(g_ui.health_hint_label,
            g_ui.font_small ? "点击任意位置重新测量" : "TAP TO MEASURE AGAIN");
    }
    g_ui.health_measuring = false;
    g_ui.health_measure_ticks = 0;
}

static void health_measure_tick(lv_timer_t *timer)
{
    int jitter;

    g_ui.health_measure_ticks++;
    if (g_ui.health_page == UI_HEALTH_HEART_RATE) {
        jitter = random_range(-4, 4);
        g_ui.heart_rate = g_ui.target_heart_rate + jitter;
        if (g_ui.health_value_label) {
            lv_label_set_text_fmt(g_ui.health_value_label,
                                  "%d", g_ui.heart_rate);
        }
        if (g_ui.health_heart_icon) {
            uint32_t scale = g_ui.health_heart_scale *
                ((g_ui.health_measure_ticks & 1) ? 106 : 96) / 100;
            lv_image_set_scale(g_ui.health_heart_icon, scale);
        }
    } else {
        if (g_ui.health_value_label) {
            lv_label_set_text_fmt(g_ui.health_value_label, "%d",
                g_ui.target_systolic + random_range(-3, 3));
        }
        if (g_ui.health_secondary_label) {
            lv_label_set_text_fmt(g_ui.health_secondary_label, "%d",
                g_ui.target_diastolic + random_range(-2, 2));
        }
        if (g_ui.health_pulse_label) {
            lv_label_set_text_fmt(g_ui.health_pulse_label, "%d",
                g_ui.target_pulse + random_range(-3, 3));
        }
    }

    if (g_ui.health_measure_ticks >= UI_HEALTH_MEASURE_TICKS) {
        g_ui.health_measure_timer = NULL;
        lv_timer_delete(timer);
        finish_health_measurement();
    }
}

static void start_health_measurement(void)
{
    if (g_ui.health_measuring ||
        (g_ui.health_page != UI_HEALTH_HEART_RATE &&
         g_ui.health_page != UI_HEALTH_BLOOD_PRESSURE)) {
        return;
    }

    g_ui.health_measuring = true;
    g_ui.health_measure_ticks = 0;
    if (g_ui.health_page == UI_HEALTH_HEART_RATE) {
        g_ui.target_heart_rate = realistic_heart_rate();
    } else {
        int systolic;
        int diastolic;
        int pulse;

        realistic_blood_pressure(&systolic, &diastolic, &pulse);
        g_ui.target_systolic = systolic;
        g_ui.target_diastolic = diastolic;
        g_ui.target_pulse = pulse;
    }

    if (g_ui.health_state_label) {
        lv_label_set_text(g_ui.health_state_label,
                          g_ui.font_body ? "正在测量" : "MEASURING");
    }
    if (g_ui.health_hint_label) {
        lv_label_set_text(g_ui.health_hint_label,
                          g_ui.font_small ? "请保持静止" : "KEEP STILL");
    }
    g_ui.health_measure_timer = lv_timer_create(health_measure_tick,
                                                 240, NULL);
}

static void change_health_page(int direction)
{
    int page;

    if (g_ui.health_measuring) {
        return;
    }
    page = g_ui.health_page + direction;
    if (page < 0 || page >= UI_HEALTH_PAGE_COUNT) {
        return;
    }
    g_ui.health_page = page;
    render_health_page();
}

static const char *music_state_text(void)
{
    switch (g_ui.music_state) {
    case UI_MUSIC_PREPARING:
        return g_ui.font_small ? "正在加载" : "LOADING";
    case UI_MUSIC_PLAYING:
        return g_ui.font_small ? "正在播放" : "PLAYING";
    case UI_MUSIC_PAUSING:
        return g_ui.font_small ? "正在暂停" : "PAUSING";
    case UI_MUSIC_PAUSED:
        return g_ui.font_small ? "已暂停" : "PAUSED";
    case UI_MUSIC_ERROR:
        return g_ui.font_small ? "音频打开失败" : "AUDIO ERROR";
    default:
        return g_ui.font_small ? "点击播放" : "TAP TO PLAY";
    }
}

static void music_set_player_volume(uint8_t volume)
{
    int ret = -ENOSYS;
#ifdef CONFIG_ARCH_SUN8IW20
    unsigned int reg_volume;
    int left;
    int right;
#endif

    if (!g_ui.music_player) {
        return;
    }

    if (volume > 100) {
        volume = 100;
    }

#ifndef CONFIG_AUDIO_EXCLUDE_VOLUME
    ret = nxplayer_setvolume(g_ui.music_player, volume);
#endif

#ifdef CONFIG_ARCH_SUN8IW20
    /* R528 exposes no standard NuttX volume feature unit.  Its usable
     * controls are the left/right DAC digital gains (the same controls as
     * `amixerset 6` and `amixerset 7`). */
    reg_volume = (unsigned int)volume * UI_MUSIC_DAC_MAX / 100U;
    left = snd_ctl_set_bynum(CONFIG_AW_AUDIO_CODEC_DEFAULT_CARDNAME,
                             6, reg_volume);
    right = snd_ctl_set_bynum(CONFIG_AW_AUDIO_CODEC_DEFAULT_CARDNAME,
                              7, reg_volume);

    if (left >= 0 && right >= 0) {
        ret = 0;
    } else {
        syslog(LOG_ERR,
               "openvela_ui: DAC volume setup failed: left=%d right=%d\n",
               left, right);
    }
#endif

    if (ret >= 0) {
        g_ui.music_applied_volume = volume;
    }
}

static uint64_t music_monotonic_ms(void)
{
    struct timespec now;

    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        return 0;
    }

    return (uint64_t)now.tv_sec * 1000U +
           (uint64_t)now.tv_nsec / 1000000U;
}

static bool music_open_player(void)
{
#ifdef CONFIG_NXPLAYER_INCLUDE_PREFERRED_DEVICE
    int ret;
#endif

    if (g_ui.music_player) {
        return true;
    }

    g_ui.music_player = nxplayer_create();
    if (!g_ui.music_player) {
        syslog(LOG_ERR, "openvela_ui: nxplayer_create failed\n");
        g_ui.music_state = UI_MUSIC_ERROR;
        return false;
    }

#ifdef CONFIG_NXPLAYER_INCLUDE_PREFERRED_DEVICE
    ret = nxplayer_setdevice(g_ui.music_player, "/dev/audio/pcm0p");

    if (ret < 0) {
        syslog(LOG_ERR, "openvela_ui: cannot select pcm0p: %d\n",
               ret);
        nxplayer_release(g_ui.music_player);
        g_ui.music_player = NULL;
        g_ui.music_state = UI_MUSIC_ERROR;
        return false;
    }
#endif

    return true;
}

static void music_prepare_current(bool start_after_prepare)
{
    const ui_music_track_t *track = &g_music_tracks[g_ui.music_track];
    struct stat file_stat;
    uint64_t duration_ms;
    int ret;

    if (stat(track->path, &file_stat) != 0 || file_stat.st_size <= 0) {
        syslog(LOG_ERR, "openvela_ui: cannot read track %s: %d\n",
               track->path, errno);
        g_ui.music_state = UI_MUSIC_ERROR;
        return;
    }

    duration_ms = (uint64_t)file_stat.st_size * 1000U /
                  UI_MUSIC_PCM_BYTES_PER_SECOND;
    g_ui.music_position_ms = 0;
    g_ui.music_duration_ms = duration_ms > UINT32_MAX ?
                             UINT32_MAX : (unsigned int)duration_ms;
    g_ui.music_last_clock_ms = 0;
    g_ui.music_poll_ticks = 0;
    g_ui.music_fade = UI_MUSIC_FADE_NONE;

    /* NxPlayer starts raw PCM immediately; an idle track therefore does not
     * need a separate prepare phase. */
    if (!start_after_prepare) {
        g_ui.music_state = UI_MUSIC_IDLE;
        return;
    }

    if (!music_open_player()) {
        g_ui.music_state = UI_MUSIC_ERROR;
        return;
    }

    /* Begin muted, then fade up after the NxPlayer worker reports PLAYING.
     * This also keeps device start/stop transients out of the speaker. */
    music_set_player_volume(0);
    g_ui.music_state = UI_MUSIC_PREPARING;
    ret = nxplayer_playraw(g_ui.music_player, track->path,
                           AUDIO_FMT_PCM, 0,
                           UI_MUSIC_PCM_CHANNELS,
                           UI_MUSIC_PCM_BITS,
                           UI_MUSIC_PCM_RATE, 0);
    if (ret < 0) {
        syslog(LOG_ERR, "openvela_ui: nxplayer_playraw %s failed: %d\n",
               track->path, ret);
        g_ui.music_state = UI_MUSIC_ERROR;
    }
}

static void music_change_track(int direction, bool autoplay)
{
    int next;
    int ret;

    /* nxplayer_playraw() returns before its worker changes IDLE to PLAYING.
     * Ignore the tiny startup window so a second click cannot race a second
     * worker onto the same player. */
    if (g_ui.music_state == UI_MUSIC_PREPARING) {
        return;
    }

    next = (g_ui.music_track + direction + UI_MUSIC_TRACK_COUNT) %
           UI_MUSIC_TRACK_COUNT;

    g_ui.music_fade = UI_MUSIC_FADE_NONE;
    if (g_ui.music_player &&
        g_ui.music_player->state != UI_NXPLAYER_IDLE) {
        music_set_player_volume(0);
        ret = nxplayer_stop(g_ui.music_player);
        if (ret < 0) {
            syslog(LOG_ERR, "openvela_ui: nxplayer_stop failed: %d\n",
                   ret);
            g_ui.music_state = UI_MUSIC_ERROR;
            return;
        }
    }

    g_ui.music_track = next;
    music_prepare_current(autoplay);
}

static void music_toggle(void)
{
    if (g_ui.music_state == UI_MUSIC_PLAYING) {
        g_ui.music_state = UI_MUSIC_PAUSING;
        g_ui.music_fade = UI_MUSIC_FADE_OUT_PAUSE;
        g_ui.music_fade_step = 0;
    } else if (g_ui.music_state == UI_MUSIC_PAUSED) {
        music_set_player_volume(0);
        if (!g_ui.music_player ||
            nxplayer_resume(g_ui.music_player) < 0) {
            g_ui.music_state = UI_MUSIC_ERROR;
            return;
        }
        g_ui.music_state = UI_MUSIC_PLAYING;
        g_ui.music_fade = UI_MUSIC_FADE_IN_RESUME;
        g_ui.music_fade_step = 0;
        g_ui.music_last_clock_ms = music_monotonic_ms();
    } else if (g_ui.music_state == UI_MUSIC_IDLE ||
               g_ui.music_state == UI_MUSIC_ERROR) {
        music_prepare_current(true);
    }
}

static void music_set_volume(int volume)
{
    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }

    g_ui.music_volume = (uint8_t)volume;
    if (g_ui.music_state == UI_MUSIC_PLAYING &&
        g_ui.music_fade == UI_MUSIC_FADE_NONE) {
        music_set_player_volume(g_ui.music_volume);
    }
}

static void music_set_label_text(lv_obj_t *label, const char *text)
{
    const char *current;

    if (!label) {
        return;
    }

    current = lv_label_get_text(label);
    if (!current || strcmp(current, text) != 0) {
        lv_label_set_text(label, text);
    }
}

static void music_set_image_src(lv_obj_t *image, const char *source)
{
    const void *current;

    if (!image) {
        return;
    }

    current = lv_image_get_src(image);
    if (!current || lv_image_src_get_type(current) != LV_IMAGE_SRC_FILE ||
        strcmp((const char *)current, source) != 0) {
        lv_image_set_src(image, source);
    }
}

static void music_refresh_ui(void)
{
    const ui_music_track_t *track = &g_music_tracks[g_ui.music_track];
    char progress[32];
    char volume[8];
    const char *play_icon;
    uint32_t position_sec = g_ui.music_position_ms / 1000;
    uint32_t duration_sec = g_ui.music_duration_ms / 1000;
    int bar_value = 0;

    if (!g_ui.music_view || !lv_obj_is_valid(g_ui.music_view)) {
        return;
    }
    music_set_label_text(g_ui.music_title_label, track->name);
    music_set_label_text(g_ui.music_artist_label, track->artist);
    music_set_label_text(g_ui.music_status_label, music_state_text());
    if (g_ui.music_progress_label) {
        snprintf(progress, sizeof(progress), "%u:%02u / %u:%02u",
                 (unsigned)(position_sec / 60),
                 (unsigned)(position_sec % 60),
                 (unsigned)(duration_sec / 60),
                 (unsigned)(duration_sec % 60));
        music_set_label_text(g_ui.music_progress_label, progress);
    }
    if (g_ui.music_progress_bar) {
        if (g_ui.music_duration_ms > 0) {
            bar_value = (int)((uint64_t)g_ui.music_position_ms * 1000 /
                              g_ui.music_duration_ms);
        }
        lv_bar_set_value(g_ui.music_progress_bar, bar_value, LV_ANIM_OFF);
    }
    play_icon = g_ui.music_state == UI_MUSIC_PLAYING ||
                g_ui.music_state == UI_MUSIC_PAUSING ?
                UI_MUSIC_ICON_PAUSE : UI_MUSIC_ICON_PLAY;
    music_set_image_src(g_ui.music_play_icon, play_icon);
    snprintf(volume, sizeof(volume), "%u%%", g_ui.music_volume);
    music_set_label_text(g_ui.music_volume_label, volume);
    if (g_ui.music_volume_bar) {
        lv_bar_set_value(g_ui.music_volume_bar, g_ui.music_volume,
                         LV_ANIM_OFF);
    }
}

static void music_update_position(void)
{
    uint64_t now;
    uint64_t elapsed;
    uint64_t position;

    if (g_ui.music_state != UI_MUSIC_PLAYING &&
        g_ui.music_state != UI_MUSIC_PAUSING) {
        g_ui.music_last_clock_ms = 0;
        return;
    }

    now = music_monotonic_ms();
    if (now == 0) {
        return;
    }
    if (g_ui.music_last_clock_ms == 0 || now < g_ui.music_last_clock_ms) {
        g_ui.music_last_clock_ms = now;
        return;
    }

    elapsed = now - g_ui.music_last_clock_ms;
    g_ui.music_last_clock_ms = now;
    position = (uint64_t)g_ui.music_position_ms + elapsed;
    if (g_ui.music_duration_ms > 0 &&
        position > g_ui.music_duration_ms) {
        position = g_ui.music_duration_ms;
    }
    g_ui.music_position_ms = (unsigned int)position;
}

static void music_process_player_state(void)
{
    int state;

    if (!g_ui.music_player) {
        return;
    }

    state = g_ui.music_player->state;
    if (g_ui.music_state == UI_MUSIC_PREPARING) {
        if (state == UI_NXPLAYER_PLAYING) {
            g_ui.music_state = UI_MUSIC_PLAYING;
            g_ui.music_fade = UI_MUSIC_FADE_IN_RESUME;
            g_ui.music_fade_step = 0;
            g_ui.music_poll_ticks = 0;
            g_ui.music_last_clock_ms = music_monotonic_ms();
        } else if (++g_ui.music_poll_ticks >=
                   UI_MUSIC_START_TIMEOUT_TICKS) {
            syslog(LOG_ERR,
                   "openvela_ui: nxplayer did not enter PLAYING (state=%d)\n",
                   state);
            g_ui.music_state = UI_MUSIC_ERROR;
            g_ui.music_fade = UI_MUSIC_FADE_NONE;
        }
        return;
    }

    if ((g_ui.music_state == UI_MUSIC_PLAYING ||
         g_ui.music_state == UI_MUSIC_PAUSING) &&
        state == UI_NXPLAYER_IDLE) {
        g_ui.music_state = UI_MUSIC_IDLE;
        g_ui.music_position_ms = g_ui.music_duration_ms;
        g_ui.music_last_clock_ms = 0;
        music_change_track(1, true);
    } else if (g_ui.music_state == UI_MUSIC_PAUSED &&
               state == UI_NXPLAYER_IDLE) {
        syslog(LOG_ERR, "openvela_ui: nxplayer stopped while paused\n");
        g_ui.music_state = UI_MUSIC_ERROR;
    }
}

static void music_fade_tick(void)
{
    uint8_t volume;

    if (g_ui.music_fade == UI_MUSIC_FADE_NONE || !g_ui.music_player) {
        return;
    }

    g_ui.music_fade_step++;
    if (g_ui.music_fade == UI_MUSIC_FADE_OUT_PAUSE) {
        volume = (uint8_t)((uint32_t)g_ui.music_volume *
                 (UI_MUSIC_FADE_STEPS - g_ui.music_fade_step) /
                 UI_MUSIC_FADE_STEPS);
        music_set_player_volume(volume);
        if (g_ui.music_fade_step >= UI_MUSIC_FADE_STEPS) {
            g_ui.music_fade = UI_MUSIC_FADE_NONE;
            music_set_player_volume(0);
            if (nxplayer_pause(g_ui.music_player) < 0) {
                g_ui.music_state = UI_MUSIC_ERROR;
            } else {
                g_ui.music_state = UI_MUSIC_PAUSED;
                g_ui.music_last_clock_ms = 0;
            }
        }
    } else {
        volume = (uint8_t)((uint32_t)g_ui.music_volume *
                           g_ui.music_fade_step /
                           UI_MUSIC_FADE_STEPS);
        music_set_player_volume(volume);
        if (g_ui.music_fade_step >= UI_MUSIC_FADE_STEPS) {
            g_ui.music_fade = UI_MUSIC_FADE_NONE;
            music_set_player_volume(g_ui.music_volume);
        }
    }
}

static void music_timer_tick(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    music_update_position();
    music_process_player_state();
    music_fade_tick();
    if (!g_ui.low_power) {
        music_refresh_ui();
    }
}

static lv_obj_t *music_icon_button(lv_obj_t *parent, const char *icon,
                                   int source_size, int32_t x, int32_t y,
                                   int32_t size, lv_event_cb_t callback,
                                   intptr_t user_data)
{
    lv_obj_t *button = make_box(parent, x, y, size, size,
                                0xffffff, size / 2, LV_OPA_90);

    lv_obj_set_style_border_width(button, sx(3), 0);
    lv_obj_set_style_border_color(button, lv_color_hex(0x2f80ed), 0);
    lv_obj_set_style_border_opa(button, LV_OPA_COVER, 0);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    if (icon) {
        make_asset_image(button, icon,
                         (size - size * 58 / 100) / 2,
                         (size - size * 58 / 100) / 2,
                         source_size, size * 58 / 100);
    }
    if (callback) {
        lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED,
                            (void *)user_data);
    }
    return button;
}

static void music_control_clicked(lv_event_t *event)
{
    int action = (int)(intptr_t)lv_event_get_user_data(event);

    if (action == 0) {
        music_toggle();
    } else {
        bool autoplay = g_ui.music_state == UI_MUSIC_PLAYING ||
                        g_ui.music_state == UI_MUSIC_PAUSING ||
                        g_ui.music_state == UI_MUSIC_PREPARING;
        music_change_track(action, autoplay);
    }
    music_refresh_ui();
}

static void render_music_page(void);

static void music_open_subpage(lv_event_t *event)
{
    int page = (int)(intptr_t)lv_event_get_user_data(event);

    if (page < UI_MUSIC_PLAYER || page > UI_MUSIC_LIST) {
        return;
    }
    g_ui.music_page = page;
    render_music_page();
}

static void music_volume_clicked(lv_event_t *event)
{
    int delta = (int)(intptr_t)lv_event_get_user_data(event);

    music_set_volume(g_ui.music_volume + delta);
    music_refresh_ui();
}

static void music_track_clicked(lv_event_t *event)
{
    int track = (int)(intptr_t)lv_event_get_user_data(event);
    int direction;

    if (track < 0 || track >= UI_MUSIC_TRACK_COUNT) {
        return;
    }
    direction = track - g_ui.music_track;
    if (direction == 0) {
        if (g_ui.music_state != UI_MUSIC_PLAYING) {
            music_toggle();
        }
    } else {
        music_change_track(direction, true);
    }
    g_ui.music_page = UI_MUSIC_PLAYER;
    render_music_page();
}

static void create_music_player_page(void)
{
    lv_obj_t *album;
    lv_obj_t *button;

    make_label(g_ui.music_view,
               g_ui.font_title ? "音乐播放" : "MUSIC PLAYER",
               g_ui.font_title, 0xffffff,
               66, 20, 300, LV_TEXT_ALIGN_CENTER);
    album = make_box(g_ui.music_view, 36, 90, 360, 146,
                     0xffffff, 28, LV_OPA_90);
    make_asset_image(album, UI_MUSIC_LOGO, 20, 17, 144, 112);
    g_ui.music_title_label = make_label(album, "", g_ui.font_body,
        0x162549, 151, 27, 185, LV_TEXT_ALIGN_LEFT);
    g_ui.music_artist_label = make_label(album, "", g_ui.font_small,
        0x65718a, 151, 78, 185, LV_TEXT_ALIGN_LEFT);

    music_icon_button(g_ui.music_view, UI_MUSIC_ICON_PREV, 63,
                      72, 270, 66, music_control_clicked, -1);
    button = music_icon_button(g_ui.music_view, UI_MUSIC_ICON_PLAY, 72,
                               166, 252, 100,
                               music_control_clicked, 0);
    g_ui.music_play_icon = lv_obj_get_child(button, 0);
    music_icon_button(g_ui.music_view, UI_MUSIC_ICON_NEXT, 63,
                      294, 270, 66, music_control_clicked, 1);

    g_ui.music_progress_bar = lv_bar_create(g_ui.music_view);
    lv_obj_set_pos(g_ui.music_progress_bar, sx(54), sx(375));
    lv_obj_set_size(g_ui.music_progress_bar, sx(324), sx(8));
    lv_bar_set_range(g_ui.music_progress_bar, 0, 1000);
    lv_obj_set_style_bg_color(g_ui.music_progress_bar,
                              lv_color_hex(0x7180a0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_ui.music_progress_bar,
                              lv_color_hex(0xffffff), LV_PART_INDICATOR);
    g_ui.music_progress_label = make_label(g_ui.music_view, "0:00 / 0:00",
        g_ui.font_small, 0xdbe2f1, 106, 392, 220, LV_TEXT_ALIGN_CENTER);
    g_ui.music_status_label = make_label(g_ui.music_view, "",
        g_ui.font_small, 0xffffff, 106, 423, 220, LV_TEXT_ALIGN_CENTER);

    music_icon_button(g_ui.music_view, UI_MUSIC_ICON_VOLUME, 64,
                      45, 423, 58, music_open_subpage, UI_MUSIC_VOLUME);
    music_icon_button(g_ui.music_view, UI_MUSIC_ICON_LIST, 64,
                      329, 423, 58, music_open_subpage, UI_MUSIC_LIST);
}

static void create_music_volume_page(void)
{
    lv_obj_t *card;
    lv_obj_t *back;

    make_label(g_ui.music_view,
               g_ui.font_title ? "音量调节" : "VOLUME",
               g_ui.font_title, 0xffffff,
               66, 20, 300, LV_TEXT_ALIGN_CENTER);
    card = make_box(g_ui.music_view, 40, 96, 352, 276,
                    0xffffff, 34, LV_OPA_90);
    make_asset_image(card, UI_MUSIC_ICON_VOLUME, 140, 18, 64, 72);
    g_ui.music_volume_label = make_label(card, "60%", g_ui.font_title,
        0x15264c, 76, 83, 200, LV_TEXT_ALIGN_CENTER);

    g_ui.music_volume_bar = lv_bar_create(card);
    lv_obj_set_pos(g_ui.music_volume_bar, sx(42), sx(163));
    lv_obj_set_size(g_ui.music_volume_bar, sx(268), sx(13));
    lv_bar_set_range(g_ui.music_volume_bar, 0, 100);
    lv_obj_set_style_bg_color(g_ui.music_volume_bar,
                              lv_color_hex(0xd4dbe7), LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_ui.music_volume_bar,
                              lv_color_hex(0x2f80ed), LV_PART_INDICATOR);
    music_icon_button(card, UI_MUSIC_ICON_MINUS, 120,
                      51, 199, 58, music_volume_clicked, -10);
    music_icon_button(card, UI_MUSIC_ICON_PLUS, 120,
                      243, 199, 58, music_volume_clicked, 10);

    back = make_box(g_ui.music_view, 116, 405, 200, 62,
                    0xffffff, 31, LV_OPA_90);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    make_asset_image(back, UI_MUSIC_ICON_CANCEL, 18, 13, 59, 36);
    make_label(back, g_ui.font_body ? "返回播放" : "BACK",
               g_ui.font_body, 0x162549,
               67, 13, 112, LV_TEXT_ALIGN_CENTER);
    lv_obj_add_event_cb(back, music_open_subpage, LV_EVENT_CLICKED,
                        (void *)(intptr_t)UI_MUSIC_PLAYER);
}

static void create_music_list_page(void)
{
    int index;

    make_label(g_ui.music_view,
               g_ui.font_title ? "播放列表" : "PLAYLIST",
               g_ui.font_title, 0xffffff,
               66, 20, 300, LV_TEXT_ALIGN_CENTER);
    for (index = 0; index < UI_MUSIC_TRACK_COUNT; index++) {
        lv_obj_t *row = make_box(g_ui.music_view, 30, 102 + index * 105,
                                 372, 88, 0xffffff, 25, LV_OPA_90);

        lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
        make_asset_image(row, g_music_badges[index],
                         18, 18, 128, 52);
        make_label(row, g_music_tracks[index].name, g_ui.font_body,
                   0x162549, 91, 10, 230, LV_TEXT_ALIGN_LEFT);
        make_label(row, g_music_tracks[index].artist, g_ui.font_small,
                   0x65718a, 91, 49, 230, LV_TEXT_ALIGN_LEFT);
        lv_obj_add_event_cb(row, music_track_clicked, LV_EVENT_CLICKED,
                            (void *)(intptr_t)index);
    }
    make_label(g_ui.music_view,
               g_ui.font_small ? "点击歌曲播放 · 上下滑返回" :
                                 "TAP A TRACK · SWIPE TO RETURN",
               g_ui.font_small, 0xdbe2f1,
               51, 440, 330, LV_TEXT_ALIGN_CENTER);
}

static void render_music_page(void)
{
    if (g_ui.music_view && lv_obj_is_valid(g_ui.music_view)) {
        lv_obj_delete(g_ui.music_view);
    }

    g_ui.music_view = lv_obj_create(g_ui.overlay);
    lv_obj_remove_style_all(g_ui.music_view);
    lv_obj_set_size(g_ui.music_view, g_ui.width, g_ui.height);
    lv_obj_set_pos(g_ui.music_view, 0, 0);
    lv_obj_clear_flag(g_ui.music_view, LV_OBJ_FLAG_CLICKABLE |
                                       LV_OBJ_FLAG_SCROLLABLE);
    g_ui.music_title_label = NULL;
    g_ui.music_artist_label = NULL;
    g_ui.music_status_label = NULL;
    g_ui.music_progress_label = NULL;
    g_ui.music_progress_bar = NULL;
    g_ui.music_play_icon = NULL;
    g_ui.music_volume_label = NULL;
    g_ui.music_volume_bar = NULL;

    if (g_ui.music_page == UI_MUSIC_VOLUME) {
        create_music_volume_page();
    } else if (g_ui.music_page == UI_MUSIC_LIST) {
        create_music_list_page();
    } else {
        create_music_player_page();
    }
    music_refresh_ui();
}

static void create_music_page(void)
{
    render_music_page();
}

static void invalidate_viewport(void)
{
    if (g_ui.viewport && lv_obj_is_valid(g_ui.viewport)) {
        lv_obj_invalidate(g_ui.viewport);
    }
}

static void stop_overlay_animations(void)
{
    lv_obj_t *sport_root = openvela_ui_sport_root(g_ui.sport);

    if (g_ui.health_view && lv_obj_is_valid(g_ui.health_view)) {
        lv_anim_delete(g_ui.health_view, NULL);
    }
    if (g_ui.music_view && lv_obj_is_valid(g_ui.music_view)) {
        lv_anim_delete(g_ui.music_view, NULL);
    }
    if (g_ui.weather_view && lv_obj_is_valid(g_ui.weather_view)) {
        lv_anim_delete(g_ui.weather_view, NULL);
    }
    if (sport_root && lv_obj_is_valid(sport_root)) {
        lv_anim_delete(sport_root, NULL);
    }
    if (g_ui.overlay && lv_obj_is_valid(g_ui.overlay)) {
        lv_anim_delete(g_ui.overlay, NULL);
    }
}

static void overlay_shown_cb(lv_anim_t *animation)
{
    lv_obj_t *target = animation->var;

    /* The R528 display path uses a single direct-render framebuffer before
     * hardware rotation.  Finish the transition with one complete repaint so
     * no intermediate animation frame remains in the rotated output buffer. */
    if (target && lv_obj_is_valid(target)) {
        lv_obj_set_y(target, 0);
    }
    if (target == g_ui.weather_view &&
        g_ui.overlay_page == UI_OVERLAY_WEATHER &&
        g_ui.weather_transitioning) {
        /* Build the image-heavy weather tree only after the full-screen
         * transition has stopped.  This keeps PNG decoding and framebuffer
         * updates out of the moving-frame critical path on R528. */
        g_ui.weather_transitioning = false;
        render_weather_page();
    }
    invalidate_viewport();
}

static void sport_overlay_shown_async(void *argument)
{
    LV_UNUSED(argument);
    if (g_ui.overlay_page != UI_OVERLAY_SPORT ||
        !g_ui.sport_transitioning || !g_ui.sport ||
        !g_ui.overlay || !lv_obj_is_valid(g_ui.overlay)) {
        return;
    }

    g_ui.sport_transitioning = false;
    openvela_ui_sport_shown(g_ui.sport);
}

static void overlay_deleted_cb(lv_anim_t *animation)
{
    bool was_weather = g_ui.overlay_page == UI_OVERLAY_WEATHER;

    LV_UNUSED(animation);

    stop_health_measurement();
    if (g_ui.sport) {
        openvela_ui_sport_destroy(g_ui.sport);
        g_ui.sport = NULL;
    }
    if (g_ui.overlay && lv_obj_is_valid(g_ui.overlay)) {
        lv_obj_delete(g_ui.overlay);
    }
    g_ui.overlay = NULL;
    g_ui.overlay_time_label = NULL;
    g_ui.health_view = NULL;
    g_ui.music_view = NULL;
    g_ui.weather_view = NULL;
    weather_reset_page_objects();
    g_ui.weather_switch_pending = false;
    g_ui.weather_transitioning = false;
    g_ui.sport_transitioning = false;
    g_ui.sport_home_visible = false;
    g_ui.overlay_page = UI_OVERLAY_NONE;
    if (was_weather) {
        weather_request_city(0);
    }
    apply_action(g_ui.selected_action);
    invalidate_viewport();
}

static void sport_overlay_deleted_async(void *argument)
{
    LV_UNUSED(argument);
    if (g_ui.overlay_page == UI_OVERLAY_SPORT &&
        g_ui.sport_transitioning) {
        overlay_deleted_cb(NULL);
    }
}

static void hide_overlay(void)
{
    lv_anim_t animation;
    lv_obj_t *target;

    if (!g_ui.overlay || !lv_obj_is_valid(g_ui.overlay)) {
        stop_health_measurement();
        if (g_ui.sport) {
            openvela_ui_sport_destroy(g_ui.sport);
            g_ui.sport = NULL;
        }
        if (g_ui.overlay_page == UI_OVERLAY_WEATHER) {
            weather_request_city(0);
            weather_reset_page_objects();
        }
        g_ui.weather_transitioning = false;
        g_ui.sport_transitioning = false;
        g_ui.sport_home_visible = false;
        g_ui.overlay_page = UI_OVERLAY_NONE;
        apply_action(g_ui.selected_action);
        return;
    }

    if (g_ui.overlay_page == UI_OVERLAY_HEALTH &&
        g_ui.health_view && lv_obj_is_valid(g_ui.health_view)) {
        target = g_ui.health_view;
    } else if (g_ui.overlay_page == UI_OVERLAY_MUSIC &&
               g_ui.music_view && lv_obj_is_valid(g_ui.music_view)) {
        target = g_ui.music_view;
    } else if (g_ui.overlay_page == UI_OVERLAY_WEATHER &&
               g_ui.weather_view && lv_obj_is_valid(g_ui.weather_view)) {
        target = g_ui.weather_view;
    } else {
        target = g_ui.overlay;
    }
    stop_overlay_animations();

    if (g_ui.overlay_page == UI_OVERLAY_SPORT && g_ui.sport) {
        g_ui.sport_transitioning = true;
        g_ui.sport_home_visible = false;
        openvela_ui_sport_hidden(g_ui.sport);
        lv_obj_set_style_bg_color(g_ui.overlay,
                                  lv_color_hex(0x07122f), 0);
        lv_obj_set_style_bg_opa(g_ui.overlay, LV_OPA_COVER, 0);
        /* The R528 hardware-rotation driver turns even a tiny invalidation
         * into a complete framebuffer rotation.  Do not drive a 170 ms
         * full-screen exit animation; delete behind one opaque frame on the
         * next LVGL turn instead. */
        if (lv_async_call(sport_overlay_deleted_async, NULL) !=
            LV_RESULT_OK) {
            overlay_deleted_cb(NULL);
        }
        return;
    }

    if (g_ui.overlay_page == UI_OVERLAY_WEATHER &&
        target == g_ui.weather_view) {
        /* Do not animate a complex transparent image tree.  Retain only an
         * opaque transition surface until the exit animation completes. */
        g_ui.weather_transitioning = true;
        g_ui.weather_switch_pending = false;
        g_ui.touch_tracking = false;
        weather_reset_page_objects();
        lv_obj_clean(target);
        lv_obj_set_style_bg_color(target, lv_color_hex(0x07122f), 0);
        lv_obj_set_style_bg_opa(target, LV_OPA_COVER, 0);
    }

    lv_anim_init(&animation);
    lv_anim_set_var(&animation, target);
    lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&animation, 0, g_ui.height);
    lv_anim_set_duration(&animation, 170);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_in);
    lv_anim_set_completed_cb(&animation, overlay_deleted_cb);
    lv_anim_start(&animation);
}

static void sport_show_special_home(void)
{
    if (!g_ui.sport ||
        !openvela_ui_sport_is_special_active(g_ui.sport) ||
        !g_ui.overlay || !lv_obj_is_valid(g_ui.overlay)) {
        return;
    }

    g_ui.sport_home_visible = true;
    openvela_ui_sport_hidden(g_ui.sport);
    lv_obj_set_style_bg_opa(g_ui.overlay, LV_OPA_TRANSP, 0);
    lv_tileview_set_tile(g_ui.tileview, g_ui.tile[UI_TILE_FIRST_REAL],
                         LV_ANIM_OFF);
    g_ui.current_page = UI_PAGE_HOME;
    update_page_dots();
    apply_action(g_ui.selected_action);
    invalidate_viewport();
}

static void sport_leave_special_home(openvela_ui_sport_page_t page)
{
    lv_obj_t *sport_root;

    if (!g_ui.sport || !g_ui.sport_home_visible ||
        !g_ui.overlay || !lv_obj_is_valid(g_ui.overlay)) {
        return;
    }

    if (g_ui.cat_anim) {
        lv_anim_delete(g_ui.cat_anim, NULL);
    }
    g_ui.sport_home_visible = false;
    lv_obj_set_style_bg_color(g_ui.overlay, lv_color_hex(0x07122f), 0);
    lv_obj_set_style_bg_opa(g_ui.overlay, LV_OPA_COVER, 0);
    sport_root = openvela_ui_sport_root(g_ui.sport);
    if (sport_root && lv_obj_is_valid(sport_root)) {
        lv_obj_clear_flag(sport_root, LV_OBJ_FLAG_HIDDEN);
    }
    openvela_ui_sport_shown(g_ui.sport);
    openvela_ui_sport_show_page(g_ui.sport, page);
    invalidate_viewport();
}

static void sport_event_callback(openvela_ui_sport_t *sport,
                                 openvela_ui_sport_event_t event,
                                 void *user_data)
{
    LV_UNUSED(user_data);
    if (!sport || sport != g_ui.sport ||
        g_ui.overlay_page != UI_OVERLAY_SPORT) {
        return;
    }

    if (event == OPENVELA_UI_SPORT_EVENT_SPECIAL_ENDED) {
        lv_tileview_set_tile(g_ui.tileview,
                             g_ui.tile[UI_TILE_FIRST_REAL], LV_ANIM_OFF);
        g_ui.current_page = UI_PAGE_HOME;
        update_page_dots();
        hide_overlay();
    } else if (event == OPENVELA_UI_SPORT_EVENT_CLOSE_REQUESTED) {
        hide_overlay();
    } else if (event == OPENVELA_UI_SPORT_EVENT_HOME_REQUESTED) {
        sport_show_special_home();
    }
}

static void overlay_touch_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_indev_t *indev = lv_indev_active();
    lv_point_t point;

    if (!indev) {
        return;
    }

    if (g_ui.overlay_page == UI_OVERLAY_WEATHER &&
        (g_ui.weather_transitioning || g_ui.weather_modal_active)) {
        if (code == LV_EVENT_RELEASED) {
            g_ui.touch_tracking = false;
        }
        return;
    }
    if (g_ui.overlay_page == UI_OVERLAY_SPORT &&
        (g_ui.sport_transitioning ||
         openvela_ui_sport_is_transitioning(g_ui.sport))) {
        if (code == LV_EVENT_RELEASED) {
            g_ui.touch_tracking = false;
        }
        return;
    }

    if (code == LV_EVENT_PRESSED) {
        lv_indev_get_point(indev, &g_ui.touch_start);
        g_ui.touch_tracking = true;
    } else if (code == LV_EVENT_RELEASED && g_ui.touch_tracking) {
        int32_t delta_x;
        int32_t delta_y;

        lv_indev_get_point(indev, &point);
        g_ui.touch_tracking = false;
        delta_x = point.x - g_ui.touch_start.x;
        delta_y = point.y - g_ui.touch_start.y;
        if (g_ui.overlay_page == UI_OVERLAY_SPORT) {
            if (g_ui.sport_home_visible) {
                bool horizontal =
                    LV_ABS(delta_x) > sx(UI_SWIPE_THRESHOLD) &&
                    LV_ABS(delta_x) * 100 > LV_ABS(delta_y) * 115;

                if (horizontal) {
                    sport_leave_special_home(delta_x < 0 ?
                        OPENVELA_UI_SPORT_PAGE_STEPS :
                        OPENVELA_UI_SPORT_PAGE_EXIT);
                }
            } else if (g_ui.sport) {
                openvela_ui_sport_gesture(g_ui.sport,
                                           delta_x, delta_y);
            } else if (LV_ABS(delta_y) > sx(UI_SWIPE_THRESHOLD) &&
                       LV_ABS(delta_y) * 100 > LV_ABS(delta_x) * 115) {
                hide_overlay();
            }
            return;
        }
        if (g_ui.overlay_page == UI_OVERLAY_WEATHER) {
            bool vertical = LV_ABS(delta_y) > sx(UI_SWIPE_THRESHOLD) &&
                            LV_ABS(delta_y) * 100 >
                            LV_ABS(delta_x) * 115;
            bool horizontal = LV_ABS(delta_x) > sx(UI_SWIPE_THRESHOLD) &&
                              LV_ABS(delta_x) * 100 >
                              LV_ABS(delta_y) * 115;

            if (g_ui.weather_stage == UI_WEATHER_CITIES) {
                if (vertical && delta_y < 0) {
                    hide_overlay();
                } else if (horizontal) {
                    weather_queue_city_page(g_ui.weather_city_page +
                                            (delta_x < 0 ? 1 : -1));
                }
            } else if (vertical) {
                weather_queue_switch(UI_WEATHER_SWITCH_CITIES);
            } else if (horizontal) {
                weather_queue_switch(delta_x < 0 ?
                    UI_WEATHER_SWITCH_FORECAST : UI_WEATHER_SWITCH_NOW);
            }
            return;
        }
        if (g_ui.overlay_page == UI_OVERLAY_HEALTH) {
            if (g_ui.health_measuring) {
                return;
            }
            if (LV_ABS(delta_y) > sx(UI_SWIPE_THRESHOLD) &&
                LV_ABS(delta_y) * 100 > LV_ABS(delta_x) * 115) {
                hide_overlay();
            } else if (LV_ABS(delta_x) > sx(UI_SWIPE_THRESHOLD) &&
                       LV_ABS(delta_x) * 100 > LV_ABS(delta_y) * 115) {
                change_health_page(delta_x < 0 ? 1 : -1);
            } else if (LV_ABS(delta_x) < sx(16) &&
                       LV_ABS(delta_y) < sx(16)) {
                start_health_measurement();
            }
            return;
        }
        if (g_ui.overlay_page == UI_OVERLAY_MUSIC) {
            if (LV_ABS(delta_y) > sx(UI_SWIPE_THRESHOLD) &&
                LV_ABS(delta_y) * 100 > LV_ABS(delta_x) * 115) {
                if (g_ui.music_page == UI_MUSIC_PLAYER) {
                    hide_overlay();
                } else {
                    g_ui.music_page = UI_MUSIC_PLAYER;
                    render_music_page();
                }
            }
            return;
        }
        if (delta_y < -sx(UI_SWIPE_THRESHOLD) &&
            -delta_y * 100 > LV_ABS(delta_x) * 115) {
            if (g_ui.overlay_page == UI_OVERLAY_CUSTOMIZE) {
                hide_overlay();
            } else {
                lv_async_call(show_overlay_async,
                              (void *)(intptr_t)UI_OVERLAY_CUSTOMIZE);
            }
        }
    }
}

static void show_overlay(int page)
{
    lv_anim_t animation;
    lv_obj_t *animation_target;

    if (page == UI_OVERLAY_NONE) {
        hide_overlay();
        return;
    }

    if (g_ui.overlay && lv_obj_is_valid(g_ui.overlay)) {
        stop_health_measurement();
        stop_overlay_animations();
        if (g_ui.sport) {
            openvela_ui_sport_destroy(g_ui.sport);
            g_ui.sport = NULL;
        }
        lv_obj_delete(g_ui.overlay);
        g_ui.overlay = NULL;
        g_ui.health_view = NULL;
        g_ui.music_view = NULL;
        g_ui.weather_view = NULL;
        weather_reset_page_objects();
        g_ui.weather_switch_pending = false;
        g_ui.weather_transitioning = false;
        g_ui.sport_transitioning = false;
        g_ui.sport_home_visible = false;
    }
    if (g_ui.cat_anim) {
        lv_anim_delete(g_ui.cat_anim, NULL);
    }

    g_ui.overlay_page = page;
    g_ui.overlay = lv_obj_create(g_ui.viewport);
    lv_obj_remove_style_all(g_ui.overlay);
    lv_obj_set_size(g_ui.overlay, g_ui.width, g_ui.height);
    lv_obj_set_pos(g_ui.overlay, 0,
                   (page == UI_OVERLAY_HEALTH ||
                    page == UI_OVERLAY_MUSIC ||
                    page == UI_OVERLAY_WEATHER ||
                    page == UI_OVERLAY_SPORT) ? 0 : g_ui.height);
    lv_obj_set_style_bg_color(g_ui.overlay,
        lv_color_hex((page == UI_OVERLAY_HEALTH ||
                      page == UI_OVERLAY_MUSIC ||
                      page == UI_OVERLAY_WEATHER ||
                      page == UI_OVERLAY_SPORT) ? 0x07122f : 0xfaf8f4), 0);
    /* A moving full-screen translucent fill causes read/write contention in
     * the board's single-buffer G2D + hardware-rotation path.  Weather uses
     * its opaque child as the transition surface; other overlays keep the
     * parent opaque. */
    lv_obj_set_style_bg_opa(g_ui.overlay,
        page == UI_OVERLAY_WEATHER ? LV_OPA_TRANSP : LV_OPA_COVER, 0);
    lv_obj_add_flag(g_ui.overlay, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(g_ui.overlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(g_ui.overlay, overlay_touch_cb, LV_EVENT_ALL, NULL);

    if (page == UI_OVERLAY_CUSTOMIZE) {
        create_customize_page();
    } else if (page == UI_OVERLAY_BACKGROUNDS) {
        create_backgrounds_page();
    } else if (page == UI_OVERLAY_ACTIONS) {
        create_actions_page();
    } else if (page == UI_OVERLAY_HEALTH) {
        g_ui.health_page = UI_HEALTH_HEART_RATE;
        create_health_page();
        lv_obj_set_y(g_ui.health_view, g_ui.height);
    } else if (page == UI_OVERLAY_MUSIC) {
        g_ui.music_page = UI_MUSIC_PLAYER;
        create_music_page();
        lv_obj_set_y(g_ui.music_view, g_ui.height);
    } else if (page == UI_OVERLAY_WEATHER) {
        g_ui.weather_stage = UI_WEATHER_CITIES;
        g_ui.weather_detail_page = UI_WEATHER_NOW;
        g_ui.weather_city_page = 0;
        g_ui.weather_switch_pending = false;
        g_ui.weather_transitioning = true;
        create_weather_page();
        lv_obj_set_y(g_ui.weather_view, g_ui.height);
    } else if (page == UI_OVERLAY_SPORT) {
        openvela_ui_sport_fonts_t fonts = {
            .small = g_ui.font_small,
            .body = g_ui.font_body,
            .title = g_ui.font_title,
            .clock = g_ui.font_clock,
        };
        weather_selected_preview_path(g_ui.preview_path,
                                      sizeof(g_ui.preview_path));
        g_ui.sport = openvela_ui_sport_create(
            g_ui.overlay, g_ui.width, g_ui.height, g_ui.scale_1000,
            &fonts, g_ui.preview_path);
        g_ui.sport_transitioning = g_ui.sport != NULL;
        g_ui.sport_home_visible = false;
        if (g_ui.sport) {
            openvela_ui_sport_set_event_cb(g_ui.sport,
                                            sport_event_callback, NULL);
        } else {
            make_label(g_ui.overlay,
                       g_ui.font_body ? "运动页面加载失败" :
                                        "SPORT UI FAILED",
                       g_ui.font_body, 0xffffff,
                       66, 226, 300, LV_TEXT_ALIGN_CENTER);
        }
    }

    if (page == UI_OVERLAY_SPORT) {
        /* Build the sport tree hidden, keep the outer surface stationary,
         * then reveal it once.  This preserves the complete visual layout
         * without the burst of full-frame FBIO_UPDATE calls that corrupts
         * the board's rotating framebuffer path. */
        if (g_ui.sport &&
            lv_async_call(sport_overlay_shown_async, NULL) != LV_RESULT_OK) {
            g_ui.sport_transitioning = false;
            openvela_ui_sport_shown(g_ui.sport);
        }
        return;
    }

    if (page == UI_OVERLAY_HEALTH) {
        animation_target = g_ui.health_view;
    } else if (page == UI_OVERLAY_MUSIC) {
        animation_target = g_ui.music_view;
    } else if (page == UI_OVERLAY_WEATHER) {
        animation_target = g_ui.weather_view;
    } else {
        animation_target = g_ui.overlay;
    }

    lv_anim_init(&animation);
    lv_anim_set_var(&animation, animation_target);
    lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&animation, g_ui.height, 0);
    lv_anim_set_duration(&animation, 190);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
    lv_anim_set_completed_cb(&animation, overlay_shown_cb);
    lv_anim_start(&animation);
}

static void update_page_dots(void)
{
    int i;
    for (i = 0; i < UI_PAGE_COUNT; i++) {
        bool active = i == g_ui.current_page;
        lv_obj_set_width(g_ui.dots[i], sx(active ? 20 : 7));
        lv_obj_set_style_bg_opa(g_ui.dots[i], active ? LV_OPA_COVER : LV_OPA_40, 0);
    }
}

static void create_page_dots(void)
{
    int i;
    int32_t start = 158;
    for (i = 0; i < UI_PAGE_COUNT; i++) {
        g_ui.dots[i] = make_box(g_ui.viewport, start + i * 23, 490,
                                i == 0 ? 20 : 7, 7, 0xffffff, 4,
                                i == 0 ? LV_OPA_COVER : LV_OPA_40);
        register_theme_fill(g_ui.dots[i]);
    }
}

static int tile_index_from_obj(lv_obj_t *tile)
{
    int i;

    for (i = 0; i < UI_TILE_COUNT; i++) {
        if (g_ui.tile[i] == tile) {
            return i;
        }
    }

    return -1;
}

static void home_touch_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_indev_t *indev = lv_indev_active();
    lv_point_t point;

    if (!indev || g_ui.overlay_page != UI_OVERLAY_NONE) {
        return;
    }

    if (code == LV_EVENT_PRESSED) {
        lv_indev_get_point(indev, &g_ui.touch_start);
        g_ui.touch_tracking = true;
    } else if (code == LV_EVENT_RELEASED && g_ui.touch_tracking) {
        int32_t delta_x;
        int32_t delta_y;

        lv_indev_get_point(indev, &point);
        g_ui.touch_tracking = false;
        delta_x = point.x - g_ui.touch_start.x;
        delta_y = point.y - g_ui.touch_start.y;
        if (g_ui.current_page == UI_PAGE_HOME &&
            delta_y > sx(UI_SWIPE_THRESHOLD) &&
            delta_y * 100 > LV_ABS(delta_x) * 120) {
            show_overlay(UI_OVERLAY_CUSTOMIZE);
        } else if (g_ui.current_page == UI_PAGE_WEATHER &&
                   delta_y > sx(UI_SWIPE_THRESHOLD) &&
                   delta_y * 100 > LV_ABS(delta_x) * 120) {
            show_overlay(UI_OVERLAY_WEATHER);
        } else if (g_ui.current_page == UI_PAGE_SPORT &&
                   LV_ABS(delta_y) > sx(UI_SWIPE_THRESHOLD) &&
                   LV_ABS(delta_y) * 100 > LV_ABS(delta_x) * 120) {
            show_overlay(UI_OVERLAY_SPORT);
        } else if (g_ui.current_page == UI_PAGE_HEALTH &&
                   LV_ABS(delta_y) > sx(UI_SWIPE_THRESHOLD) &&
                   LV_ABS(delta_y) * 100 > LV_ABS(delta_x) * 120) {
            show_overlay(UI_OVERLAY_HEALTH);
        } else if (g_ui.current_page == UI_PAGE_MUSIC &&
                   LV_ABS(delta_y) > sx(UI_SWIPE_THRESHOLD) &&
                   LV_ABS(delta_y) * 100 > LV_ABS(delta_x) * 120) {
            show_overlay(UI_OVERLAY_MUSIC);
        }
    }
}

static void tileview_changed_cb(lv_event_t *event)
{
    lv_obj_t *tileview = lv_event_get_current_target(event);
    lv_obj_t *active = lv_tileview_get_tile_active(tileview);
    int tile_index = tile_index_from_obj(active);

    if (tile_index < 0) {
        return;
    }

    if (tile_index == 0) {
        g_ui.current_page = UI_PAGE_NOTIFICATIONS;
        update_page_dots();
        lv_tileview_set_tile(tileview, g_ui.tile[UI_TILE_LAST_REAL],
                             LV_ANIM_OFF);
    } else if (tile_index == UI_TILE_COUNT - 1) {
        g_ui.current_page = UI_PAGE_HOME;
        update_page_dots();
        lv_tileview_set_tile(tileview, g_ui.tile[UI_TILE_FIRST_REAL],
                             LV_ANIM_OFF);
    } else {
        g_ui.current_page = g_tile_page[tile_index];
        update_page_dots();
    }
}

static void init_fonts(void)
{
    if (access(UI_FONT, R_OK) != 0) {
        return;
    }

    g_ui.font_small = lv_freetype_font_create(UI_FONT,
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP, (uint32_t)sx(17),
        LV_FREETYPE_FONT_STYLE_NORMAL);
    g_ui.font_body = lv_freetype_font_create(UI_FONT,
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP, (uint32_t)sx(27),
        LV_FREETYPE_FONT_STYLE_NORMAL);
    g_ui.font_title = lv_freetype_font_create(UI_FONT,
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP, (uint32_t)sx(52),
        LV_FREETYPE_FONT_STYLE_NORMAL);
    g_ui.font_clock = lv_freetype_font_create(UI_FONT,
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP, (uint32_t)sx(68),
        LV_FREETYPE_FONT_STYLE_NORMAL);
}

void openvela_ui_create(void)
{
    lv_obj_t *screen = lv_screen_active();
    int32_t screen_width = lv_display_get_horizontal_resolution(NULL);
    int32_t screen_height = lv_display_get_vertical_resolution(NULL);
    int32_t viewport_width = LV_MIN(screen_width,
        (screen_height * UI_DESIGN_WIDTH) / UI_DESIGN_HEIGHT);
    int systolic;
    int diastolic;
    int pulse;
    int home_slot = 0;
    int i;

    lv_memset(&g_ui, 0, sizeof(g_ui));
    /* The home background and one complete 34-frame action need about
     * 3.7 MiB after PNG decoding.  The board default (1 MiB) otherwise
     * reloads and inflates a frame every 100 ms forever, eventually causing
     * libpng/zlib failures on the NAND target.  Resize before the first image
     * is created; the cache still allocates entries lazily. */
    lv_image_cache_resize(UI_IMAGE_CACHE_SIZE, false);
    srand((unsigned int)(time(NULL) ^ (uintptr_t)&g_ui));
    g_ui.width = viewport_width;
    g_ui.height = (viewport_width * UI_DESIGN_HEIGHT) / UI_DESIGN_WIDTH;
    g_ui.scale_1000 = (viewport_width * 1000) / UI_DESIGN_WIDTH;
    g_ui.current_page = UI_PAGE_HOME;
    g_ui.overlay_page = UI_OVERLAY_NONE;
    g_ui.selected_background = UI_BACKGROUND_COUNT - 1;
    g_ui.selected_action = 0;
    g_ui.music_state = UI_MUSIC_IDLE;
    g_ui.music_volume = 60;
    g_ui.weather_stage = UI_WEATHER_CITIES;
    g_ui.weather_detail_page = UI_WEATHER_NOW;
    g_ui.weather_selected_city = 0;
    g_ui.weather_request_city = -1;
    g_ui.weather_delete_city = -1;
    weather_load_custom_cities();
    g_ui.heart_rate = realistic_heart_rate();
    realistic_blood_pressure(&systolic, &diastolic, &pulse);
    g_ui.systolic = systolic;
    g_ui.diastolic = diastolic;
    g_ui.pulse = pulse;
    g_ui.assets_ready = access(g_background_paths[g_ui.selected_background],
                               R_OK) == 0 &&
                        access(UI_DATA_ROOT "/cat/frame-01.png", R_OK) == 0;

    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    init_fonts();

    g_ui.viewport = lv_obj_create(screen);
    lv_obj_remove_style_all(g_ui.viewport);
    lv_obj_set_size(g_ui.viewport, g_ui.width, g_ui.height);
    lv_obj_center(g_ui.viewport);
    lv_obj_set_style_bg_color(g_ui.viewport, lv_color_hex(0x07163d), 0);
    lv_obj_set_style_bg_opa(g_ui.viewport, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(g_ui.viewport, sx(42), 0);
    lv_obj_set_style_clip_corner(g_ui.viewport, true, 0);
    lv_obj_clear_flag(g_ui.viewport, LV_OBJ_FLAG_SCROLLABLE);

    create_background(g_ui.viewport);

    g_ui.tileview = lv_tileview_create(g_ui.viewport);
    lv_obj_set_size(g_ui.tileview, g_ui.width, g_ui.height);
    lv_obj_set_pos(g_ui.tileview, 0, 0);
    lv_obj_set_style_bg_opa(g_ui.tileview, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(g_ui.tileview, 0, 0);
    lv_obj_set_style_pad_all(g_ui.tileview, 0, 0);
    lv_obj_set_scrollbar_mode(g_ui.tileview, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(g_ui.tileview, LV_OBJ_FLAG_SCROLL_ELASTIC);

    for (i = 0; i < UI_TILE_COUNT; i++) {
        lv_dir_t direction = LV_DIR_LEFT | LV_DIR_RIGHT;

        if (i == 0) {
            direction = LV_DIR_RIGHT;
        } else if (i == UI_TILE_COUNT - 1) {
            direction = LV_DIR_LEFT;
        }

        g_ui.tile[i] = lv_tileview_add_tile(g_ui.tileview, i, 0,
                                             direction);
        lv_obj_set_style_bg_opa(g_ui.tile[i], LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(g_ui.tile[i], 0, 0);
        lv_obj_set_style_pad_all(g_ui.tile[i], 0, 0);
        /* Keep the tile itself static while preserving horizontal scroll
         * chaining, so the parent tileview follows the finger directly. */
        lv_obj_clear_flag(g_ui.tile[i], LV_OBJ_FLAG_SCROLLABLE |
                          LV_OBJ_FLAG_SCROLL_ELASTIC);
        populate_tile(g_ui.tile[i], g_tile_page[i],
                      g_tile_page[i] == UI_PAGE_HOME ? home_slot++ : 0);
        if (g_tile_page[i] == UI_PAGE_HOME ||
            g_tile_page[i] == UI_PAGE_WEATHER ||
            g_tile_page[i] == UI_PAGE_SPORT ||
            g_tile_page[i] == UI_PAGE_HEALTH ||
            g_tile_page[i] == UI_PAGE_MUSIC) {
            lv_obj_add_event_cb(g_ui.tile[i], home_touch_cb,
                                LV_EVENT_ALL, NULL);
        }
    }

    lv_tileview_set_tile(g_ui.tileview, g_ui.tile[UI_TILE_FIRST_REAL],
                         LV_ANIM_OFF);

    create_cat_layer();
    create_page_dots();
    apply_theme_foreground();
    lv_obj_add_event_cb(g_ui.tileview, tileview_changed_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    g_ui.clock_timer = lv_timer_create(update_clock, 1000, NULL);
    /* This timer belongs to the player, not to the music page.  It remains
     * alive while the user leaves and re-enters the page, preserving playback
     * state and completing click-free fade transitions in the background. */
    g_ui.music_timer = lv_timer_create(music_timer_tick, 40, NULL);
    openvela_ui_weather_start();
    weather_request_city(0);
    g_ui.weather_timer = lv_timer_create(weather_timer_tick, 500, NULL);
    update_clock(NULL);
}

void openvela_ui_set_low_power(bool enabled)
{
    if (g_ui.low_power == enabled) {
        return;
    }

    g_ui.low_power = enabled;

    if (g_ui.clock_timer) {
        lv_timer_set_period(g_ui.clock_timer, enabled ? 60000 : 1000);
    }

    if (g_ui.cat_anim) {
        if (enabled) {
            lv_anim_delete(g_ui.cat_anim, NULL);
        } else {
            apply_action(g_ui.selected_action);
        }
    }

    if (g_ui.sport) {
        openvela_ui_sport_set_low_power(g_ui.sport, enabled);
    }

    if (!enabled) {
        update_clock(NULL);
        weather_timer_tick(NULL);
        music_refresh_ui();
    }
}
