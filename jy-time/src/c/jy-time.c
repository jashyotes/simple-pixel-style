#include <pebble.h>
#include <stdlib.h>
#include <string.h>

// JY Time watchface
// Pixel-style structure, emery target (Pebble Time 2, 200x228).
//
// Watch-side data: time, date, watch battery, Bluetooth, steps, HR.
// Companion-fed via AppMessage: next calendar event, phone battery %, weather,
// and configurable complication layout.

#define PERSIST_KEY_PHONE_BATTERY  100
#define PERSIST_KEY_WEATHER_CODE   101
#define PERSIST_KEY_TEMPERATURE    102
#define PERSIST_KEY_EVENT          103
#define PERSIST_KEY_RAIN_CHANCE    104
#define PERSIST_KEY_TOP_STEPS      105
#define PERSIST_KEY_FEELS_LIKE     106
#define PERSIST_KEY_HIGH_TEMP      107
#define PERSIST_KEY_WIND_SPEED     108
#define PERSIST_KEY_UV_INDEX       109
#define PERSIST_KEY_EVENT_DELTA    110
#define PERSIST_KEY_COMP_SLOT_1    111
#define PERSIST_KEY_COMP_SLOT_2    112
#define PERSIST_KEY_COMP_SLOT_3    113
#define PERSIST_KEY_TEMP_UNIT      114
#define PERSIST_KEY_VERBOSE_WEATHER 115
#define PERSIST_KEY_WEATHER_SUMMARY 116
#define PERSIST_KEY_VERBOSE_WEATHER_STYLE 117
#define PERSIST_KEY_LIGHT_MODE     118
#define PERSIST_KEY_INVERT_TOP_BAR 119
#define PERSIST_KEY_INVERT_DATE_BAR 120
#define PERSIST_KEY_INVERT_TIME    121
#define PERSIST_KEY_INVERT_WEATHER 122
#define PERSIST_KEY_INVERT_MEETING_BAR 123
// Persist key 124 is intentionally unused after the shake behavior selector
// replaced the old boolean gate.
#define PERSIST_KEY_FITNESS_RING_STEPS_ON 125
#define PERSIST_KEY_FITNESS_RING_ACTIVE_ON 126
#define PERSIST_KEY_FITNESS_RING_CALORIES_ON 127
#define PERSIST_KEY_FITNESS_TARGET_STEPS 128
#define PERSIST_KEY_FITNESS_TARGET_ACTIVE_MIN 129
#define PERSIST_KEY_FITNESS_TARGET_CALORIES 130
#define PERSIST_KEY_FITNESS_COLOR_STEPS 131
#define PERSIST_KEY_FITNESS_COLOR_ACTIVE 132
#define PERSIST_KEY_FITNESS_COLOR_CALORIES 133
#define PERSIST_KEY_FITNESS_OVERLAY_DURATION_S 134
#define PERSIST_KEY_MILITARY_TIME 135
#define PERSIST_KEY_SHAKE_BEHAVIOR 136
#define PERSIST_KEY_LOW_TEMP 137
#define PERSIST_KEY_SUNRISE_T 138
#define PERSIST_KEY_SUNSET_T 139
#define PERSIST_KEY_CALENDAR_EVENT_TITLE_2 140
#define PERSIST_KEY_CALENDAR_EVENT_DELTA_2 141
#define PERSIST_KEY_CALENDAR_EVENT_TITLE_3 142
#define PERSIST_KEY_CALENDAR_EVENT_DELTA_3 143
#define PERSIST_KEY_CALENDAR_EVENT_TITLE_4 144
#define PERSIST_KEY_CALENDAR_EVENT_DELTA_4 145
#define PERSIST_KEY_CALENDAR_EVENT_TITLE_5 146
#define PERSIST_KEY_CALENDAR_EVENT_DELTA_5 147
#define PERSIST_KEY_CALENDAR_SHAKE_EVENT_COUNT 148
#define PERSIST_KEY_DAY_EVENT_HOURS_BITMAP 149
#define PERSIST_KEY_DAY_EVENT_COUNT_TODAY 150
#define PERSIST_KEY_ALT_TZ_LABEL 151
#define PERSIST_KEY_ALT_TZ_OFFSET_MIN 152
#define PERSIST_KEY_REMOVE_LEADING_ZERO 153
#define PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_TOMORROW 154
#define PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_YESTERDAY 155
#define PERSIST_KEY_YOUR_DAY_WINDOW_MODE 156
#define PERSIST_KEY_YOUR_DAY_WINDOW_HOURS 157
#define PERSIST_KEY_YOUR_DAY_START_HOUR 158
#define PERSIST_KEY_YOUR_DAY_END_HOUR 159
#define PERSIST_KEY_COLOR_MODE 160
#define PERSIST_KEY_COLOR_SECTION_BG_TOP_BAR 161
#define PERSIST_KEY_COLOR_SECTION_FG_TOP_BAR 162
#define PERSIST_KEY_COLOR_SECTION_BG_DATE_BAR 163
#define PERSIST_KEY_COLOR_SECTION_FG_DATE_BAR 164
#define PERSIST_KEY_COLOR_SECTION_BG_TIME 165
#define PERSIST_KEY_COLOR_SECTION_FG_TIME 166
#define PERSIST_KEY_COLOR_SECTION_BG_WEATHER 167
#define PERSIST_KEY_COLOR_SECTION_FG_WEATHER 168
#define PERSIST_KEY_COLOR_SECTION_BG_MEETING_BAR 169
#define PERSIST_KEY_COLOR_SECTION_FG_MEETING_BAR 170

#define SCREEN_W 200
#define SCREEN_H 228
#define FITNESS_DEFAULT_TARGET_STEPS 10000
#define FITNESS_DEFAULT_TARGET_ACTIVE_MIN 30
#define FITNESS_DEFAULT_TARGET_CALORIES 500
#define FITNESS_DEFAULT_COLOR_STEPS 0x00FF00
#define FITNESS_DEFAULT_COLOR_ACTIVE 0x00AAFF
#define FITNESS_DEFAULT_COLOR_CALORIES 0xFF0000
#define DATE_FRAME_Y 36
#define TIME_FRAME_Y 69
#define TIME_FRAME_H 60
#define TIME_VISUAL_BOTTOM 111
#define VERBOSE_WEATHER_OFFSET_Y 16
#define VERBOSE_WEATHER_CENTER_Y 183
#define EVENT_SEPARATOR_Y 200
#define COMPLICATION_RADIUS 24
#define COMPLICATION_COUNT 3
#define COMPLICATION_CENTER_Y 163
#define WEATHER_ICON_SMALL_SIZE 18
#define WEATHER_ICON_LARGE_SIZE 32
#define QUIET_TIME_ICON_SIZE 18
#define QUIET_TIME_ICON_X 177

typedef enum {
  ComplicationTemperature = 0,
  ComplicationRainChance = 1,
  ComplicationHeartRate = 2,
  ComplicationSteps = 3,
  ComplicationWatchBattery = 4,
  ComplicationPhoneBattery = 5,
  ComplicationFeelsLike = 6,
  ComplicationHighTemp = 7,
  ComplicationWindSpeed = 8,
  ComplicationUvIndex = 9,
  ComplicationNextEvent = 10,
  ComplicationWeather = 11,
  ComplicationWeatherIcon = 12,
  ComplicationFitnessRings = 13,
  ComplicationWeatherCircle = 14,
  ComplicationBatteryRing = 15,
  ComplicationHighLowCombined = 16,
  ComplicationActiveMinutes = 17,
  ComplicationActiveCalories = 18,
  ComplicationSleepLastNight = 19,
  ComplicationDistanceToday = 20,
} ComplicationType;

typedef enum {
  ShakeBehaviorOff = 0,
  ShakeBehaviorFitnessRings = 1,
  ShakeBehaviorCalendarEvents = 2,
  ShakeBehaviorYourDay = 3,
  ShakeBehaviorDetailedWeather = 4,
  ShakeBehaviorAltTimezone = 5,
  ShakeBehaviorHeartRate = 6,
  ShakeBehaviorStepGraph = 7,
} ShakeBehavior;

typedef enum {
  WeatherIconSunny = 0,
  WeatherIconPartlyCloudy = 1,
  WeatherIconCloudy = 2,
  WeatherIconFog = 3,
  WeatherIconRain = 4,
  WeatherIconSnow = 5,
  WeatherIconStorm = 6,
  WeatherIconCount = 7,
} WeatherIconBitmap;

typedef enum {
  BitmapThemeDark = 0,
  BitmapThemeLight = 1,
  BitmapThemeCount = 2,
} BitmapTheme;

typedef enum {
  ColorSectionBase = 0,
  ColorSectionTopBar = 1,
  ColorSectionDateBar = 2,
  ColorSectionTime = 3,
  ColorSectionWeather = 4,
  ColorSectionMeetingBar = 5,
} ColorSection;

typedef enum {
  ColorModeBW = 0,
  ColorModeColor = 1,
} ColorMode;

static Window *s_window;
static Layer *s_face_layer;
static Layer *s_shake_overlay_layer;
static AppTimer *s_shake_overlay_timer;

static GFont s_font_top;
static GFont s_font_date;
static GFont s_font_time;
static GFont s_font_complication;
static GFont s_font_event;
static GBitmap *s_step_boot_bitmaps[BitmapThemeCount];
static GBitmap *s_w800_walking_bitmaps[BitmapThemeCount];
static GBitmap *s_quiet_time_mouse_bitmaps[BitmapThemeCount];
static GBitmap *s_w800_digit_bitmaps[BitmapThemeCount][10];
static GBitmap *s_weather_icon_small_bitmaps[BitmapThemeCount][WeatherIconCount];
static GBitmap *s_weather_icon_large_bitmaps[BitmapThemeCount][WeatherIconCount];

static char s_date_buf[32];
static char s_time_buf[8];
static char s_ampm_buf[3];
static char s_event_buf[80];
static char s_watch_buf[8];
static char s_watch_battery_buf[8];
static char s_phone_battery_buf[8];
static char s_steps_buf[8];
static char s_temp_buf[8];
static char s_rain_buf[8];
static char s_bpm_buf[12];
static char s_feels_like_buf[8];
static char s_high_temp_buf[8];
static char s_low_temp_buf[8];
static char s_wind_buf[8];
static char s_uv_buf[8];
static char s_event_delta_buf[8];
static char s_weather_summary_buf[32] = "CLOUDY";
static char s_calendar_event_titles[4][80];
static char s_calendar_event_deltas[4][16];
static char s_alt_tz_label[24] = "LONDON";

static uint8_t s_phone_battery_pct = 0;
static bool s_phone_battery_known = false;
static bool s_phone_connected = false;
static uint8_t s_watch_battery_pct = 0;

static int8_t s_temperature = 0;
static bool s_temperature_known = false;
static int8_t s_feels_like = 0;
static bool s_feels_like_known = false;
static int8_t s_high_temp = 0;
static bool s_high_temp_known = false;
static int8_t s_low_temp = 0;
static bool s_low_temp_known = false;
static time_t s_sunrise_t = 0;
static bool s_sunrise_known = false;
static time_t s_sunset_t = 0;
static bool s_sunset_known = false;
static uint8_t s_weather_code = 0;
static bool s_weather_known = false;
static uint8_t s_rain_chance = 0;
static bool s_rain_known = false;
static uint8_t s_wind_speed = 0;
static bool s_wind_known = false;
static uint8_t s_uv_index = 0;
static bool s_uv_known = false;
static bool s_w800_steps_top_enabled = true;
static bool s_temperature_unit_celsius = false;
static bool s_verbose_weather_enabled = false;
static bool s_verbose_weather_large = false;
static bool s_light_mode_enabled = false;
static bool s_invert_top_bar = false;
static bool s_invert_date_bar = false;
static bool s_invert_time = false;
static bool s_military_time_enabled = false;
static bool s_remove_leading_zero = false;
static bool s_invert_weather = false;
static bool s_invert_meeting_bar = false;
static ColorMode s_color_mode = ColorModeBW;
static ShakeBehavior s_shake_behavior = ShakeBehaviorOff;
static bool s_shake_tap_subscribed = false;
static bool s_shake_overlay_visible = false;
static bool s_fitness_ring_steps_on = true;
static bool s_fitness_ring_active_on = true;
static bool s_fitness_ring_calories_on = true;
static ColorSection s_draw_section = ColorSectionBase;
static int s_steps_count = 0;
static int s_fitness_steps_value = 0;
static int s_fitness_active_minutes_value = 0;
static int s_fitness_active_calories_value = 0;
static int s_fitness_distance_meters_value = 0;
static int s_fitness_target_steps = FITNESS_DEFAULT_TARGET_STEPS;
static int s_fitness_target_active_min = FITNESS_DEFAULT_TARGET_ACTIVE_MIN;
static int s_fitness_target_calories = FITNESS_DEFAULT_TARGET_CALORIES;
static int s_fitness_color_steps_hex = FITNESS_DEFAULT_COLOR_STEPS;
static int s_fitness_color_active_hex = FITNESS_DEFAULT_COLOR_ACTIVE;
static int s_fitness_color_calories_hex = FITNESS_DEFAULT_COLOR_CALORIES;
static int s_fitness_overlay_duration_ms = 5000;
static int s_calendar_shake_event_count = 3;
static int s_day_event_hours_bitmap = 0;
static int s_day_event_hours_bitmap_tomorrow = 0;
static int s_day_event_hours_bitmap_yesterday = 0;
static int s_day_event_count_today = 0;
static int s_alt_tz_offset_min = 0;
static int s_your_day_window_mode = 1;
static int s_your_day_window_hours = 10;
static int s_your_day_start_hour = 8;
static int s_your_day_end_hour = 17;
static int s_color_section_bg[ColorSectionMeetingBar + 1] = {
  0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
};
static int s_color_section_fg[ColorSectionMeetingBar + 1] = {
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
};
static time_t s_last_hr_sample_time = 0;
static ComplicationType s_complication_slots[COMPLICATION_COUNT] = {
  ComplicationWeather,
  ComplicationRainChance,
  ComplicationHeartRate,
};

static void apply_light_mode(bool enabled);
static void update_stats(void);
static void format_temp_with_degree(char *buf, size_t len, bool known, int value);

static void mark_face_dirty(void) {
  if (s_face_layer) {
    layer_mark_dirty(s_face_layer);
  }
}

static GColor theme_bg_color(void) {
  return s_light_mode_enabled ? GColorWhite : GColorBlack;
}

static GColor theme_fg_color(void) {
  return s_light_mode_enabled ? GColorBlack : GColorWhite;
}

static bool section_inverted(ColorSection section) {
  if (section == ColorSectionTopBar) {
    return s_invert_top_bar;
  } else if (section == ColorSectionDateBar) {
    return s_invert_date_bar;
  } else if (section == ColorSectionTime) {
    return s_invert_time;
  } else if (section == ColorSectionWeather) {
    return s_invert_weather;
  } else if (section == ColorSectionMeetingBar) {
    return s_invert_meeting_bar;
  }
  return false;
}

static bool section_uses_light_palette(ColorSection section) {
  return s_light_mode_enabled != section_inverted(section);
}

static ColorSection normalize_color_section(ColorSection section) {
  int section_id = (int)section;
  if (section_id < (int)ColorSectionBase ||
      section_id > (int)ColorSectionMeetingBar) {
    return ColorSectionBase;
  }
  return section;
}

static int sanitize_packed_color(int value) {
  if (value < 0) {
    return 0;
  }
  if (value > 0xFFFFFF) {
    return 0xFFFFFF;
  }
  return value;
}

static GColor gcolor_from_packed_int(int value) {
  return GColorFromHEX((uint32_t)sanitize_packed_color(value));
}

static void store_color_section_value(int values[], ColorSection section,
                                      int value, uint32_t persist_key) {
  section = normalize_color_section(section);
  values[section] = sanitize_packed_color(value);
  persist_write_int(persist_key, values[section]);
  mark_face_dirty();
}

static void load_color_section_value(int values[], ColorSection section,
                                     uint32_t persist_key) {
  if (!persist_exists(persist_key)) {
    return;
  }

  section = normalize_color_section(section);
  values[section] = sanitize_packed_color(persist_read_int(persist_key));
}

static GColor section_bg_color(ColorSection section) {
  section = normalize_color_section(section);
  if (s_color_mode == ColorModeColor) {
    return gcolor_from_packed_int(s_color_section_bg[section]);
  }
  return section_inverted(section) ? theme_fg_color() : theme_bg_color();
}

static GColor section_fg_color(ColorSection section) {
  section = normalize_color_section(section);
  if (s_color_mode == ColorModeColor) {
    return gcolor_from_packed_int(s_color_section_fg[section]);
  }
  return section_inverted(section) ? theme_bg_color() : theme_fg_color();
}

static BitmapTheme section_bitmap_theme(ColorSection section) {
  return section_uses_light_palette(section) ? BitmapThemeLight : BitmapThemeDark;
}

static bool section_backgrounds_differ(ColorSection first, ColorSection second) {
  if (s_color_mode == ColorModeColor) {
    first = normalize_color_section(first);
    second = normalize_color_section(second);
    return sanitize_packed_color(s_color_section_bg[first]) !=
        sanitize_packed_color(s_color_section_bg[second]);
  }
  return section_uses_light_palette(first) != section_uses_light_palette(second);
}

static GColor draw_bg_color(void) {
  return section_bg_color(s_draw_section);
}

static GColor draw_fg_color(void) {
  return section_fg_color(s_draw_section);
}

static BitmapTheme draw_bitmap_theme(void) {
  return section_bitmap_theme(s_draw_section);
}

static void set_draw_section(ColorSection section) {
  s_draw_section = section;
}

static void fill_inverted_section_background(GContext *ctx, ColorSection section, GRect frame) {
  if (s_color_mode != ColorModeColor && !section_inverted(section)) {
    return;
  }

  graphics_context_set_fill_color(ctx, section_bg_color(section));
  graphics_fill_rect(ctx, frame, 0, GCornerNone);
}

static int weather_band_y(void) {
  if (s_verbose_weather_enabled) {
    return s_verbose_weather_large ? 148 : 162;
  }

  return COMPLICATION_CENTER_Y - COMPLICATION_RADIUS - 1;
}

static int weather_band_h(void) {
  if (s_verbose_weather_enabled) {
    return EVENT_SEPARATOR_Y - weather_band_y();
  }

  return (COMPLICATION_RADIUS * 2) + 4;
}

static void set_text_color(GContext *ctx, GColor color) {
  graphics_context_set_text_color(ctx, color);
}

static void draw_text(GContext *ctx, const char *text, GFont font, GRect frame,
                      GColor color, GTextAlignment alignment) {
  set_text_color(ctx, color);
  graphics_draw_text(ctx, text, font, frame, GTextOverflowModeTrailingEllipsis,
                     alignment, NULL);
}

static ComplicationType sanitize_complication(int value, ComplicationType fallback) {
  return value >= ComplicationTemperature && value <= ComplicationDistanceToday
      ? (ComplicationType)value
      : fallback;
}

static ShakeBehavior sanitize_shake_behavior(int value) {
  return value >= ShakeBehaviorOff && value <= ShakeBehaviorStepGraph
      ? (ShakeBehavior)value
      : ShakeBehaviorOff;
}

static void format_unknown(char *buf, size_t len) {
  snprintf(buf, len, "--");
}

static void format_int_3(char *buf, size_t len, bool known, int value) {
  if (!known) {
    format_unknown(buf, len);
    return;
  }
  if (value > 999) {
    snprintf(buf, len, "999");
  } else if (value < -99) {
    snprintf(buf, len, "-99");
  } else {
    snprintf(buf, len, "%d", value);
  }
}

static void format_percent_3(char *buf, size_t len, bool known, int value) {
  if (!known) {
    format_unknown(buf, len);
    return;
  }
  if (value >= 100) {
    snprintf(buf, len, "100");
  } else if (value < 0) {
    snprintf(buf, len, "0%%");
  } else {
    snprintf(buf, len, "%d%%", value);
  }
}

static void format_rain_chance(char *buf, size_t len, bool known, int value) {
  if (!known) {
    format_unknown(buf, len);
    return;
  }
  if (value >= 100) {
    snprintf(buf, len, "100");
  } else if (value < 0) {
    snprintf(buf, len, "0");
  } else {
    snprintf(buf, len, "%d", value);
  }
}

static void format_capped_99_plus(char *buf, size_t len, bool known, int value) {
  if (!known) {
    format_unknown(buf, len);
    return;
  }
  if (value > 99) {
    snprintf(buf, len, "99+");
  } else if (value < 0) {
    snprintf(buf, len, "0");
  } else {
    snprintf(buf, len, "%d", value);
  }
}

static void draw_pixel_block(GContext *ctx, GPoint origin, int x, int y, int w, int h) {
  graphics_fill_rect(ctx, GRect(origin.x + x, origin.y + y, w, h), 0, GCornerNone);
}

static int fitness_sanitize_target(int value, int fallback) {
  if (value < 1) {
    return fallback;
  }
  if (value > 999999) {
    return 999999;
  }
  return value;
}

static int fitness_sanitize_color(int value, int fallback) {
  if (value < 0 || value > 0xFFFFFF) {
    return fallback;
  }
  return value;
}

static GColor fitness_color_from_hex(int value, int fallback) {
  return GColorFromHEX((uint32_t)fitness_sanitize_color(value, fallback));
}

static GColor fitness_track_color(void) {
  return s_light_mode_enabled ? GColorLightGray : GColorDarkGray;
}

static int fitness_overlay_duration_ms_from_seconds(int seconds) {
  if (seconds < 3) {
    seconds = 3;
  }
  if (seconds > 30) {
    seconds = 30;
  }
  return seconds * 1000;
}

static int sanitize_your_day_window_mode(int value) {
  return value == 1 ? 1 : 0;
}

static int sanitize_your_day_window_hours(int value) {
  if (value < 2) {
    return 2;
  }
  if (value > 10) {
    return 10;
  }
  return value;
}

static int sanitize_your_day_hour(int value) {
  if (value < 0) {
    return 0;
  }
  if (value > 23) {
    return 23;
  }
  return value;
}

static GColor fitness_muted_text_color(void) {
  return s_light_mode_enabled ? GColorDarkGray : GColorLightGray;
}

static void format_number_commas(char *buf, size_t len, int value) {
  if (value < 0) {
    value = 0;
  }
  if (value > 999999) {
    snprintf(buf, len, "999,999");
  } else if (value >= 1000) {
    snprintf(buf, len, "%d,%03d", value / 1000, value % 1000);
  } else {
    snprintf(buf, len, "%d", value);
  }
}

static void format_distance_km(char *buf, size_t len, int meters) {
  if (meters < 0) {
    meters = 0;
  }
  int whole = meters / 1000;
  int hundredths = (meters % 1000) / 10;
  snprintf(buf, len, "%d.%02d km", whole, hundredths);
}

static void fitness_draw_ring(GContext *ctx, GPoint center, int radius, int stroke_width,
                              int value, int target, GColor color) {
  GRect frame = GRect(center.x - radius, center.y - radius, radius * 2, radius * 2);
  graphics_context_set_stroke_width(ctx, stroke_width);
  graphics_context_set_stroke_color(ctx, fitness_track_color());
  graphics_draw_arc(ctx, frame, GOvalScaleModeFitCircle, 0, TRIG_MAX_ANGLE);

  if (target < 1 || value < 1) {
    return;
  }

  int32_t end_angle = TRIG_MAX_ANGLE;
  if (value < target) {
    end_angle = (int32_t)(((int64_t)TRIG_MAX_ANGLE * value) / target);
  }
  graphics_context_set_stroke_color(ctx, color);
  graphics_draw_arc(ctx, frame, GOvalScaleModeFitCircle, 0, end_angle);
}

static void fitness_draw_leaf_icon(GContext *ctx, GPoint origin, GColor color) {
  graphics_context_set_fill_color(ctx, color);
  draw_pixel_block(ctx, origin, 7, 1, 4, 2);
  draw_pixel_block(ctx, origin, 5, 3, 8, 2);
  draw_pixel_block(ctx, origin, 3, 5, 9, 2);
  draw_pixel_block(ctx, origin, 2, 7, 7, 2);
  draw_pixel_block(ctx, origin, 1, 9, 5, 2);
  draw_pixel_block(ctx, origin, 5, 10, 2, 2);
  draw_pixel_block(ctx, origin, 7, 8, 2, 2);
  draw_pixel_block(ctx, origin, 9, 6, 2, 2);
  draw_pixel_block(ctx, origin, 11, 4, 2, 2);
}

static void fitness_draw_clock_icon(GContext *ctx, GPoint origin, GColor color) {
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_circle(ctx, GPoint(origin.x + 7, origin.y + 7), 6);
  graphics_draw_line(ctx, GPoint(origin.x + 7, origin.y + 7), GPoint(origin.x + 7, origin.y + 3));
  graphics_draw_line(ctx, GPoint(origin.x + 7, origin.y + 7), GPoint(origin.x + 11, origin.y + 7));
}

static void fitness_draw_flame_icon(GContext *ctx, GPoint origin, GColor color) {
  graphics_context_set_fill_color(ctx, color);
  draw_pixel_block(ctx, origin, 7, 0, 2, 3);
  draw_pixel_block(ctx, origin, 5, 3, 6, 2);
  draw_pixel_block(ctx, origin, 4, 5, 8, 2);
  draw_pixel_block(ctx, origin, 3, 7, 10, 3);
  draw_pixel_block(ctx, origin, 4, 10, 8, 2);
  draw_pixel_block(ctx, origin, 6, 12, 4, 2);
  graphics_context_set_fill_color(ctx, theme_bg_color());
  draw_pixel_block(ctx, origin, 7, 7, 2, 4);
}

static void fitness_draw_metric_row(GContext *ctx, int y, int metric,
                                    int value, int target, const char *unit, GColor color) {
  char value_buf[16];
  char target_buf[16];
  char suffix_buf[24];
  format_number_commas(value_buf, sizeof(value_buf), value);
  format_number_commas(target_buf, sizeof(target_buf), target);
  if (unit && unit[0] != '\0') {
    snprintf(suffix_buf, sizeof(suffix_buf), "/%s %s", target_buf, unit);
  } else {
    snprintf(suffix_buf, sizeof(suffix_buf), "/%s", target_buf);
  }

  GRect measure_frame = GRect(0, 0, SCREEN_W, 24);
  GSize value_size = graphics_text_layout_get_content_size(
      value_buf, s_font_complication, measure_frame,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
  GSize suffix_size = graphics_text_layout_get_content_size(
      suffix_buf, s_font_complication, measure_frame,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);

  const int icon_w = 16;
  const int icon_gap = 8;
  int row_width = icon_w + icon_gap + value_size.w + suffix_size.w;
  int row_x = (SCREEN_W - row_width) / 2;
  if (row_x < 6) {
    row_x = 6;
  }

  GPoint icon_origin = GPoint(row_x, y + 4);
  if (metric == 0) {
    fitness_draw_leaf_icon(ctx, icon_origin, color);
  } else if (metric == 1) {
    fitness_draw_clock_icon(ctx, icon_origin, color);
  } else {
    fitness_draw_flame_icon(ctx, icon_origin, color);
  }

  int text_x = row_x + icon_w + icon_gap;
  draw_text(ctx, value_buf, s_font_complication,
            GRect(text_x, y - 1, value_size.w + 2, 24),
            color, GTextAlignmentLeft);
  draw_text(ctx, suffix_buf, s_font_complication,
            GRect(text_x + value_size.w + 1, y - 1, suffix_size.w + 4, 24),
            fitness_muted_text_color(), GTextAlignmentLeft);
}

static void fitness_draw_info_row(GContext *ctx, int y, const char *label, const char *value) {
  GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  draw_text(ctx, label, label_font, GRect(8, y, 126, 18),
            fitness_muted_text_color(), GTextAlignmentLeft);
  draw_text(ctx, value, s_font_complication, GRect(126, y - 4, 66, 24),
            theme_fg_color(), GTextAlignmentRight);
}

static void fitness_read_health_values(void) {
#if defined(PBL_HEALTH)
  HealthValue steps = health_service_sum_today(HealthMetricStepCount);
  HealthValue active_seconds = health_service_sum_today(HealthMetricActiveSeconds);
  HealthValue active_calories = health_service_sum_today(HealthMetricActiveKCalories);
  HealthValue distance_meters = health_service_sum_today(HealthMetricWalkedDistanceMeters);

  s_fitness_steps_value = steps > 0 ? (int)steps : 0;
  s_fitness_active_minutes_value = active_seconds > 0 ? (int)(active_seconds / 60) : 0;
  s_fitness_active_calories_value = active_calories > 0 ? (int)active_calories : 0;
  s_fitness_distance_meters_value = distance_meters > 0 ? (int)distance_meters : 0;
#else
  s_fitness_steps_value = 0;
  s_fitness_active_minutes_value = 0;
  s_fitness_active_calories_value = 0;
  s_fitness_distance_meters_value = 0;
#endif
}

static void shake_hide_overlay(bool cancel_timer) {
  if (cancel_timer && s_shake_overlay_timer) {
    app_timer_cancel(s_shake_overlay_timer);
    s_shake_overlay_timer = NULL;
  }
  s_shake_overlay_visible = false;
  if (s_shake_overlay_layer) {
    layer_set_hidden(s_shake_overlay_layer, true);
  }
}

static void shake_overlay_timer_handler(void *context) {
  (void)context;
  s_shake_overlay_timer = NULL;
  shake_hide_overlay(false);
}

static void shake_schedule_hide_timer(void) {
  if (s_shake_overlay_timer &&
      app_timer_reschedule(s_shake_overlay_timer, s_fitness_overlay_duration_ms)) {
    return;
  }
  s_shake_overlay_timer = app_timer_register(
      s_fitness_overlay_duration_ms, shake_overlay_timer_handler, NULL);
}

static void shake_show_overlay(void) {
  if (s_shake_behavior == ShakeBehaviorOff || !s_shake_overlay_layer) {
    return;
  }

  if (s_shake_behavior == ShakeBehaviorFitnessRings ||
      s_shake_behavior == ShakeBehaviorStepGraph) {
    fitness_read_health_values();
  } else if (s_shake_behavior == ShakeBehaviorHeartRate) {
    update_stats();
#if defined(PBL_HEALTH)
    health_service_set_heart_rate_sample_period(15);
#endif
  }

  s_shake_overlay_visible = true;
  layer_set_hidden(s_shake_overlay_layer, false);
  layer_mark_dirty(s_shake_overlay_layer);
  shake_schedule_hide_timer();
}

static void shake_tap_handler(AccelAxisType axis, int32_t direction) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Shake tap axis=%d direction=%ld behavior=%d",
          (int)axis, (long)direction, (int)s_shake_behavior);
  shake_show_overlay();
}

static void shake_configure_tap_service(void) {
  if (s_shake_behavior != ShakeBehaviorOff && !s_shake_tap_subscribed) {
    accel_tap_service_subscribe(shake_tap_handler);
    s_shake_tap_subscribed = true;
  } else if (s_shake_behavior == ShakeBehaviorOff && s_shake_tap_subscribed) {
    accel_tap_service_unsubscribe();
    s_shake_tap_subscribed = false;
  }
}

static void fitness_draw_overlay(GContext *ctx) {
  GColor steps_color = fitness_color_from_hex(
      s_fitness_color_steps_hex, FITNESS_DEFAULT_COLOR_STEPS);
  GColor active_color = fitness_color_from_hex(
      s_fitness_color_active_hex, FITNESS_DEFAULT_COLOR_ACTIVE);
  GColor calories_color = fitness_color_from_hex(
      s_fitness_color_calories_hex, FITNESS_DEFAULT_COLOR_CALORIES);

  graphics_context_set_fill_color(ctx, theme_bg_color());
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  const GPoint ring_center = GPoint(100, 64);
  const int ring_stroke = 11;
  if (s_fitness_ring_steps_on) {
    fitness_draw_ring(ctx, ring_center, 58, ring_stroke,
                      s_fitness_steps_value, s_fitness_target_steps, steps_color);
  }
  if (s_fitness_ring_active_on) {
    fitness_draw_ring(ctx, ring_center, 42, ring_stroke,
                      s_fitness_active_minutes_value, s_fitness_target_active_min,
                      active_color);
  }
  if (s_fitness_ring_calories_on) {
    fitness_draw_ring(ctx, ring_center, 26, ring_stroke,
                      s_fitness_active_calories_value, s_fitness_target_calories,
                      calories_color);
  }

  int enabled_rows = 0;
  if (s_fitness_ring_steps_on) {
    enabled_rows++;
  }
  if (s_fitness_ring_active_on) {
    enabled_rows++;
  }
  if (s_fitness_ring_calories_on) {
    enabled_rows++;
  }

  const int separator_y = 192;
  const int row_spacing = 21;
  int row_y = separator_y - (enabled_rows * row_spacing) - 3;
  if (s_fitness_ring_steps_on) {
    fitness_draw_metric_row(ctx, row_y, 0,
                            s_fitness_steps_value, s_fitness_target_steps, "",
                            steps_color);
    row_y += row_spacing;
  }
  if (s_fitness_ring_active_on) {
    fitness_draw_metric_row(ctx, row_y, 1,
                            s_fitness_active_minutes_value, s_fitness_target_active_min,
                            "mins", active_color);
    row_y += row_spacing;
  }
  if (s_fitness_ring_calories_on) {
    fitness_draw_metric_row(ctx, row_y, 2,
                            s_fitness_active_calories_value, s_fitness_target_calories,
                            "Cal", calories_color);
  }

  graphics_context_set_stroke_color(ctx, theme_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(8, separator_y), GPoint(192, separator_y));

  char distance_buf[20];
  format_distance_km(distance_buf, sizeof(distance_buf), s_fitness_distance_meters_value);

  fitness_draw_info_row(ctx, 198, "Distance while active", distance_buf);
}

static void draw_ampm_letter(GContext *ctx, char letter, GPoint origin) {
  if (letter == 'A') {
    draw_pixel_block(ctx, origin, 2, 0, 4, 2);
    draw_pixel_block(ctx, origin, 0, 2, 2, 8);
    draw_pixel_block(ctx, origin, 6, 2, 2, 8);
    draw_pixel_block(ctx, origin, 2, 4, 4, 2);
  } else if (letter == 'P') {
    draw_pixel_block(ctx, origin, 0, 0, 2, 10);
    draw_pixel_block(ctx, origin, 2, 0, 4, 2);
    draw_pixel_block(ctx, origin, 6, 2, 2, 2);
    draw_pixel_block(ctx, origin, 2, 4, 4, 2);
  } else if (letter == 'M') {
    draw_pixel_block(ctx, origin, 0, 0, 2, 10);
    draw_pixel_block(ctx, origin, 8, 0, 2, 10);
    draw_pixel_block(ctx, origin, 2, 2, 2, 2);
    draw_pixel_block(ctx, origin, 6, 2, 2, 2);
    draw_pixel_block(ctx, origin, 4, 4, 2, 2);
  }
}

static void draw_ampm_label(GContext *ctx, const char *label, GPoint origin) {
  graphics_context_set_fill_color(ctx, draw_fg_color());
  draw_ampm_letter(ctx, label[0], origin);
  draw_ampm_letter(ctx, label[1], GPoint(origin.x + 10, origin.y));
}

static bool quiet_time_active_now(void) {
#ifdef FORCE_QUIET_TIME_PREVIEW
  return true;
#else
  return quiet_time_is_active();
#endif
}

static void draw_quiet_time_icon(GContext *ctx, GPoint origin) {
  GBitmap *bitmap = s_quiet_time_mouse_bitmaps[draw_bitmap_theme()];
  if (!bitmap || !quiet_time_active_now()) {
    return;
  }

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, bitmap,
                               GRect(origin.x, origin.y,
                                     QUIET_TIME_ICON_SIZE, QUIET_TIME_ICON_SIZE));
}

static void draw_time_row_at(GContext *ctx, int frame_y, int visual_bottom) {
  const GRect time_frame = GRect(0, frame_y, SCREEN_W, TIME_FRAME_H);
  const int ampm_width = 20;
  const int ampm_height = 10;
  const int ampm_gap = 5;

  GSize time_size = graphics_text_layout_get_content_size(
      s_time_buf, s_font_time, time_frame,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
  int time_left = (SCREEN_W - time_size.w) / 2;
  int ampm_x = time_left - ampm_gap - ampm_width;
  if (ampm_x < 0) {
    ampm_x = 0;
  }

  draw_text(ctx, s_time_buf, s_font_time, time_frame,
            draw_fg_color(), GTextAlignmentCenter);
  if (s_ampm_buf[0] != '\0') {
    draw_ampm_label(ctx, s_ampm_buf, GPoint(ampm_x, visual_bottom - ampm_height));
  }

  draw_quiet_time_icon(ctx,
                       GPoint(QUIET_TIME_ICON_X, visual_bottom - QUIET_TIME_ICON_SIZE));
}

static void draw_time_row(GContext *ctx) {
  draw_time_row_at(ctx, TIME_FRAME_Y, TIME_VISUAL_BOTTOM);
}

static void draw_watch_icon_c(GContext *ctx, GPoint origin) {
  graphics_context_set_stroke_color(ctx, draw_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, GRect(origin.x + 3, origin.y + 4, 8, 10));

  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y + 1), GPoint(origin.x + 8, origin.y + 1));
  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y + 1), GPoint(origin.x + 4, origin.y + 4));
  graphics_draw_line(ctx, GPoint(origin.x + 8, origin.y + 1), GPoint(origin.x + 9, origin.y + 4));
  graphics_draw_line(ctx, GPoint(origin.x + 4, origin.y + 4), GPoint(origin.x + 9, origin.y + 4));

  graphics_draw_line(ctx, GPoint(origin.x + 4, origin.y + 14), GPoint(origin.x + 9, origin.y + 14));
  graphics_draw_line(ctx, GPoint(origin.x + 4, origin.y + 14), GPoint(origin.x + 5, origin.y + 17));
  graphics_draw_line(ctx, GPoint(origin.x + 9, origin.y + 14), GPoint(origin.x + 8, origin.y + 17));
  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y + 17), GPoint(origin.x + 8, origin.y + 17));

  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y + 8), GPoint(origin.x + 9, origin.y + 8));
}

static void draw_watch_battery(GContext *ctx) {
  const GRect battery_frame = GRect(8, 4, 42, 24);
  GSize battery_size = graphics_text_layout_get_content_size(
      s_watch_buf, s_font_top, battery_frame,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);

  draw_text(ctx, s_watch_buf, s_font_top, battery_frame,
            draw_fg_color(), GTextAlignmentLeft);
  draw_watch_icon_c(ctx, GPoint(battery_frame.origin.x + battery_size.w - 1, 5));
}

static void draw_w800_steps_digits(GContext *ctx, GRect frame, int steps) {
  char digits[6];
  if (steps < 0) {
    steps = 0;
  } else if (steps > 99999) {
    steps = 99999;
  }
  snprintf(digits, sizeof(digits), "%d", steps);

  const int digit_w = 12;
  const int digit_h = 24;
  const int digit_count = strlen(digits);
  const int separator_gap = digit_count > 3 ? 2 : 0;
  int x = frame.origin.x + frame.size.w - ((digit_count * digit_w) + separator_gap);
  if (x < frame.origin.x) {
    x = frame.origin.x;
  }

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  BitmapTheme bitmap_theme = draw_bitmap_theme();
  for (int i = 0; i < digit_count; i++) {
    int digit = digits[i] - '0';
    if (digit >= 0 && digit <= 9 && s_w800_digit_bitmaps[bitmap_theme][digit]) {
      int draw_x = x + (i * digit_w);
      if (digit_count > 3 && i >= digit_count - 3) {
        draw_x += separator_gap;
      }
      graphics_draw_bitmap_in_rect(ctx, s_w800_digit_bitmaps[bitmap_theme][digit],
                                   GRect(draw_x, frame.origin.y, digit_w, digit_h));
    }
  }

  if (digit_count > 3) {
    const int comma_x = x + ((digit_count - 3) * digit_w) - 1;
    const int comma_y = frame.origin.y + digit_h - 3;
    graphics_context_set_fill_color(ctx, draw_fg_color());
    graphics_fill_rect(ctx, GRect(comma_x, comma_y, 2, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(comma_x - 1, comma_y + 2, 2, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(comma_x - 2, comma_y + 4, 2, 2), 0, GCornerNone);
  }
}

static void draw_w800_steps_top_bar(GContext *ctx) {
  GBitmap *walking_bitmap = s_w800_walking_bitmaps[draw_bitmap_theme()];
  if (!walking_bitmap) {
    return;
  }

  const GRect bar_frame = GRect(56, 3, 88, 24);
  graphics_context_set_fill_color(ctx, draw_bg_color());
  graphics_fill_rect(ctx, bar_frame, 1, GCornersAll);

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, walking_bitmap, GRect(61, 8, 8, 13));
  draw_w800_steps_digits(ctx, GRect(73, 3, 64, 24), s_steps_count);
}

static void draw_bt_icon(GContext *ctx, GPoint origin) {
  if (!s_phone_connected) {
    graphics_context_set_stroke_color(ctx, GColorRed);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, GPoint(origin.x + 1, origin.y + 1), GPoint(origin.x + 10, origin.y + 12));
    graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y + 1), GPoint(origin.x + 1, origin.y + 12));
    return;
  }

  graphics_context_set_stroke_color(ctx, draw_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y), GPoint(origin.x + 5, origin.y + 14));
  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y), GPoint(origin.x + 10, origin.y + 4));
  graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y + 4), GPoint(origin.x + 2, origin.y + 10));
  graphics_draw_line(ctx, GPoint(origin.x + 2, origin.y + 4), GPoint(origin.x + 10, origin.y + 10));
  graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y + 10), GPoint(origin.x + 5, origin.y + 14));
}

static void draw_step_icon(GContext *ctx, GPoint origin) {
  GBitmap *bitmap = s_step_boot_bitmaps[draw_bitmap_theme()];
  if (!bitmap) {
    return;
  }

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, bitmap, GRect(origin.x, origin.y, 16, 14));
}

static void draw_step_icon_centered(GContext *ctx, GPoint center) {
  draw_step_icon(ctx, GPoint(center.x - 8, center.y - 7));
}

static void draw_icon_block(GContext *ctx, GPoint origin, int x, int y, int w, int h) {
  graphics_fill_rect(ctx, GRect(origin.x + x, origin.y + y, w, h), 0, GCornerNone);
}

static void draw_uv_icon_centered(GContext *ctx, GPoint center) {
  draw_text(ctx, "UV", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 13, center.y - 8, 26, 14),
            draw_fg_color(), GTextAlignmentCenter);
}

static WeatherIconBitmap weather_icon_for_code(void) {
  if (!s_weather_known) {
    return WeatherIconCloudy;
  }

  if (s_weather_code == 0) {
    return WeatherIconSunny;
  } else if (s_weather_code == 1 || s_weather_code == 2) {
    return WeatherIconPartlyCloudy;
  } else if (s_weather_code == 3) {
    return WeatherIconCloudy;
  } else if (s_weather_code == 45 || s_weather_code == 48) {
    return WeatherIconFog;
  } else if ((s_weather_code >= 51 && s_weather_code <= 67) ||
             (s_weather_code >= 80 && s_weather_code <= 82)) {
    return WeatherIconRain;
  } else if ((s_weather_code >= 71 && s_weather_code <= 77) ||
             (s_weather_code >= 85 && s_weather_code <= 86)) {
    return WeatherIconSnow;
  } else if (s_weather_code >= 95 && s_weather_code <= 99) {
    return WeatherIconStorm;
  }

  return WeatherIconCloudy;
}

static void draw_weather_icon_centered(GContext *ctx, GPoint center, bool large) {
  WeatherIconBitmap icon = weather_icon_for_code();
  BitmapTheme bitmap_theme = draw_bitmap_theme();
  GBitmap *bitmap = large
      ? s_weather_icon_large_bitmaps[bitmap_theme][icon]
      : s_weather_icon_small_bitmaps[bitmap_theme][icon];
  int size = large ? WEATHER_ICON_LARGE_SIZE : WEATHER_ICON_SMALL_SIZE;

  if (!bitmap) {
    return;
  }

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, bitmap,
                               GRect(center.x - (size / 2), center.y - (size / 2),
                                     size, size));
}

static void draw_temp_unit_icon_centered(GContext *ctx, GPoint center) {
  GPoint origin = GPoint(center.x - 7, center.y - 5);
  graphics_context_set_fill_color(ctx, draw_fg_color());
  graphics_context_set_stroke_color(ctx, draw_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, GPoint(origin.x + 2, origin.y + 2), 2);

  if (s_temperature_unit_celsius) {
    draw_icon_block(ctx, origin, 8, 0, 7, 2);
    draw_icon_block(ctx, origin, 7, 1, 2, 8);
    draw_icon_block(ctx, origin, 8, 8, 7, 2);
  } else {
    draw_icon_block(ctx, origin, 7, 0, 8, 2);
    draw_icon_block(ctx, origin, 7, 0, 2, 10);
    draw_icon_block(ctx, origin, 7, 4, 7, 2);
  }
}

static void draw_drop_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_stroke_color(ctx, draw_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(origin.x + 7, origin.y), GPoint(origin.x + 3, origin.y + 7));
  graphics_draw_line(ctx, GPoint(origin.x + 7, origin.y), GPoint(origin.x + 11, origin.y + 7));
  graphics_draw_line(ctx, GPoint(origin.x + 3, origin.y + 7), GPoint(origin.x + 3, origin.y + 11));
  graphics_draw_line(ctx, GPoint(origin.x + 11, origin.y + 7), GPoint(origin.x + 11, origin.y + 11));
  graphics_draw_line(ctx, GPoint(origin.x + 4, origin.y + 12), GPoint(origin.x + 10, origin.y + 12));
  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y + 13), GPoint(origin.x + 9, origin.y + 13));
}

static void draw_drop_icon_centered(GContext *ctx, GPoint center) {
  draw_drop_icon(ctx, GPoint(center.x - 7, center.y - 8));
}

static void draw_heart_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_fill_color(ctx, draw_fg_color());
  draw_icon_block(ctx, origin, 3, 4, 4, 2);
  draw_icon_block(ctx, origin, 10, 4, 4, 2);
  draw_icon_block(ctx, origin, 2, 6, 13, 4);
  draw_icon_block(ctx, origin, 4, 10, 9, 2);
  draw_icon_block(ctx, origin, 6, 12, 5, 2);
  draw_icon_block(ctx, origin, 8, 14, 1, 1);
}

static void draw_heart_icon_centered(GContext *ctx, GPoint center) {
  draw_heart_icon(ctx, GPoint(center.x - 8, center.y - 8));
}

static void draw_watch_icon_centered(GContext *ctx, GPoint center) {
  draw_watch_icon_c(ctx, GPoint(center.x - 7, center.y - 9));
}

static void draw_phone_icon_centered(GContext *ctx, GPoint center) {
  graphics_context_set_stroke_color(ctx, draw_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, GRect(center.x - 5, center.y - 8, 10, 16));
  graphics_draw_line(ctx, GPoint(center.x - 2, center.y + 5), GPoint(center.x + 2, center.y + 5));
}

static void draw_wind_icon_centered(GContext *ctx, GPoint center) {
  graphics_context_set_stroke_color(ctx, draw_fg_color());
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_line(ctx, GPoint(center.x - 8, center.y - 5), GPoint(center.x + 7, center.y - 5));
  graphics_draw_line(ctx, GPoint(center.x - 5, center.y), GPoint(center.x + 5, center.y));
  graphics_draw_line(ctx, GPoint(center.x - 8, center.y + 5), GPoint(center.x + 2, center.y + 5));
}

static void draw_calendar_icon_centered(GContext *ctx, GPoint center) {
  graphics_context_set_fill_color(ctx, draw_fg_color());
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 0, 2, 14, 2);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 0, 4, 2, 11);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 12, 4, 2, 11);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 0, 13, 14, 2);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 4, 7, 2, 2);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 8, 7, 2, 2);
}

static void draw_complication_fitness_rings(GContext *ctx, GPoint center) {
  fitness_read_health_values();

  GColor steps_color = fitness_color_from_hex(
      s_fitness_color_steps_hex, FITNESS_DEFAULT_COLOR_STEPS);
  GColor active_color = fitness_color_from_hex(
      s_fitness_color_active_hex, FITNESS_DEFAULT_COLOR_ACTIVE);
  GColor calories_color = fitness_color_from_hex(
      s_fitness_color_calories_hex, FITNESS_DEFAULT_COLOR_CALORIES);

  const int outer_radius = 22;
  const int ring_gap = 5;
  const int ring_stroke = 3;
  fitness_draw_ring(ctx, center, outer_radius, ring_stroke,
                    s_fitness_steps_value, s_fitness_target_steps, steps_color);
  fitness_draw_ring(ctx, center, outer_radius - ring_gap, ring_stroke,
                    s_fitness_active_minutes_value, s_fitness_target_active_min,
                    active_color);
  fitness_draw_ring(ctx, center, outer_radius - (ring_gap * 2), ring_stroke,
                    s_fitness_active_calories_value, s_fitness_target_calories,
                    calories_color);
}

static void draw_complication_weather_circle(GContext *ctx, GPoint center) {
  draw_weather_icon_centered(ctx, GPoint(center.x, center.y - 7), false);
  draw_text(ctx, s_temp_buf, s_font_complication,
            GRect(center.x - 22, center.y + 2, 44, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static void draw_complication_battery_ring(GContext *ctx, GPoint center) {
  int battery_pct = s_watch_battery_pct;
  if (battery_pct < 0) {
    battery_pct = 0;
  } else if (battery_pct > 100) {
    battery_pct = 100;
  }

  const int radius = 22;
  const int empty_angle = ((100 - battery_pct) * 360) / 100;
  const GRect frame = GRect(center.x - radius, center.y - radius,
                            radius * 2, radius * 2);
  graphics_context_set_fill_color(ctx, battery_pct > 20 ? draw_fg_color() : GColorRed);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 4,
                       DEG_TO_TRIGANGLE(empty_angle), DEG_TO_TRIGANGLE(360));
  draw_text(ctx, s_watch_battery_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 22, center.y - 9, 44, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static int health_sum_today_int(HealthMetric metric) {
#if defined(PBL_HEALTH)
  HealthValue value = health_service_sum_today(metric);
  return value > 0 ? (int)value : 0;
#else
  (void)metric;
  return 0;
#endif
}

static void draw_complication_high_low_combined(GContext *ctx, GPoint center) {
  char hi_buf[8];
  char lo_buf[8];
  format_temp_with_degree(hi_buf, sizeof(hi_buf), s_high_temp_known, s_high_temp);
  format_temp_with_degree(lo_buf, sizeof(lo_buf), s_low_temp_known, s_low_temp);

  draw_text(ctx, hi_buf, s_font_complication, GRect(center.x - 23, center.y - 18, 46, 20),
            draw_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, lo_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 23, center.y + 2, 46, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static void draw_complication_active_minutes(GContext *ctx, GPoint center) {
  char value_buf[8];
  int minutes = health_sum_today_int(HealthMetricActiveSeconds) / 60;
  format_int_3(value_buf, sizeof(value_buf), true, minutes);
  draw_text(ctx, value_buf, s_font_complication,
            GRect(center.x - 23, center.y - 14, 46, 20),
            draw_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, "min", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 23, center.y + 4, 46, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static void draw_complication_active_calories(GContext *ctx, GPoint center) {
  char value_buf[8];
  format_int_3(value_buf, sizeof(value_buf), true,
               health_sum_today_int(HealthMetricActiveKCalories));
  draw_text(ctx, value_buf, s_font_complication,
            GRect(center.x - 23, center.y - 14, 46, 20),
            draw_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, "Cal", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 23, center.y + 4, 46, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static void draw_complication_sleep_last_night(GContext *ctx, GPoint center) {
  int minutes = health_sum_today_int(HealthMetricSleepSeconds) / 60;
  int hours = minutes / 60;
  int mins = minutes % 60;
  char value_buf[12];
  snprintf(value_buf, sizeof(value_buf), "%dh%02dm", hours, mins);
  draw_text(ctx, value_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 23, center.y - 8, 46, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static void draw_complication_distance_today(GContext *ctx, GPoint center) {
  char value_buf[16];
  format_distance_km(value_buf, sizeof(value_buf),
                     health_sum_today_int(HealthMetricWalkedDistanceMeters));
  draw_text(ctx, value_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 23, center.y - 8, 46, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static void draw_complication_icon(GContext *ctx, ComplicationType type, GPoint center) {
  if (type == ComplicationSteps) {
    draw_step_icon_centered(ctx, center);
  } else if (type == ComplicationTemperature ||
             type == ComplicationFeelsLike ||
             type == ComplicationHighTemp) {
    draw_temp_unit_icon_centered(ctx, center);
  } else if (type == ComplicationRainChance) {
    draw_drop_icon_centered(ctx, center);
  } else if (type == ComplicationHeartRate) {
    draw_heart_icon_centered(ctx, center);
  } else if (type == ComplicationWatchBattery) {
    draw_watch_icon_centered(ctx, center);
  } else if (type == ComplicationPhoneBattery) {
    draw_phone_icon_centered(ctx, center);
  } else if (type == ComplicationWindSpeed) {
    draw_wind_icon_centered(ctx, center);
  } else if (type == ComplicationUvIndex) {
    draw_uv_icon_centered(ctx, center);
  } else if (type == ComplicationNextEvent) {
    draw_calendar_icon_centered(ctx, center);
  } else if (type == ComplicationWeather) {
    draw_weather_icon_centered(ctx, center, false);
  } else if (type == ComplicationWeatherIcon) {
    draw_weather_icon_centered(ctx, center, true);
  }
}

static const char *value_for_complication(ComplicationType type) {
  if (type == ComplicationSteps) {
    return s_steps_buf;
  } else if (type == ComplicationTemperature) {
    return s_temp_buf;
  } else if (type == ComplicationRainChance) {
    return s_rain_buf;
  } else if (type == ComplicationHeartRate) {
    return s_bpm_buf;
  } else if (type == ComplicationWatchBattery) {
    return s_watch_battery_buf;
  } else if (type == ComplicationPhoneBattery) {
    return s_phone_battery_buf;
  } else if (type == ComplicationFeelsLike) {
    return s_feels_like_buf;
  } else if (type == ComplicationHighTemp) {
    return s_high_temp_buf;
  } else if (type == ComplicationWindSpeed) {
    return s_wind_buf;
  } else if (type == ComplicationUvIndex) {
    return s_uv_buf;
  } else if (type == ComplicationNextEvent) {
    return s_event_delta_buf;
  } else if (type == ComplicationWeather) {
    return s_temp_buf;
  } else if (type == ComplicationWeatherIcon) {
    return "";
  } else if (type == ComplicationFitnessRings ||
             type == ComplicationWeatherCircle ||
             type == ComplicationBatteryRing ||
             type == ComplicationHighLowCombined ||
             type == ComplicationActiveMinutes ||
             type == ComplicationActiveCalories ||
             type == ComplicationSleepLastNight ||
             type == ComplicationDistanceToday) {
    return "";
  }
  return "--";
}

static void draw_complication(GContext *ctx, GPoint center, ComplicationType type) {
  const char *value = value_for_complication(type);

  graphics_context_set_stroke_color(ctx, draw_fg_color());
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_circle(ctx, center, COMPLICATION_RADIUS);

  if (type == ComplicationWeatherIcon) {
    draw_complication_icon(ctx, type, center);
    return;
  } else if (type == ComplicationFitnessRings) {
    draw_complication_fitness_rings(ctx, center);
    return;
  } else if (type == ComplicationWeatherCircle) {
    draw_complication_weather_circle(ctx, center);
    return;
  } else if (type == ComplicationBatteryRing) {
    draw_complication_battery_ring(ctx, center);
    return;
  } else if (type == ComplicationHighLowCombined) {
    draw_complication_high_low_combined(ctx, center);
    return;
  } else if (type == ComplicationActiveMinutes) {
    draw_complication_active_minutes(ctx, center);
    return;
  } else if (type == ComplicationActiveCalories) {
    draw_complication_active_calories(ctx, center);
    return;
  } else if (type == ComplicationSleepLastNight) {
    draw_complication_sleep_last_night(ctx, center);
    return;
  } else if (type == ComplicationDistanceToday) {
    draw_complication_distance_today(ctx, center);
    return;
  }

  draw_complication_icon(ctx, type, GPoint(center.x, center.y - 12));

  GFont value_font = strlen(value) > 2
      ? fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)
      : s_font_complication;
  draw_text(ctx, value, value_font,
            GRect(center.x - 23, center.y - 6, 46, 18),
            draw_fg_color(), GTextAlignmentCenter);
}

static void format_temp_with_degree(char *buf, size_t len, bool known, int value) {
  if (!known) {
    format_unknown(buf, len);
    return;
  }
  snprintf(buf, len, "%d\xC2\xB0", value);
}

static void format_time_short(char *buf, size_t len, time_t timestamp, bool known) {
  if (!known || timestamp <= 0) {
    snprintf(buf, len, "--:--");
    return;
  }

  struct tm *time_info = localtime(&timestamp);
  if (!time_info) {
    snprintf(buf, len, "--:--");
    return;
  }

  if (s_military_time_enabled) {
    strftime(buf, len, "%H:%M", time_info);
  } else {
    char tmp[12];
    strftime(tmp, sizeof(tmp), "%I:%M%p", time_info);
    if (tmp[0] == '0') {
      memmove(tmp, tmp + 1, strlen(tmp));
    }
    snprintf(buf, len, "%s", tmp);
  }
}

static void draw_overlay_title(GContext *ctx, const char *title) {
  draw_text(ctx, title, s_font_top, GRect(8, 2, SCREEN_W - 16, 24),
            fitness_muted_text_color(), GTextAlignmentCenter);
}

static const char *calendar_title_for_index(int index) {
  if (index == 0) {
    return s_event_buf;
  }
  return s_calendar_event_titles[index - 1];
}

static const char *calendar_delta_for_index(int index) {
  if (index == 0) {
    return s_event_delta_buf;
  }
  return s_calendar_event_deltas[index - 1];
}

static bool calendar_title_is_empty(const char *title) {
  return !title || title[0] == '\0' || strcmp(title, "[None]") == 0;
}

static void calendar_draw_overlay(GContext *ctx) {
  draw_overlay_title(ctx, "UPCOMING");

  int row_count = s_calendar_shake_event_count == 5 ? 5 : 3;
  int visible_count = 0;
  for (int i = 0; i < row_count; i++) {
    if (!calendar_title_is_empty(calendar_title_for_index(i))) {
      visible_count++;
    }
  }

  if (visible_count == 0) {
    draw_text(ctx, "[No Events]", s_font_complication,
              GRect(8, 96, SCREEN_W - 16, 28), theme_fg_color(), GTextAlignmentCenter);
    return;
  }

  int row_h = row_count == 5 ? 38 : 54;
  int y = row_count == 5 ? 27 : 34;
  for (int i = 0; i < row_count; i++) {
    const char *title = calendar_title_for_index(i);
    const char *delta = calendar_delta_for_index(i);
    if (calendar_title_is_empty(title)) {
      continue;
    }

    draw_text(ctx, title, s_font_event, GRect(8, y, SCREEN_W - 16, 22),
              theme_fg_color(), GTextAlignmentLeft);
    draw_text(ctx, delta && delta[0] ? delta : "--",
              fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
              GRect(8, y + 19, SCREEN_W - 16, 18),
              fitness_muted_text_color(), GTextAlignmentLeft);

    if (i < row_count - 1) {
      graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
      graphics_context_set_stroke_width(ctx, 1);
      graphics_draw_line(ctx, GPoint(8, y + row_h - 2), GPoint(192, y + row_h - 2));
    }
    y += row_h;
  }
}

static bool day_event_hour_busy(int day_offset, int hour) {
  int bitmap;
  if (day_offset < 0) {
    bitmap = s_day_event_hours_bitmap_yesterday;
  } else if (day_offset > 0) {
    bitmap = s_day_event_hours_bitmap_tomorrow;
  } else {
    bitmap = s_day_event_hours_bitmap;
  }
  return (bitmap & (1 << hour)) != 0;
}

static int your_day_fixed_hour_count(void) {
  int span = s_your_day_end_hour - s_your_day_start_hour;
  if (span < 0) {
    span += 24;
  }
  return sanitize_your_day_window_hours(span + 1);
}

static int your_day_display_hour_count(void) {
  return s_your_day_window_mode == 1
      ? your_day_fixed_hour_count()
      : sanitize_your_day_window_hours(s_your_day_window_hours);
}

static int your_day_start_hour(struct tm *now_tm) {
  if (s_your_day_window_mode == 1) {
    return s_your_day_start_hour;
  }
  return (now_tm ? now_tm->tm_hour : 0) - 1;
}

static void normalize_display_hour(int *hour, int *day_offset) {
  while (*hour < 0) {
    *hour += 24;
    (*day_offset)--;
  }
  while (*hour >= 24) {
    *hour -= 24;
    (*day_offset)++;
  }
}

static void draw_your_day_hour_pips(GContext *ctx) {
  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);
  int current_hour = now_tm ? now_tm->tm_hour : 0;
  int pip_count = your_day_display_hour_count();
  int start_hour = your_day_start_hour(now_tm);

  int gap_x = pip_count > 1 ? 180 / (pip_count - 1) : 0;
  if (gap_x > 22) {
    gap_x = 22;
  }
  const int start_x = (SCREEN_W - ((pip_count - 1) * gap_x)) / 2;
  const int y = 96;
  GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);

  for (int i = 0; i < pip_count; i++) {
    int hour = start_hour + i;
    int day_offset = 0;
    normalize_display_hour(&hour, &day_offset);
    int x = start_x + (i * gap_x);
    bool busy = day_event_hour_busy(day_offset, hour);
    bool current = day_offset == 0 && hour == current_hour;

    graphics_context_set_stroke_color(ctx, theme_fg_color());
    graphics_context_set_fill_color(ctx, theme_fg_color());
    graphics_context_set_stroke_width(ctx, 1);
    if (current) {
      graphics_draw_circle(ctx, GPoint(x, y), 5);
      graphics_fill_circle(ctx, GPoint(x, y), 2);
    } else if (busy) {
      graphics_fill_circle(ctx, GPoint(x, y), 5);
    } else {
      graphics_draw_circle(ctx, GPoint(x, y), 5);
    }

    char label[4];
    int display_hour = s_military_time_enabled ? hour : hour % 12;
    if (!s_military_time_enabled && display_hour == 0) {
      display_hour = 12;
    }
    snprintf(label, sizeof(label), "%d", display_hour);
    draw_text(ctx, label, label_font, GRect(x - 8, y + 9, 16, 16),
              fitness_muted_text_color(), GTextAlignmentCenter);
  }
}

static void your_day_draw_overlay(GContext *ctx) {
  char hi_buf[8];
  char lo_buf[8];
  char rain_buf[8];
  char weather_buf[32];
  format_temp_with_degree(hi_buf, sizeof(hi_buf), s_high_temp_known, s_high_temp);
  format_temp_with_degree(lo_buf, sizeof(lo_buf), s_low_temp_known, s_low_temp);
  format_percent_3(rain_buf, sizeof(rain_buf), s_rain_known, s_rain_chance);
  snprintf(weather_buf, sizeof(weather_buf), "%s/%s  %s rain", hi_buf, lo_buf, rain_buf);

  draw_weather_icon_centered(ctx, GPoint(38, 30), true);
  draw_text(ctx, weather_buf, s_font_complication, GRect(64, 18, 128, 28),
            theme_fg_color(), GTextAlignmentLeft);

  graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(8, 58), GPoint(192, 58));

  char title_buf[24];
  int span = your_day_display_hour_count();
  if (s_your_day_window_mode == 1) {
    snprintf(title_buf, sizeof(title_buf), "%d HOUR WORKDAY", span);
  } else {
    snprintf(title_buf, sizeof(title_buf), "%d HOUR WINDOW", span);
  }
  draw_text(ctx, title_buf, s_font_top, GRect(8, 64, SCREEN_W - 16, 24),
            fitness_muted_text_color(), GTextAlignmentCenter);
  draw_your_day_hour_pips(ctx);

  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);
  char now_buf[24];
  if (now_tm) {
    if (s_military_time_enabled) {
      strftime(now_buf, sizeof(now_buf), "Now: %H:%M", now_tm);
    } else {
      strftime(now_buf, sizeof(now_buf), "Now: %I:%M%p", now_tm);
      if (now_buf[5] == '0') {
        memmove(now_buf + 5, now_buf + 6, strlen(now_buf + 5));
      }
    }
  } else {
    snprintf(now_buf, sizeof(now_buf), "Now: --:--");
  }
  draw_text(ctx, now_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18),
            GRect(8, 126, SCREEN_W - 16, 22), theme_fg_color(), GTextAlignmentCenter);

  graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
  graphics_draw_line(ctx, GPoint(8, 154), GPoint(192, 154));

  char count_buf[28];
  snprintf(count_buf, sizeof(count_buf), "%d events in window", s_day_event_count_today);
  draw_text(ctx, count_buf, s_font_complication, GRect(8, 162, SCREEN_W - 16, 24),
            theme_fg_color(), GTextAlignmentCenter);

  char next_buf[96];
  snprintf(next_buf, sizeof(next_buf), "Next: %s",
           calendar_title_is_empty(s_event_buf) ? "[None]" : s_event_buf);
  draw_text(ctx, next_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(8, 190, SCREEN_W - 16, 28), fitness_muted_text_color(),
            GTextAlignmentCenter);
}

static void draw_small_sun(GContext *ctx, GPoint center, GColor color) {
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_fill_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_fill_circle(ctx, center, 3);
  graphics_draw_line(ctx, GPoint(center.x - 6, center.y), GPoint(center.x - 4, center.y));
  graphics_draw_line(ctx, GPoint(center.x + 4, center.y), GPoint(center.x + 6, center.y));
  graphics_draw_line(ctx, GPoint(center.x, center.y - 6), GPoint(center.x, center.y - 4));
  graphics_draw_line(ctx, GPoint(center.x, center.y + 4), GPoint(center.x, center.y + 6));
}

static void draw_small_moon(GContext *ctx, GPoint center, GColor color) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_circle(ctx, center, 5);
  graphics_context_set_fill_color(ctx, theme_bg_color());
  graphics_fill_circle(ctx, GPoint(center.x + 3, center.y - 1), 5);
}

static void detailed_weather_draw_overlay(GContext *ctx) {
  char temp_buf[10];
  char feels_buf[20];
  char hi_buf[8];
  char lo_buf[8];
  char forecast_buf[48];
  char wind_uv_buf[40];
  char sunrise_buf[12];
  char sunset_buf[12];

  format_temp_with_degree(temp_buf, sizeof(temp_buf), s_temperature_known, s_temperature);
  format_temp_with_degree(hi_buf, sizeof(hi_buf), s_high_temp_known, s_high_temp);
  format_temp_with_degree(lo_buf, sizeof(lo_buf), s_low_temp_known, s_low_temp);
  if (s_feels_like_known) {
    snprintf(feels_buf, sizeof(feels_buf), "Feels %d\xC2\xB0", s_feels_like);
  } else {
    snprintf(feels_buf, sizeof(feels_buf), "Feels --");
  }
  snprintf(forecast_buf, sizeof(forecast_buf), "Hi %s   Lo %s   Rain %s",
           hi_buf, lo_buf, s_rain_buf);
  snprintf(wind_uv_buf, sizeof(wind_uv_buf), "Wind %s mph    UV %s", s_wind_buf, s_uv_buf);
  format_time_short(sunrise_buf, sizeof(sunrise_buf), s_sunrise_t, s_sunrise_known);
  format_time_short(sunset_buf, sizeof(sunset_buf), s_sunset_t, s_sunset_known);

  draw_weather_icon_centered(ctx, GPoint(100, 28), true);
  draw_text(ctx, temp_buf, s_font_time, GRect(0, 54, SCREEN_W, 52),
            theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, feels_buf, s_font_complication, GRect(8, 98, SCREEN_W - 16, 24),
            fitness_muted_text_color(), GTextAlignmentCenter);
  draw_text(ctx, forecast_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(6, 128, SCREEN_W - 12, 22), theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, wind_uv_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(6, 154, SCREEN_W - 12, 22), theme_fg_color(), GTextAlignmentCenter);

  draw_small_sun(ctx, GPoint(43, 194), theme_fg_color());
  draw_text(ctx, sunrise_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(54, 184, 48, 24), theme_fg_color(), GTextAlignmentLeft);
  draw_small_moon(ctx, GPoint(122, 194), theme_fg_color());
  draw_text(ctx, sunset_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(133, 184, 58, 24), theme_fg_color(), GTextAlignmentLeft);
}

static void alt_timezone_draw_overlay(GContext *ctx) {
  time_t adjusted = time(NULL) + (s_alt_tz_offset_min * 60);
  struct tm *alt_tm = gmtime(&adjusted);
  char time_buf[12];
  char date_buf[32];
  if (alt_tm) {
    if (s_military_time_enabled) {
      strftime(time_buf, sizeof(time_buf), "%H:%M", alt_tm);
    } else {
      strftime(time_buf, sizeof(time_buf), "%I:%M %p", alt_tm);
      if (time_buf[0] == '0') {
        memmove(time_buf, time_buf + 1, strlen(time_buf));
      }
    }
    strftime(date_buf, sizeof(date_buf), "%a, %b %d", alt_tm);
  } else {
    snprintf(time_buf, sizeof(time_buf), "--:--");
    snprintf(date_buf, sizeof(date_buf), "---");
  }

  int abs_offset = s_alt_tz_offset_min < 0 ? -s_alt_tz_offset_min : s_alt_tz_offset_min;
  int hours = abs_offset / 60;
  int minutes = abs_offset % 60;
  char offset_buf[32];
  if (s_alt_tz_offset_min == 0) {
    snprintf(offset_buf, sizeof(offset_buf), "UTC");
  } else if (minutes == 0) {
    snprintf(offset_buf, sizeof(offset_buf), "%d hours %s", hours,
             s_alt_tz_offset_min > 0 ? "ahead" : "behind");
  } else {
    snprintf(offset_buf, sizeof(offset_buf), "%d:%02d %s", hours, minutes,
             s_alt_tz_offset_min > 0 ? "ahead" : "behind");
  }

  draw_text(ctx, s_alt_tz_label, s_font_top, GRect(8, 22, SCREEN_W - 16, 24),
            fitness_muted_text_color(), GTextAlignmentCenter);
  draw_text(ctx, time_buf, s_font_time, GRect(0, 62, SCREEN_W, 58),
            theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, date_buf, s_font_complication, GRect(8, 132, SCREEN_W - 16, 24),
            theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, offset_buf, s_font_complication, GRect(8, 172, SCREEN_W - 16, 24),
            fitness_muted_text_color(), GTextAlignmentCenter);
}

static void draw_large_heart(GContext *ctx, GPoint origin, GColor color) {
  graphics_context_set_fill_color(ctx, color);
  draw_pixel_block(ctx, origin, 6, 0, 10, 4);
  draw_pixel_block(ctx, origin, 22, 0, 10, 4);
  draw_pixel_block(ctx, origin, 2, 4, 34, 8);
  draw_pixel_block(ctx, origin, 6, 12, 26, 4);
  draw_pixel_block(ctx, origin, 10, 16, 18, 4);
  draw_pixel_block(ctx, origin, 14, 20, 10, 4);
  draw_pixel_block(ctx, origin, 18, 24, 2, 2);
}

static const char *heart_rate_zone(int bpm) {
  if (bpm <= 0) {
    return "No reading";
  }
  if (bpm < 80) {
    return "Resting";
  }
  if (bpm < 120) {
    return "Light";
  }
  if (bpm < 150) {
    return "Moderate";
  }
  return "Vigorous";
}

static void heart_rate_draw_overlay(GContext *ctx) {
  int bpm = atoi(s_bpm_buf);
  char last_buf[32];
  if (s_last_hr_sample_time > 0) {
    int diff = (int)(time(NULL) - s_last_hr_sample_time);
    if (diff < 0) {
      diff = 0;
    }
    snprintf(last_buf, sizeof(last_buf), "Last: %d sec ago", diff);
  } else {
    snprintf(last_buf, sizeof(last_buf), "Last: --");
  }

  draw_large_heart(ctx, GPoint(82, 24), GColorRed);
  draw_text(ctx, s_bpm_buf, s_font_time, GRect(0, 66, SCREEN_W, 58),
            theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, "BPM", s_font_complication, GRect(8, 120, SCREEN_W - 16, 24),
            fitness_muted_text_color(), GTextAlignmentCenter);
  draw_text(ctx, heart_rate_zone(bpm), s_font_complication,
            GRect(8, 154, SCREEN_W - 16, 24), theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, last_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18),
            GRect(8, 188, SCREEN_W - 16, 24), fitness_muted_text_color(),
            GTextAlignmentCenter);
}

static void draw_step_graph_bars(GContext *ctx, int *hour_steps, int count) {
  int max_steps = 1;
  for (int i = 0; i < count; i++) {
    if (hour_steps[i] > max_steps) {
      max_steps = hour_steps[i];
    }
  }

  const int graph_bottom = 162;
  const int max_h = 94;
  const int bar_w = 9;
  const int gap = 3;
  const int start_x = 5;
  graphics_context_set_fill_color(ctx, theme_fg_color());
  for (int i = 0; i < count; i++) {
    int h = (hour_steps[i] * max_h) / max_steps;
    if (hour_steps[i] > 0 && h < 2) {
      h = 2;
    }
    graphics_fill_rect(ctx, GRect(start_x + (i * (bar_w + gap)),
                                  graph_bottom - h, bar_w, h),
                       0, GCornerNone);
  }
}

static void step_graph_draw_overlay(GContext *ctx) {
  int hour_steps[16];
  memset(hour_steps, 0, sizeof(hour_steps));

#if defined(PBL_HEALTH)
  time_t now = time(NULL);
  struct tm start_tm = *localtime(&now);
  start_tm.tm_min = 0;
  start_tm.tm_sec = 0;
  for (int i = 0; i < 16; i++) {
    start_tm.tm_hour = 6 + i;
    time_t start = mktime(&start_tm);
    time_t end = start + 3600;
    HealthMinuteData records[60];
    uint32_t count = health_service_get_minute_history(records, 60, &start, &end);
    for (uint32_t j = 0; j < count; j++) {
      if (!records[j].is_invalid) {
        hour_steps[i] += records[j].steps;
      }
    }
  }
#endif

  char total_buf[36];
  char steps_buf[16];
  char target_buf[16];
  format_number_commas(steps_buf, sizeof(steps_buf), s_fitness_steps_value);
  format_number_commas(target_buf, sizeof(target_buf), s_fitness_target_steps);
  snprintf(total_buf, sizeof(total_buf), "%s / %s", steps_buf, target_buf);

  draw_overlay_title(ctx, "STEPS TODAY");
  draw_text(ctx, total_buf, s_font_complication, GRect(8, 26, SCREEN_W - 16, 24),
            theme_fg_color(), GTextAlignmentCenter);
  draw_step_graph_bars(ctx, hour_steps, 16);
  draw_text(ctx, "6     9     12     3     6     9",
            fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(0, 166, SCREEN_W, 18), fitness_muted_text_color(),
            GTextAlignmentCenter);

  int percent = s_fitness_target_steps > 0
      ? (s_fitness_steps_value * 100) / s_fitness_target_steps
      : 0;
  if (percent > 999) {
    percent = 999;
  }
  char pct_buf[24];
  snprintf(pct_buf, sizeof(pct_buf), "%d%% to goal", percent);
  draw_text(ctx, pct_buf, s_font_complication, GRect(8, 198, SCREEN_W - 16, 24),
            theme_fg_color(), GTextAlignmentCenter);
}

static void shake_overlay_update_proc(Layer *layer, GContext *ctx) {
  (void)layer;

  graphics_context_set_fill_color(ctx, theme_bg_color());
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  switch (s_shake_behavior) {
    case ShakeBehaviorFitnessRings:
      fitness_draw_overlay(ctx);
      break;
    case ShakeBehaviorCalendarEvents:
      calendar_draw_overlay(ctx);
      break;
    case ShakeBehaviorYourDay:
      your_day_draw_overlay(ctx);
      break;
    case ShakeBehaviorDetailedWeather:
      detailed_weather_draw_overlay(ctx);
      break;
    case ShakeBehaviorAltTimezone:
      alt_timezone_draw_overlay(ctx);
      break;
    case ShakeBehaviorHeartRate:
      heart_rate_draw_overlay(ctx);
      break;
    case ShakeBehaviorStepGraph:
      step_graph_draw_overlay(ctx);
      break;
    case ShakeBehaviorOff:
    default:
      break;
  }
}

static void draw_verbose_weather_row(GContext *ctx) {
  char temp_text[12];
  if (s_temperature_known) {
    snprintf(temp_text, sizeof(temp_text), "%s%s", s_temp_buf,
             s_temperature_unit_celsius ? "\xC2\xB0""C" : "\xC2\xB0""F");
  } else {
    snprintf(temp_text, sizeof(temp_text), "--");
  }

  const char *summary = s_weather_summary_buf[0] ? s_weather_summary_buf : "WEATHER";

  if (s_verbose_weather_large) {
    char large_temp_text[12];
    snprintf(large_temp_text, sizeof(large_temp_text), "%s", temp_text);

    const int icon_size = WEATHER_ICON_LARGE_SIZE;
    const int icon_gap = 8;
    GFont temp_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    GFont summary_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    const GRect measure_frame = GRect(0, 0, SCREEN_W, 28);
    GSize temp_size = graphics_text_layout_get_content_size(
        large_temp_text, temp_font, measure_frame,
        GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
    int top_row_width = icon_size + icon_gap + temp_size.w;
    int top_row_x = (SCREEN_W - top_row_width) / 2;
    const int top_center_y = VERBOSE_WEATHER_CENTER_Y - 19;

    draw_weather_icon_centered(ctx,
                               GPoint(top_row_x + (icon_size / 2), top_center_y),
                               true);
    draw_text(ctx, large_temp_text, temp_font,
              GRect(top_row_x + icon_size + icon_gap, top_center_y - 19,
                    SCREEN_W - top_row_x - icon_size - icon_gap, 32),
              draw_fg_color(), GTextAlignmentLeft);

    draw_text(ctx, summary, summary_font,
              GRect(8, VERBOSE_WEATHER_CENTER_Y - 4, SCREEN_W - 16, 20),
              draw_fg_color(), GTextAlignmentCenter);
    return;
  }

  char row_text[48];
  snprintf(row_text, sizeof(row_text), "%s %s", temp_text, summary);

  const int icon_size = WEATHER_ICON_SMALL_SIZE;
  const int icon_gap = 4;
  const int max_row_width = SCREEN_W - 8;
  const GRect measure_frame = GRect(0, 0, SCREEN_W, 24);
  bool using_large_row_font = true;
  GFont row_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  GSize text_size = graphics_text_layout_get_content_size(
      row_text, row_font, measure_frame,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);

  if (icon_size + icon_gap + text_size.w > max_row_width) {
    using_large_row_font = false;
    row_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    text_size = graphics_text_layout_get_content_size(
        row_text, row_font, measure_frame,
        GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
  }

  int text_width = text_size.w;
  int row_width = icon_size + icon_gap + text_width;
  if (row_width > max_row_width) {
    text_width = max_row_width - icon_size - icon_gap;
    row_width = max_row_width;
  }

  const int row_x = (SCREEN_W - row_width) / 2;
  const int icon_center_y = VERBOSE_WEATHER_CENTER_Y;
  const int text_y = using_large_row_font ? VERBOSE_WEATHER_CENTER_Y - 17
                                          : VERBOSE_WEATHER_CENTER_Y - 13;

  draw_weather_icon_centered(ctx, GPoint(row_x + (icon_size / 2), icon_center_y), false);
  draw_text(ctx, row_text, row_font,
            GRect(row_x + icon_size + icon_gap, text_y, text_width, 32),
            draw_fg_color(), GTextAlignmentLeft);
}

static void face_update_proc(Layer *layer, GContext *ctx) {
  (void)layer;

  set_draw_section(ColorSectionBase);
  graphics_context_set_fill_color(ctx, theme_bg_color());
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  set_draw_section(ColorSectionTopBar);
  fill_inverted_section_background(ctx, ColorSectionTopBar, GRect(0, 0, SCREEN_W, 31));
  draw_watch_battery(ctx);
  if (s_w800_steps_top_enabled) {
    draw_w800_steps_top_bar(ctx);
  }
  draw_bt_icon(ctx, GPoint(181, 7));

  const int content_offset_y = s_verbose_weather_enabled ? VERBOSE_WEATHER_OFFSET_Y : 0;
  const int time_frame_y = TIME_FRAME_Y + content_offset_y;
  const int weather_y = weather_band_y();
  const GRect date_frame = GRect(0, DATE_FRAME_Y + content_offset_y, SCREEN_W, 29);
  const bool verbose_weather_meeting_color_break =
      s_verbose_weather_enabled &&
      section_backgrounds_differ(ColorSectionWeather, ColorSectionMeetingBar);
  const bool large_weather_meeting_color_break =
      s_verbose_weather_large && verbose_weather_meeting_color_break;
  const bool large_weather_layout = s_verbose_weather_enabled && s_verbose_weather_large;
  const bool compact_weather_meeting_inverted =
      !s_verbose_weather_enabled &&
      section_inverted(ColorSectionWeather) &&
      section_inverted(ColorSectionMeetingBar);
  const int meeting_bar_y = large_weather_layout
      ? (large_weather_meeting_color_break ? EVENT_SEPARATOR_Y + 3 : EVENT_SEPARATOR_Y + 1)
      : EVENT_SEPARATOR_Y;
  const int verbose_weather_bottom = large_weather_layout
      ? meeting_bar_y
      : EVENT_SEPARATOR_Y;

  set_draw_section(ColorSectionDateBar);
  fill_inverted_section_background(
      ctx, ColorSectionDateBar,
      GRect(0, 31, SCREEN_W, time_frame_y - 31));
  draw_text(ctx, s_date_buf, s_font_date, date_frame,
            draw_fg_color(), GTextAlignmentCenter);

  set_draw_section(ColorSectionTime);
  fill_inverted_section_background(
      ctx, ColorSectionTime,
      GRect(0, time_frame_y, SCREEN_W, weather_y - time_frame_y));

  if (s_verbose_weather_enabled) {
    draw_time_row_at(ctx, TIME_FRAME_Y + VERBOSE_WEATHER_OFFSET_Y,
                     TIME_VISUAL_BOTTOM + VERBOSE_WEATHER_OFFSET_Y);
    set_draw_section(ColorSectionWeather);
    fill_inverted_section_background(
        ctx, ColorSectionWeather,
        GRect(0, weather_y, SCREEN_W, verbose_weather_bottom - weather_y));
    draw_verbose_weather_row(ctx);
  } else {
    draw_time_row(ctx);

    set_draw_section(ColorSectionWeather);
    const int compact_weather_bottom = compact_weather_meeting_inverted
        ? meeting_bar_y
        : weather_y + weather_band_h();
    fill_inverted_section_background(
        ctx, ColorSectionWeather,
        GRect(0, weather_y, SCREEN_W, compact_weather_bottom - weather_y));
    const int complication_center_y = COMPLICATION_CENTER_Y + 3;
    draw_complication(ctx, GPoint(40, complication_center_y), s_complication_slots[0]);
    draw_complication(ctx, GPoint(100, complication_center_y), s_complication_slots[1]);
    draw_complication(ctx, GPoint(160, complication_center_y), s_complication_slots[2]);
  }

  set_draw_section(ColorSectionMeetingBar);
  fill_inverted_section_background(
      ctx, ColorSectionMeetingBar,
      GRect(0, meeting_bar_y, SCREEN_W, SCREEN_H - meeting_bar_y));
  const bool draw_meeting_separator = s_verbose_weather_enabled
      ? !verbose_weather_meeting_color_break
      : (!section_inverted(ColorSectionMeetingBar) || compact_weather_meeting_inverted);
  if (draw_meeting_separator) {
    graphics_context_set_stroke_color(ctx, draw_fg_color());
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(8, meeting_bar_y), GPoint(192, meeting_bar_y));
  }

  draw_text(ctx, s_event_buf, s_font_event, GRect(8, 203, 184, 25),
            draw_fg_color(), GTextAlignmentCenter);
}

static void update_time_date(struct tm *t) {
  if (s_military_time_enabled) {
    strftime(s_time_buf, sizeof(s_time_buf), "%H:%M", t);
    s_ampm_buf[0] = '\0';
  } else {
    strftime(s_time_buf, sizeof(s_time_buf), "%I:%M", t);
    strftime(s_ampm_buf, sizeof(s_ampm_buf), "%p", t);
  }
  if (s_remove_leading_zero && s_time_buf[0] == '0') {
    memmove(s_time_buf, s_time_buf + 1, strlen(s_time_buf));
  }

  static const char *days[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
  };
  static const char *months[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
  };
  snprintf(s_date_buf, sizeof(s_date_buf), "%s, %s %d",
           days[t->tm_wday], months[t->tm_mon], t->tm_mday);
  mark_face_dirty();
}

static void update_watch_battery(BatteryChargeState s) {
  s_watch_battery_pct = s.charge_percent;
  snprintf(s_watch_buf, sizeof(s_watch_buf), "%d", s.charge_percent);
  format_percent_3(s_watch_battery_buf, sizeof(s_watch_battery_buf), true, s_watch_battery_pct);
  mark_face_dirty();
}

static void update_weather_widget(void) {
  format_int_3(s_temp_buf, sizeof(s_temp_buf), s_temperature_known, s_temperature);
  format_rain_chance(s_rain_buf, sizeof(s_rain_buf), s_rain_known, s_rain_chance);
  format_int_3(s_feels_like_buf, sizeof(s_feels_like_buf), s_feels_like_known, s_feels_like);
  format_int_3(s_high_temp_buf, sizeof(s_high_temp_buf), s_high_temp_known, s_high_temp);
  format_int_3(s_low_temp_buf, sizeof(s_low_temp_buf), s_low_temp_known, s_low_temp);
  format_capped_99_plus(s_wind_buf, sizeof(s_wind_buf), s_wind_known, s_wind_speed);
  if (s_uv_known && s_uv_index > 11) {
    snprintf(s_uv_buf, sizeof(s_uv_buf), "11+");
  } else {
    format_int_3(s_uv_buf, sizeof(s_uv_buf), s_uv_known, s_uv_index);
  }
  mark_face_dirty();
}

static void update_phone_battery_widget(void) {
  format_percent_3(s_phone_battery_buf, sizeof(s_phone_battery_buf),
                   s_phone_battery_known, s_phone_battery_pct);
  mark_face_dirty();
}

static void update_event(const char *title) {
  if (title && title[0] != '\0') {
    strncpy(s_event_buf, title, sizeof(s_event_buf) - 1);
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
  } else {
    strncpy(s_event_buf, "[None]", sizeof(s_event_buf));
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
  }
  mark_face_dirty();
}

static void update_event_delta(const char *delta) {
  if (delta && delta[0] != '\0') {
    strncpy(s_event_delta_buf, delta, sizeof(s_event_delta_buf) - 1);
    s_event_delta_buf[sizeof(s_event_delta_buf) - 1] = '\0';
  } else {
    format_unknown(s_event_delta_buf, sizeof(s_event_delta_buf));
  }
  mark_face_dirty();
}

static void store_overlay_string(char *dest, size_t dest_len, uint32_t persist_key,
                                 const char *value) {
  if (!value) {
    value = "";
  }
  strncpy(dest, value, dest_len - 1);
  dest[dest_len - 1] = '\0';
  persist_write_string(persist_key, dest);
}

static void update_stats(void) {
#if defined(PBL_HEALTH)
  int steps = (int) health_service_sum_today(HealthMetricStepCount);
  HealthValue bpm = health_service_peek_current_value(HealthMetricHeartRateBPM);

  if (steps < 0) steps = 0;
  s_steps_count = steps;
  if (steps < 1000) {
    snprintf(s_steps_buf, sizeof(s_steps_buf), "%d", steps);
  } else if (steps < 100000) {
    snprintf(s_steps_buf, sizeof(s_steps_buf), "%dk", steps / 1000);
  } else {
    snprintf(s_steps_buf, sizeof(s_steps_buf), "99k");
  }

  if (bpm > 0) {
    snprintf(s_bpm_buf, sizeof(s_bpm_buf), "%ld", (long)bpm);
    s_last_hr_sample_time = time(NULL);
  } else {
    snprintf(s_bpm_buf, sizeof(s_bpm_buf), "--");
  }
#else
  s_steps_count = 0;
  snprintf(s_steps_buf, sizeof(s_steps_buf), "--");
  snprintf(s_bpm_buf, sizeof(s_bpm_buf), "--");
#endif
  mark_face_dirty();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  (void)units_changed;
  update_time_date(tick_time);
  update_stats();
}

static void battery_handler(BatteryChargeState charge) {
  update_watch_battery(charge);
}

static void connection_handler(bool connected) {
  s_phone_connected = connected;
  if (!connected) {
    s_phone_battery_known = false;
    update_phone_battery_widget();
  }
  mark_face_dirty();
}

static void health_handler(HealthEventType event, void *context) {
  (void)event;
  (void)context;
  update_stats();
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  (void)context;
  Tuple *t;
  bool fitness_settings_changed = false;

  t = dict_find(iter, MESSAGE_KEY_NEXT_EVENT);
  if (t && t->type == TUPLE_CSTRING) {
    persist_write_string(PERSIST_KEY_EVENT, t->value->cstring);
    update_event(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_PHONE_BATTERY);
  if (t) {
    s_phone_battery_pct = (uint8_t)t->value->int32;
    s_phone_battery_known = true;
    update_phone_battery_widget();
    persist_write_int(PERSIST_KEY_PHONE_BATTERY, s_phone_battery_pct);
  }

  t = dict_find(iter, MESSAGE_KEY_WEATHER_CODE);
  if (t) {
    s_weather_code = (uint8_t)t->value->int32;
    s_weather_known = true;
    persist_write_int(PERSIST_KEY_WEATHER_CODE, s_weather_code);
  }

  t = dict_find(iter, MESSAGE_KEY_TEMPERATURE);
  if (t) {
    s_temperature = (int8_t)t->value->int32;
    s_temperature_known = true;
    persist_write_int(PERSIST_KEY_TEMPERATURE, s_temperature);
  }

  t = dict_find(iter, MESSAGE_KEY_RAIN_CHANCE);
  if (t) {
    s_rain_chance = (uint8_t)t->value->int32;
    s_rain_known = true;
    persist_write_int(PERSIST_KEY_RAIN_CHANCE, s_rain_chance);
  }

  t = dict_find(iter, MESSAGE_KEY_FEELS_LIKE);
  if (t) {
    s_feels_like = (int8_t)t->value->int32;
    s_feels_like_known = true;
    persist_write_int(PERSIST_KEY_FEELS_LIKE, s_feels_like);
  }

  t = dict_find(iter, MESSAGE_KEY_HIGH_TEMP);
  if (t) {
    s_high_temp = (int8_t)t->value->int32;
    s_high_temp_known = true;
    persist_write_int(PERSIST_KEY_HIGH_TEMP, s_high_temp);
  }

  t = dict_find(iter, MESSAGE_KEY_LOW_TEMP);
  if (t) {
    s_low_temp = (int8_t)t->value->int32;
    s_low_temp_known = true;
    persist_write_int(PERSIST_KEY_LOW_TEMP, s_low_temp);
  }

  t = dict_find(iter, MESSAGE_KEY_WIND_SPEED);
  if (t) {
    s_wind_speed = (uint8_t)t->value->int32;
    s_wind_known = true;
    persist_write_int(PERSIST_KEY_WIND_SPEED, s_wind_speed);
  }

  t = dict_find(iter, MESSAGE_KEY_UV_INDEX);
  if (t) {
    s_uv_index = (uint8_t)t->value->int32;
    s_uv_known = true;
    persist_write_int(PERSIST_KEY_UV_INDEX, s_uv_index);
  }

  t = dict_find(iter, MESSAGE_KEY_SUNRISE_T);
  if (t) {
    s_sunrise_t = (time_t)t->value->int32;
    s_sunrise_known = s_sunrise_t > 0;
    persist_write_int(PERSIST_KEY_SUNRISE_T, (int)s_sunrise_t);
  }

  t = dict_find(iter, MESSAGE_KEY_SUNSET_T);
  if (t) {
    s_sunset_t = (time_t)t->value->int32;
    s_sunset_known = s_sunset_t > 0;
    persist_write_int(PERSIST_KEY_SUNSET_T, (int)s_sunset_t);
  }

  t = dict_find(iter, MESSAGE_KEY_NEXT_EVENT_DELTA);
  if (t && t->type == TUPLE_CSTRING) {
    persist_write_string(PERSIST_KEY_EVENT_DELTA, t->value->cstring);
    update_event_delta(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_TITLE_2);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_titles[0], sizeof(s_calendar_event_titles[0]),
                         PERSIST_KEY_CALENDAR_EVENT_TITLE_2, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_DELTA_2);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_deltas[0], sizeof(s_calendar_event_deltas[0]),
                         PERSIST_KEY_CALENDAR_EVENT_DELTA_2, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_TITLE_3);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_titles[1], sizeof(s_calendar_event_titles[1]),
                         PERSIST_KEY_CALENDAR_EVENT_TITLE_3, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_DELTA_3);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_deltas[1], sizeof(s_calendar_event_deltas[1]),
                         PERSIST_KEY_CALENDAR_EVENT_DELTA_3, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_TITLE_4);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_titles[2], sizeof(s_calendar_event_titles[2]),
                         PERSIST_KEY_CALENDAR_EVENT_TITLE_4, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_DELTA_4);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_deltas[2], sizeof(s_calendar_event_deltas[2]),
                         PERSIST_KEY_CALENDAR_EVENT_DELTA_4, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_TITLE_5);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_titles[3], sizeof(s_calendar_event_titles[3]),
                         PERSIST_KEY_CALENDAR_EVENT_TITLE_5, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_EVENT_DELTA_5);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_calendar_event_deltas[3], sizeof(s_calendar_event_deltas[3]),
                         PERSIST_KEY_CALENDAR_EVENT_DELTA_5, t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_LIGHT_MODE);
  if (t) {
    bool light_mode_enabled = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_LIGHT_MODE, light_mode_enabled);
    apply_light_mode(light_mode_enabled);
  }

  t = dict_find(iter, MESSAGE_KEY_INVERT_TOP_BAR);
  if (t) {
    s_invert_top_bar = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_INVERT_TOP_BAR, s_invert_top_bar);
  }

  t = dict_find(iter, MESSAGE_KEY_INVERT_DATE_BAR);
  if (t) {
    s_invert_date_bar = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_INVERT_DATE_BAR, s_invert_date_bar);
  }

  t = dict_find(iter, MESSAGE_KEY_INVERT_TIME);
  if (t) {
    s_invert_time = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_INVERT_TIME, s_invert_time);
  }

  t = dict_find(iter, MESSAGE_KEY_MILITARY_TIME);
  if (t) {
    s_military_time_enabled = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_MILITARY_TIME, s_military_time_enabled);
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    update_time_date(current_time);
  }

  t = dict_find(iter, MESSAGE_KEY_REMOVE_LEADING_ZERO);
  if (t) {
    s_remove_leading_zero = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_REMOVE_LEADING_ZERO, s_remove_leading_zero);
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    update_time_date(current_time);
  }

  t = dict_find(iter, MESSAGE_KEY_INVERT_WEATHER);
  if (t) {
    s_invert_weather = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_INVERT_WEATHER, s_invert_weather);
  }

  t = dict_find(iter, MESSAGE_KEY_INVERT_MEETING_BAR);
  if (t) {
    s_invert_meeting_bar = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_INVERT_MEETING_BAR, s_invert_meeting_bar);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_MODE);
  if (t) {
    s_color_mode = t->value->int32 == 1 ? ColorModeColor : ColorModeBW;
    persist_write_int(PERSIST_KEY_COLOR_MODE, s_color_mode);
    mark_face_dirty();
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_BG_TOP_BAR);
  if (t) {
    store_color_section_value(s_color_section_bg, ColorSectionTopBar,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_BG_TOP_BAR);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_FG_TOP_BAR);
  if (t) {
    store_color_section_value(s_color_section_fg, ColorSectionTopBar,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_FG_TOP_BAR);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_BG_DATE_BAR);
  if (t) {
    store_color_section_value(s_color_section_bg, ColorSectionDateBar,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_BG_DATE_BAR);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_FG_DATE_BAR);
  if (t) {
    store_color_section_value(s_color_section_fg, ColorSectionDateBar,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_FG_DATE_BAR);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_BG_TIME);
  if (t) {
    store_color_section_value(s_color_section_bg, ColorSectionTime,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_BG_TIME);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_FG_TIME);
  if (t) {
    store_color_section_value(s_color_section_fg, ColorSectionTime,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_FG_TIME);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_BG_WEATHER);
  if (t) {
    store_color_section_value(s_color_section_bg, ColorSectionWeather,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_BG_WEATHER);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_FG_WEATHER);
  if (t) {
    store_color_section_value(s_color_section_fg, ColorSectionWeather,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_FG_WEATHER);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_BG_MEETING_BAR);
  if (t) {
    store_color_section_value(s_color_section_bg, ColorSectionMeetingBar,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_BG_MEETING_BAR);
  }

  t = dict_find(iter, MESSAGE_KEY_COLOR_SECTION_FG_MEETING_BAR);
  if (t) {
    store_color_section_value(s_color_section_fg, ColorSectionMeetingBar,
                              (int)t->value->int32,
                              PERSIST_KEY_COLOR_SECTION_FG_MEETING_BAR);
  }

  t = dict_find(iter, MESSAGE_KEY_TOP_STEPS);
  if (t) {
    s_w800_steps_top_enabled = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_TOP_STEPS, s_w800_steps_top_enabled);
  }

  t = dict_find(iter, MESSAGE_KEY_TEMPERATURE_UNIT);
  if (t) {
    s_temperature_unit_celsius = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_TEMP_UNIT, s_temperature_unit_celsius);
  }

  t = dict_find(iter, MESSAGE_KEY_VERBOSE_WEATHER);
  if (t) {
    s_verbose_weather_enabled = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_VERBOSE_WEATHER, s_verbose_weather_enabled);
  }

  t = dict_find(iter, MESSAGE_KEY_VERBOSE_WEATHER_STYLE);
  if (t) {
    s_verbose_weather_large = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_VERBOSE_WEATHER_STYLE, s_verbose_weather_large);
  }

  t = dict_find(iter, MESSAGE_KEY_WEATHER_SUMMARY);
  if (t && t->type == TUPLE_CSTRING) {
    strncpy(s_weather_summary_buf, t->value->cstring, sizeof(s_weather_summary_buf) - 1);
    s_weather_summary_buf[sizeof(s_weather_summary_buf) - 1] = '\0';
    persist_write_string(PERSIST_KEY_WEATHER_SUMMARY, s_weather_summary_buf);
  }

  t = dict_find(iter, MESSAGE_KEY_COMPLICATION_1);
  if (t) {
    s_complication_slots[0] = sanitize_complication(t->value->int32, ComplicationTemperature);
    persist_write_int(PERSIST_KEY_COMP_SLOT_1, s_complication_slots[0]);
  }

  t = dict_find(iter, MESSAGE_KEY_COMPLICATION_2);
  if (t) {
    s_complication_slots[1] = sanitize_complication(t->value->int32, ComplicationRainChance);
    persist_write_int(PERSIST_KEY_COMP_SLOT_2, s_complication_slots[1]);
  }

  t = dict_find(iter, MESSAGE_KEY_COMPLICATION_3);
  if (t) {
    s_complication_slots[2] = sanitize_complication(t->value->int32, ComplicationHeartRate);
    persist_write_int(PERSIST_KEY_COMP_SLOT_3, s_complication_slots[2]);
  }

  t = dict_find(iter, MESSAGE_KEY_SHAKE_BEHAVIOR);
  if (t) {
    s_shake_behavior = sanitize_shake_behavior(t->value->int32);
    persist_write_int(PERSIST_KEY_SHAKE_BEHAVIOR, s_shake_behavior);
    if (s_shake_behavior == ShakeBehaviorOff) {
      shake_hide_overlay(true);
    }
    shake_configure_tap_service();
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_RING_STEPS_ON);
  if (t) {
    s_fitness_ring_steps_on = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_FITNESS_RING_STEPS_ON, s_fitness_ring_steps_on);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_RING_ACTIVE_ON);
  if (t) {
    s_fitness_ring_active_on = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_FITNESS_RING_ACTIVE_ON, s_fitness_ring_active_on);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_RING_CALORIES_ON);
  if (t) {
    s_fitness_ring_calories_on = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_FITNESS_RING_CALORIES_ON, s_fitness_ring_calories_on);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_TARGET_STEPS);
  if (t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Steps target received: %ld", (long)t->value->int32);
    s_fitness_target_steps =
        fitness_sanitize_target(t->value->int32, FITNESS_DEFAULT_TARGET_STEPS);
    persist_write_int(PERSIST_KEY_FITNESS_TARGET_STEPS, s_fitness_target_steps);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_TARGET_ACTIVE_MIN);
  if (t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Active min target received: %ld", (long)t->value->int32);
    s_fitness_target_active_min =
        fitness_sanitize_target(t->value->int32, FITNESS_DEFAULT_TARGET_ACTIVE_MIN);
    persist_write_int(PERSIST_KEY_FITNESS_TARGET_ACTIVE_MIN, s_fitness_target_active_min);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_TARGET_CALORIES);
  if (t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Calories target received: %ld", (long)t->value->int32);
    s_fitness_target_calories =
        fitness_sanitize_target(t->value->int32, FITNESS_DEFAULT_TARGET_CALORIES);
    persist_write_int(PERSIST_KEY_FITNESS_TARGET_CALORIES, s_fitness_target_calories);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_COLOR_STEPS);
  if (t) {
    s_fitness_color_steps_hex =
        fitness_sanitize_color(t->value->int32, FITNESS_DEFAULT_COLOR_STEPS);
    persist_write_int(PERSIST_KEY_FITNESS_COLOR_STEPS, s_fitness_color_steps_hex);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_COLOR_ACTIVE);
  if (t) {
    s_fitness_color_active_hex =
        fitness_sanitize_color(t->value->int32, FITNESS_DEFAULT_COLOR_ACTIVE);
    persist_write_int(PERSIST_KEY_FITNESS_COLOR_ACTIVE, s_fitness_color_active_hex);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_COLOR_CALORIES);
  if (t) {
    s_fitness_color_calories_hex =
        fitness_sanitize_color(t->value->int32, FITNESS_DEFAULT_COLOR_CALORIES);
    persist_write_int(PERSIST_KEY_FITNESS_COLOR_CALORIES, s_fitness_color_calories_hex);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FITNESS_OVERLAY_DURATION_S);
  if (t) {
    s_fitness_overlay_duration_ms =
        fitness_overlay_duration_ms_from_seconds((int)t->value->int32);
    persist_write_int(PERSIST_KEY_FITNESS_OVERLAY_DURATION_S,
                      s_fitness_overlay_duration_ms / 1000);
    if (s_shake_overlay_visible) {
      shake_schedule_hide_timer();
    }
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_CALENDAR_SHAKE_EVENT_COUNT);
  if (t) {
    int count = (int)t->value->int32;
    s_calendar_shake_event_count = count == 5 ? 5 : 3;
    persist_write_int(PERSIST_KEY_CALENDAR_SHAKE_EVENT_COUNT, s_calendar_shake_event_count);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HOURS_BITMAP);
  if (t) {
    s_day_event_hours_bitmap = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP, s_day_event_hours_bitmap);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HOURS_BITMAP_TOMORROW);
  if (t) {
    s_day_event_hours_bitmap_tomorrow = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_TOMORROW,
                      s_day_event_hours_bitmap_tomorrow);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HOURS_BITMAP_YESTERDAY);
  if (t) {
    s_day_event_hours_bitmap_yesterday = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_YESTERDAY,
                      s_day_event_hours_bitmap_yesterday);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_COUNT_TODAY);
  if (t) {
    s_day_event_count_today = (int)t->value->int32;
    if (s_day_event_count_today < 0) {
      s_day_event_count_today = 0;
    }
    persist_write_int(PERSIST_KEY_DAY_EVENT_COUNT_TODAY, s_day_event_count_today);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_YOUR_DAY_WINDOW_MODE);
  if (t) {
    s_your_day_window_mode = sanitize_your_day_window_mode((int)t->value->int32);
    persist_write_int(PERSIST_KEY_YOUR_DAY_WINDOW_MODE, s_your_day_window_mode);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_YOUR_DAY_WINDOW_HOURS);
  if (t) {
    s_your_day_window_hours = sanitize_your_day_window_hours((int)t->value->int32);
    persist_write_int(PERSIST_KEY_YOUR_DAY_WINDOW_HOURS, s_your_day_window_hours);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_YOUR_DAY_START_HOUR);
  if (t) {
    s_your_day_start_hour = sanitize_your_day_hour((int)t->value->int32);
    persist_write_int(PERSIST_KEY_YOUR_DAY_START_HOUR, s_your_day_start_hour);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_YOUR_DAY_END_HOUR);
  if (t) {
    s_your_day_end_hour = sanitize_your_day_hour((int)t->value->int32);
    persist_write_int(PERSIST_KEY_YOUR_DAY_END_HOUR, s_your_day_end_hour);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_ALT_TZ_LABEL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_alt_tz_label, sizeof(s_alt_tz_label),
                         PERSIST_KEY_ALT_TZ_LABEL, t->value->cstring);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_ALT_TZ_OFFSET_MIN);
  if (t) {
    s_alt_tz_offset_min = (int)t->value->int32;
    if (s_alt_tz_offset_min < -720) {
      s_alt_tz_offset_min = -720;
    } else if (s_alt_tz_offset_min > 840) {
      s_alt_tz_offset_min = 840;
    }
    persist_write_int(PERSIST_KEY_ALT_TZ_OFFSET_MIN, s_alt_tz_offset_min);
    fitness_settings_changed = true;
  }

  if (fitness_settings_changed && s_shake_overlay_visible && s_shake_overlay_layer) {
    layer_mark_dirty(s_shake_overlay_layer);
  }

  update_weather_widget();
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  (void)context;
  APP_LOG(APP_LOG_LEVEL_WARNING, "AppMessage inbox dropped: %d", (int)reason);
}

static void load_persisted(void) {
  if (persist_exists(PERSIST_KEY_PHONE_BATTERY)) {
    s_phone_battery_pct = (uint8_t)persist_read_int(PERSIST_KEY_PHONE_BATTERY);
    s_phone_battery_known = true;
  }
  if (persist_exists(PERSIST_KEY_WEATHER_CODE)) {
    s_weather_code = (uint8_t)persist_read_int(PERSIST_KEY_WEATHER_CODE);
    s_weather_known = true;
  }
  if (persist_exists(PERSIST_KEY_TEMPERATURE)) {
    s_temperature = (int8_t)persist_read_int(PERSIST_KEY_TEMPERATURE);
    s_temperature_known = true;
  }
  if (persist_exists(PERSIST_KEY_RAIN_CHANCE)) {
    s_rain_chance = (uint8_t)persist_read_int(PERSIST_KEY_RAIN_CHANCE);
    s_rain_known = true;
  }
  if (persist_exists(PERSIST_KEY_FEELS_LIKE)) {
    s_feels_like = (int8_t)persist_read_int(PERSIST_KEY_FEELS_LIKE);
    s_feels_like_known = true;
  }
  if (persist_exists(PERSIST_KEY_HIGH_TEMP)) {
    s_high_temp = (int8_t)persist_read_int(PERSIST_KEY_HIGH_TEMP);
    s_high_temp_known = true;
  }
  if (persist_exists(PERSIST_KEY_LOW_TEMP)) {
    s_low_temp = (int8_t)persist_read_int(PERSIST_KEY_LOW_TEMP);
    s_low_temp_known = true;
  }
  if (persist_exists(PERSIST_KEY_WIND_SPEED)) {
    s_wind_speed = (uint8_t)persist_read_int(PERSIST_KEY_WIND_SPEED);
    s_wind_known = true;
  }
  if (persist_exists(PERSIST_KEY_UV_INDEX)) {
    s_uv_index = (uint8_t)persist_read_int(PERSIST_KEY_UV_INDEX);
    s_uv_known = true;
  }
  if (persist_exists(PERSIST_KEY_SUNRISE_T)) {
    s_sunrise_t = (time_t)persist_read_int(PERSIST_KEY_SUNRISE_T);
    s_sunrise_known = s_sunrise_t > 0;
  }
  if (persist_exists(PERSIST_KEY_SUNSET_T)) {
    s_sunset_t = (time_t)persist_read_int(PERSIST_KEY_SUNSET_T);
    s_sunset_known = s_sunset_t > 0;
  }
  if (persist_exists(PERSIST_KEY_TOP_STEPS)) {
    s_w800_steps_top_enabled = persist_read_bool(PERSIST_KEY_TOP_STEPS);
  }
  if (persist_exists(PERSIST_KEY_TEMP_UNIT)) {
    s_temperature_unit_celsius = persist_read_bool(PERSIST_KEY_TEMP_UNIT);
  }
  if (persist_exists(PERSIST_KEY_VERBOSE_WEATHER)) {
    s_verbose_weather_enabled = persist_read_bool(PERSIST_KEY_VERBOSE_WEATHER);
  }
  if (persist_exists(PERSIST_KEY_VERBOSE_WEATHER_STYLE)) {
    s_verbose_weather_large = persist_read_bool(PERSIST_KEY_VERBOSE_WEATHER_STYLE);
  }
  if (persist_exists(PERSIST_KEY_LIGHT_MODE)) {
    s_light_mode_enabled = persist_read_bool(PERSIST_KEY_LIGHT_MODE);
  }
  if (persist_exists(PERSIST_KEY_INVERT_TOP_BAR)) {
    s_invert_top_bar = persist_read_bool(PERSIST_KEY_INVERT_TOP_BAR);
  }
  if (persist_exists(PERSIST_KEY_INVERT_DATE_BAR)) {
    s_invert_date_bar = persist_read_bool(PERSIST_KEY_INVERT_DATE_BAR);
  }
  if (persist_exists(PERSIST_KEY_INVERT_TIME)) {
    s_invert_time = persist_read_bool(PERSIST_KEY_INVERT_TIME);
  }
  if (persist_exists(PERSIST_KEY_MILITARY_TIME)) {
    s_military_time_enabled = persist_read_bool(PERSIST_KEY_MILITARY_TIME);
  }
  if (persist_exists(PERSIST_KEY_REMOVE_LEADING_ZERO)) {
    s_remove_leading_zero = persist_read_bool(PERSIST_KEY_REMOVE_LEADING_ZERO);
  }
  if (persist_exists(PERSIST_KEY_INVERT_WEATHER)) {
    s_invert_weather = persist_read_bool(PERSIST_KEY_INVERT_WEATHER);
  }
  if (persist_exists(PERSIST_KEY_INVERT_MEETING_BAR)) {
    s_invert_meeting_bar = persist_read_bool(PERSIST_KEY_INVERT_MEETING_BAR);
  }
  if (persist_exists(PERSIST_KEY_COLOR_MODE)) {
    s_color_mode = persist_read_int(PERSIST_KEY_COLOR_MODE) == 1
        ? ColorModeColor
        : ColorModeBW;
  }
  load_color_section_value(s_color_section_bg, ColorSectionTopBar,
                           PERSIST_KEY_COLOR_SECTION_BG_TOP_BAR);
  load_color_section_value(s_color_section_fg, ColorSectionTopBar,
                           PERSIST_KEY_COLOR_SECTION_FG_TOP_BAR);
  load_color_section_value(s_color_section_bg, ColorSectionDateBar,
                           PERSIST_KEY_COLOR_SECTION_BG_DATE_BAR);
  load_color_section_value(s_color_section_fg, ColorSectionDateBar,
                           PERSIST_KEY_COLOR_SECTION_FG_DATE_BAR);
  load_color_section_value(s_color_section_bg, ColorSectionTime,
                           PERSIST_KEY_COLOR_SECTION_BG_TIME);
  load_color_section_value(s_color_section_fg, ColorSectionTime,
                           PERSIST_KEY_COLOR_SECTION_FG_TIME);
  load_color_section_value(s_color_section_bg, ColorSectionWeather,
                           PERSIST_KEY_COLOR_SECTION_BG_WEATHER);
  load_color_section_value(s_color_section_fg, ColorSectionWeather,
                           PERSIST_KEY_COLOR_SECTION_FG_WEATHER);
  load_color_section_value(s_color_section_bg, ColorSectionMeetingBar,
                           PERSIST_KEY_COLOR_SECTION_BG_MEETING_BAR);
  load_color_section_value(s_color_section_fg, ColorSectionMeetingBar,
                           PERSIST_KEY_COLOR_SECTION_FG_MEETING_BAR);
  if (persist_exists(PERSIST_KEY_EVENT)) {
    persist_read_string(PERSIST_KEY_EVENT, s_event_buf, sizeof(s_event_buf));
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_WEATHER_SUMMARY)) {
    persist_read_string(PERSIST_KEY_WEATHER_SUMMARY, s_weather_summary_buf,
                        sizeof(s_weather_summary_buf));
    s_weather_summary_buf[sizeof(s_weather_summary_buf) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_EVENT_DELTA)) {
    persist_read_string(PERSIST_KEY_EVENT_DELTA, s_event_delta_buf, sizeof(s_event_delta_buf));
    s_event_delta_buf[sizeof(s_event_delta_buf) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_TITLE_2)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_TITLE_2, s_calendar_event_titles[0],
                        sizeof(s_calendar_event_titles[0]));
    s_calendar_event_titles[0][sizeof(s_calendar_event_titles[0]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_DELTA_2)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_DELTA_2, s_calendar_event_deltas[0],
                        sizeof(s_calendar_event_deltas[0]));
    s_calendar_event_deltas[0][sizeof(s_calendar_event_deltas[0]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_TITLE_3)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_TITLE_3, s_calendar_event_titles[1],
                        sizeof(s_calendar_event_titles[1]));
    s_calendar_event_titles[1][sizeof(s_calendar_event_titles[1]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_DELTA_3)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_DELTA_3, s_calendar_event_deltas[1],
                        sizeof(s_calendar_event_deltas[1]));
    s_calendar_event_deltas[1][sizeof(s_calendar_event_deltas[1]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_TITLE_4)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_TITLE_4, s_calendar_event_titles[2],
                        sizeof(s_calendar_event_titles[2]));
    s_calendar_event_titles[2][sizeof(s_calendar_event_titles[2]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_DELTA_4)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_DELTA_4, s_calendar_event_deltas[2],
                        sizeof(s_calendar_event_deltas[2]));
    s_calendar_event_deltas[2][sizeof(s_calendar_event_deltas[2]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_TITLE_5)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_TITLE_5, s_calendar_event_titles[3],
                        sizeof(s_calendar_event_titles[3]));
    s_calendar_event_titles[3][sizeof(s_calendar_event_titles[3]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_EVENT_DELTA_5)) {
    persist_read_string(PERSIST_KEY_CALENDAR_EVENT_DELTA_5, s_calendar_event_deltas[3],
                        sizeof(s_calendar_event_deltas[3]));
    s_calendar_event_deltas[3][sizeof(s_calendar_event_deltas[3]) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_COMP_SLOT_1)) {
    s_complication_slots[0] = sanitize_complication(
        persist_read_int(PERSIST_KEY_COMP_SLOT_1), ComplicationWeather);
  }
  if (persist_exists(PERSIST_KEY_COMP_SLOT_2)) {
    s_complication_slots[1] = sanitize_complication(
        persist_read_int(PERSIST_KEY_COMP_SLOT_2), ComplicationRainChance);
  }
  if (persist_exists(PERSIST_KEY_COMP_SLOT_3)) {
    s_complication_slots[2] = sanitize_complication(
        persist_read_int(PERSIST_KEY_COMP_SLOT_3), ComplicationHeartRate);
  }
  if (persist_exists(PERSIST_KEY_SHAKE_BEHAVIOR)) {
    s_shake_behavior = sanitize_shake_behavior(persist_read_int(PERSIST_KEY_SHAKE_BEHAVIOR));
  }
  if (persist_exists(PERSIST_KEY_FITNESS_RING_STEPS_ON)) {
    s_fitness_ring_steps_on = persist_read_bool(PERSIST_KEY_FITNESS_RING_STEPS_ON);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_RING_ACTIVE_ON)) {
    s_fitness_ring_active_on = persist_read_bool(PERSIST_KEY_FITNESS_RING_ACTIVE_ON);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_RING_CALORIES_ON)) {
    s_fitness_ring_calories_on = persist_read_bool(PERSIST_KEY_FITNESS_RING_CALORIES_ON);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_TARGET_STEPS)) {
    s_fitness_target_steps = fitness_sanitize_target(
        persist_read_int(PERSIST_KEY_FITNESS_TARGET_STEPS), FITNESS_DEFAULT_TARGET_STEPS);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_TARGET_ACTIVE_MIN)) {
    s_fitness_target_active_min = fitness_sanitize_target(
        persist_read_int(PERSIST_KEY_FITNESS_TARGET_ACTIVE_MIN), FITNESS_DEFAULT_TARGET_ACTIVE_MIN);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_TARGET_CALORIES)) {
    s_fitness_target_calories = fitness_sanitize_target(
        persist_read_int(PERSIST_KEY_FITNESS_TARGET_CALORIES), FITNESS_DEFAULT_TARGET_CALORIES);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_COLOR_STEPS)) {
    s_fitness_color_steps_hex = fitness_sanitize_color(
        persist_read_int(PERSIST_KEY_FITNESS_COLOR_STEPS), FITNESS_DEFAULT_COLOR_STEPS);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_COLOR_ACTIVE)) {
    s_fitness_color_active_hex = fitness_sanitize_color(
        persist_read_int(PERSIST_KEY_FITNESS_COLOR_ACTIVE), FITNESS_DEFAULT_COLOR_ACTIVE);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_COLOR_CALORIES)) {
    s_fitness_color_calories_hex = fitness_sanitize_color(
        persist_read_int(PERSIST_KEY_FITNESS_COLOR_CALORIES), FITNESS_DEFAULT_COLOR_CALORIES);
  }
  if (persist_exists(PERSIST_KEY_FITNESS_OVERLAY_DURATION_S)) {
    s_fitness_overlay_duration_ms = fitness_overlay_duration_ms_from_seconds(
        persist_read_int(PERSIST_KEY_FITNESS_OVERLAY_DURATION_S));
  }
  if (persist_exists(PERSIST_KEY_CALENDAR_SHAKE_EVENT_COUNT)) {
    s_calendar_shake_event_count =
        persist_read_int(PERSIST_KEY_CALENDAR_SHAKE_EVENT_COUNT) == 5 ? 5 : 3;
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP)) {
    s_day_event_hours_bitmap = persist_read_int(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_TOMORROW)) {
    s_day_event_hours_bitmap_tomorrow =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_TOMORROW);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_YESTERDAY)) {
    s_day_event_hours_bitmap_yesterday =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HOURS_BITMAP_YESTERDAY);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_COUNT_TODAY)) {
    s_day_event_count_today = persist_read_int(PERSIST_KEY_DAY_EVENT_COUNT_TODAY);
    if (s_day_event_count_today < 0) {
      s_day_event_count_today = 0;
    }
  }
  if (persist_exists(PERSIST_KEY_YOUR_DAY_WINDOW_MODE)) {
    s_your_day_window_mode =
        sanitize_your_day_window_mode(persist_read_int(PERSIST_KEY_YOUR_DAY_WINDOW_MODE));
  }
  if (persist_exists(PERSIST_KEY_YOUR_DAY_WINDOW_HOURS)) {
    s_your_day_window_hours =
        sanitize_your_day_window_hours(persist_read_int(PERSIST_KEY_YOUR_DAY_WINDOW_HOURS));
  }
  if (persist_exists(PERSIST_KEY_YOUR_DAY_START_HOUR)) {
    s_your_day_start_hour =
        sanitize_your_day_hour(persist_read_int(PERSIST_KEY_YOUR_DAY_START_HOUR));
  }
  if (persist_exists(PERSIST_KEY_YOUR_DAY_END_HOUR)) {
    s_your_day_end_hour =
        sanitize_your_day_hour(persist_read_int(PERSIST_KEY_YOUR_DAY_END_HOUR));
  }
  if (persist_exists(PERSIST_KEY_ALT_TZ_LABEL)) {
    persist_read_string(PERSIST_KEY_ALT_TZ_LABEL, s_alt_tz_label, sizeof(s_alt_tz_label));
    s_alt_tz_label[sizeof(s_alt_tz_label) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_ALT_TZ_OFFSET_MIN)) {
    s_alt_tz_offset_min = persist_read_int(PERSIST_KEY_ALT_TZ_OFFSET_MIN);
    if (s_alt_tz_offset_min < -720) {
      s_alt_tz_offset_min = -720;
    } else if (s_alt_tz_offset_min > 840) {
      s_alt_tz_offset_min = 840;
    }
  }
}

static uint32_t resource_id_for_bitmap_theme(BitmapTheme theme,
                                             uint32_t dark_resource,
                                             uint32_t light_resource) {
  return theme == BitmapThemeLight ? light_resource : dark_resource;
}

static void load_weather_icon_bitmaps_for_theme(BitmapTheme theme) {
  s_weather_icon_small_bitmaps[theme][WeatherIconSunny] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_SUNNY_SMALL, RESOURCE_ID_WEATHER_SUNNY_SMALL_LIGHT));
  s_weather_icon_small_bitmaps[theme][WeatherIconPartlyCloudy] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_PARTLY_CLOUDY_SMALL,
          RESOURCE_ID_WEATHER_PARTLY_CLOUDY_SMALL_LIGHT));
  s_weather_icon_small_bitmaps[theme][WeatherIconCloudy] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_CLOUDY_SMALL, RESOURCE_ID_WEATHER_CLOUDY_SMALL_LIGHT));
  s_weather_icon_small_bitmaps[theme][WeatherIconFog] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_FOG_SMALL, RESOURCE_ID_WEATHER_FOG_SMALL_LIGHT));
  s_weather_icon_small_bitmaps[theme][WeatherIconRain] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_RAIN_SMALL, RESOURCE_ID_WEATHER_RAIN_SMALL_LIGHT));
  s_weather_icon_small_bitmaps[theme][WeatherIconSnow] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_SNOW_SMALL, RESOURCE_ID_WEATHER_SNOW_SMALL_LIGHT));
  s_weather_icon_small_bitmaps[theme][WeatherIconStorm] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_STORM_SMALL, RESOURCE_ID_WEATHER_STORM_SMALL_LIGHT));

  s_weather_icon_large_bitmaps[theme][WeatherIconSunny] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_SUNNY_LARGE, RESOURCE_ID_WEATHER_SUNNY_LARGE_LIGHT));
  s_weather_icon_large_bitmaps[theme][WeatherIconPartlyCloudy] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_PARTLY_CLOUDY_LARGE,
          RESOURCE_ID_WEATHER_PARTLY_CLOUDY_LARGE_LIGHT));
  s_weather_icon_large_bitmaps[theme][WeatherIconCloudy] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_CLOUDY_LARGE, RESOURCE_ID_WEATHER_CLOUDY_LARGE_LIGHT));
  s_weather_icon_large_bitmaps[theme][WeatherIconFog] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_FOG_LARGE, RESOURCE_ID_WEATHER_FOG_LARGE_LIGHT));
  s_weather_icon_large_bitmaps[theme][WeatherIconRain] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_RAIN_LARGE, RESOURCE_ID_WEATHER_RAIN_LARGE_LIGHT));
  s_weather_icon_large_bitmaps[theme][WeatherIconSnow] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_SNOW_LARGE, RESOURCE_ID_WEATHER_SNOW_LARGE_LIGHT));
  s_weather_icon_large_bitmaps[theme][WeatherIconStorm] =
      gbitmap_create_with_resource(resource_id_for_bitmap_theme(
          theme, RESOURCE_ID_WEATHER_STORM_LARGE, RESOURCE_ID_WEATHER_STORM_LARGE_LIGHT));
}

static void load_weather_icon_bitmaps(void) {
  load_weather_icon_bitmaps_for_theme(BitmapThemeDark);
  load_weather_icon_bitmaps_for_theme(BitmapThemeLight);
}

static void destroy_bitmap(GBitmap **bitmap) {
  if (*bitmap) {
    gbitmap_destroy(*bitmap);
    *bitmap = NULL;
  }
}

static void destroy_weather_icon_bitmaps(void) {
  for (int theme = 0; theme < BitmapThemeCount; theme++) {
    for (int i = 0; i < WeatherIconCount; i++) {
      destroy_bitmap(&s_weather_icon_small_bitmaps[theme][i]);
      destroy_bitmap(&s_weather_icon_large_bitmaps[theme][i]);
    }
  }
}

static void load_theme_bitmaps(void) {
  s_step_boot_bitmaps[BitmapThemeDark] =
      gbitmap_create_with_resource(RESOURCE_ID_STEP_BOOT);
  s_step_boot_bitmaps[BitmapThemeLight] =
      gbitmap_create_with_resource(RESOURCE_ID_STEP_BOOT_LIGHT);
  s_w800_walking_bitmaps[BitmapThemeDark] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_WALKING_MAN);
  s_w800_walking_bitmaps[BitmapThemeLight] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_WALKING_MAN_LIGHT);
  s_quiet_time_mouse_bitmaps[BitmapThemeDark] =
      gbitmap_create_with_resource(RESOURCE_ID_QUIET_TIME_MOUSE);
  s_quiet_time_mouse_bitmaps[BitmapThemeLight] =
      gbitmap_create_with_resource(RESOURCE_ID_QUIET_TIME_MOUSE_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][0] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_0);
  s_w800_digit_bitmaps[BitmapThemeLight][0] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_0_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][1] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_1);
  s_w800_digit_bitmaps[BitmapThemeLight][1] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_1_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][2] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_2);
  s_w800_digit_bitmaps[BitmapThemeLight][2] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_2_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][3] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_3);
  s_w800_digit_bitmaps[BitmapThemeLight][3] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_3_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][4] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_4);
  s_w800_digit_bitmaps[BitmapThemeLight][4] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_4_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][5] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_5);
  s_w800_digit_bitmaps[BitmapThemeLight][5] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_5_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][6] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_6);
  s_w800_digit_bitmaps[BitmapThemeLight][6] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_6_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][7] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_7);
  s_w800_digit_bitmaps[BitmapThemeLight][7] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_7_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][8] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_8);
  s_w800_digit_bitmaps[BitmapThemeLight][8] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_8_LIGHT);
  s_w800_digit_bitmaps[BitmapThemeDark][9] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_9);
  s_w800_digit_bitmaps[BitmapThemeLight][9] =
      gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_9_LIGHT);
  load_weather_icon_bitmaps();
}

static void destroy_theme_bitmaps(void) {
  destroy_weather_icon_bitmaps();
  for (int theme = 0; theme < BitmapThemeCount; theme++) {
    for (int i = 0; i < 10; i++) {
      destroy_bitmap(&s_w800_digit_bitmaps[theme][i]);
    }
    destroy_bitmap(&s_quiet_time_mouse_bitmaps[theme]);
    destroy_bitmap(&s_w800_walking_bitmaps[theme]);
    destroy_bitmap(&s_step_boot_bitmaps[theme]);
  }
}

static void reload_theme_bitmaps(void) {
  destroy_theme_bitmaps();
  load_theme_bitmaps();
}

static void apply_light_mode(bool enabled) {
  if (s_light_mode_enabled == enabled) {
    return;
  }

  s_light_mode_enabled = enabled;
  if (s_window) {
    window_set_background_color(s_window, theme_bg_color());
  }
  reload_theme_bitmaps();
  mark_face_dirty();
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  (void)recognizer;
  (void)context;
  if (s_shake_overlay_visible) {
    shake_hide_overlay(true);
  } else if (s_window) {
    window_stack_remove(s_window, true);
  }
}

static void click_config_provider(void *context) {
  (void)context;
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  window_set_background_color(window, theme_bg_color());
  window_set_click_config_provider(window, click_config_provider);

  s_font_top = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_date = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_font_time = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  s_font_complication = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_event = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  load_theme_bitmaps();

  s_face_layer = layer_create(bounds);
  layer_set_update_proc(s_face_layer, face_update_proc);
  layer_add_child(root, s_face_layer);

  s_shake_overlay_layer = layer_create(bounds);
  layer_set_update_proc(s_shake_overlay_layer, shake_overlay_update_proc);
  layer_set_hidden(s_shake_overlay_layer, true);
  layer_add_child(root, s_shake_overlay_layer);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time_date(t);
  update_watch_battery(battery_state_service_peek());
  s_phone_connected = connection_service_peek_pebble_app_connection();
  if (!s_phone_connected) {
    s_phone_battery_known = false;
  }
  update_phone_battery_widget();
  update_weather_widget();
  update_event(s_event_buf[0] ? s_event_buf : NULL);
  update_event_delta(s_event_delta_buf[0] ? s_event_delta_buf : NULL);
  update_stats();
}

static void window_unload(Window *window) {
  (void)window;
  destroy_theme_bitmaps();
  layer_destroy(s_shake_overlay_layer);
  s_shake_overlay_layer = NULL;
  layer_destroy(s_face_layer);
  s_face_layer = NULL;
}

static void init(void) {
  load_persisted();

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = connection_handler,
  });

#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
  health_service_set_heart_rate_sample_period(60);
#endif

  shake_configure_tap_service();

  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);
  app_message_open(256, 64);
}

static void deinit(void) {
  app_message_deregister_callbacks();
  shake_hide_overlay(true);
  if (s_shake_tap_subscribed) {
    accel_tap_service_unsubscribe();
    s_shake_tap_subscribed = false;
  }
#if defined(PBL_HEALTH)
  health_service_set_heart_rate_sample_period(0);
  health_service_events_unsubscribe();
#endif
  connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
