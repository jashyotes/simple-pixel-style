#include <pebble.h>
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

#define SCREEN_W 200
#define SCREEN_H 228
#define DATE_FRAME_Y 36
#define TIME_FRAME_Y 69
#define TIME_FRAME_H 60
#define TIME_VISUAL_BOTTOM 111
#define EVENT_SEPARATOR_Y 200
#define COMPLICATION_RADIUS 24
#define COMPLICATION_COUNT 3
#define COMPLICATION_CENTER_Y 163

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
} ComplicationType;

static Window *s_window;
static Layer *s_face_layer;

static GFont s_font_top;
static GFont s_font_date;
static GFont s_font_time;
static GFont s_font_complication;
static GFont s_font_event;
static GBitmap *s_step_boot_bitmap;
static GBitmap *s_w800_walking_bitmap;
static GBitmap *s_w800_digit_bitmaps[10];

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
static char s_wind_buf[8];
static char s_uv_buf[8];
static char s_event_delta_buf[8];

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
static int s_steps_count = 0;
static ComplicationType s_complication_slots[COMPLICATION_COUNT] = {
  ComplicationWeather,
  ComplicationRainChance,
  ComplicationHeartRate,
};

static void mark_face_dirty(void) {
  if (s_face_layer) {
    layer_mark_dirty(s_face_layer);
  }
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
  return value >= ComplicationTemperature && value <= ComplicationWeatherIcon
      ? (ComplicationType)value
      : fallback;
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
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_ampm_letter(ctx, label[0], origin);
  draw_ampm_letter(ctx, label[1], GPoint(origin.x + 10, origin.y));
}

static void draw_time_row(GContext *ctx) {
  const GRect time_frame = GRect(0, TIME_FRAME_Y, SCREEN_W, TIME_FRAME_H);
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
            GColorWhite, GTextAlignmentCenter);
  draw_ampm_label(ctx, s_ampm_buf, GPoint(ampm_x, TIME_VISUAL_BOTTOM - ampm_height));
}

static void draw_watch_icon_c(GContext *ctx, GPoint origin) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
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
            GColorWhite, GTextAlignmentLeft);
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
  int x = frame.origin.x + frame.size.w - (digit_count * digit_w);
  if (x < frame.origin.x) {
    x = frame.origin.x;
  }

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  for (int i = 0; i < digit_count; i++) {
    int digit = digits[i] - '0';
    if (digit >= 0 && digit <= 9 && s_w800_digit_bitmaps[digit]) {
      graphics_draw_bitmap_in_rect(ctx, s_w800_digit_bitmaps[digit],
                                   GRect(x + (i * digit_w), frame.origin.y,
                                         digit_w, digit_h));
    }
  }
}

static void draw_w800_steps_top_bar(GContext *ctx) {
  if (!s_w800_walking_bitmap) {
    return;
  }

  const GRect bar_frame = GRect(56, 3, 88, 24);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bar_frame, 1, GCornersAll);

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_w800_walking_bitmap, GRect(61, 8, 8, 13));
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

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y), GPoint(origin.x + 5, origin.y + 14));
  graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y), GPoint(origin.x + 10, origin.y + 4));
  graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y + 4), GPoint(origin.x + 2, origin.y + 10));
  graphics_draw_line(ctx, GPoint(origin.x + 2, origin.y + 4), GPoint(origin.x + 10, origin.y + 10));
  graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y + 10), GPoint(origin.x + 5, origin.y + 14));
}

static void draw_step_icon(GContext *ctx, GPoint origin) {
  if (!s_step_boot_bitmap) {
    return;
  }

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_step_boot_bitmap, GRect(origin.x, origin.y, 16, 14));
}

static void draw_step_icon_centered(GContext *ctx, GPoint center) {
  draw_step_icon(ctx, GPoint(center.x - 8, center.y - 7));
}

static void draw_icon_block(GContext *ctx, GPoint origin, int x, int y, int w, int h) {
  graphics_fill_rect(ctx, GRect(origin.x + x, origin.y + y, w, h), 0, GCornerNone);
}

static void draw_sun_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, origin, 7, 1, 2, 3);
  draw_icon_block(ctx, origin, 7, 13, 2, 3);
  draw_icon_block(ctx, origin, 1, 7, 3, 2);
  draw_icon_block(ctx, origin, 12, 7, 3, 2);
  draw_icon_block(ctx, origin, 5, 5, 6, 6);
}

static void draw_weather_sun_icon_centered(GContext *ctx, GPoint center) {
  GPoint origin = GPoint(center.x - 7, center.y - 7);
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, origin, 6, 0, 2, 3);
  draw_icon_block(ctx, origin, 6, 11, 2, 3);
  draw_icon_block(ctx, origin, 0, 6, 3, 2);
  draw_icon_block(ctx, origin, 11, 6, 3, 2);
  draw_icon_block(ctx, origin, 4, 4, 6, 6);
}

static void draw_weather_cloud_shape(GContext *ctx, GPoint origin) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, origin, 5, 1, 5, 2);
  draw_icon_block(ctx, origin, 3, 3, 10, 2);
  draw_icon_block(ctx, origin, 1, 5, 14, 4);
  draw_icon_block(ctx, origin, 2, 9, 12, 1);
}

static void draw_weather_cloud_icon_centered(GContext *ctx, GPoint center) {
  draw_weather_cloud_shape(ctx, GPoint(center.x - 8, center.y - 5));
}

static void draw_weather_rain_icon_centered(GContext *ctx, GPoint center) {
  draw_weather_cloud_shape(ctx, GPoint(center.x - 8, center.y - 8));
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(center.x - 5, center.y + 3), GPoint(center.x - 6, center.y + 6));
  graphics_draw_line(ctx, GPoint(center.x, center.y + 3), GPoint(center.x - 1, center.y + 6));
  graphics_draw_line(ctx, GPoint(center.x + 5, center.y + 3), GPoint(center.x + 4, center.y + 6));
}

static void draw_weather_snow_icon_centered(GContext *ctx, GPoint center) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(center.x, center.y - 7), GPoint(center.x, center.y + 7));
  graphics_draw_line(ctx, GPoint(center.x - 6, center.y - 3), GPoint(center.x + 6, center.y + 3));
  graphics_draw_line(ctx, GPoint(center.x - 6, center.y + 3), GPoint(center.x + 6, center.y - 3));
}

static void draw_weather_storm_icon_centered(GContext *ctx, GPoint center) {
  draw_weather_cloud_shape(ctx, GPoint(center.x - 8, center.y - 8));
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, GPoint(center.x - 1, center.y + 2), 0, 0, 4, 2);
  draw_icon_block(ctx, GPoint(center.x - 1, center.y + 2), 2, 2, 2, 3);
  draw_icon_block(ctx, GPoint(center.x - 1, center.y + 2), -2, 4, 4, 2);
}

static void draw_weather_fog_icon_centered(GContext *ctx, GPoint center) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_line(ctx, GPoint(center.x - 7, center.y - 5), GPoint(center.x + 7, center.y - 5));
  graphics_draw_line(ctx, GPoint(center.x - 5, center.y), GPoint(center.x + 5, center.y));
  graphics_draw_line(ctx, GPoint(center.x - 7, center.y + 5), GPoint(center.x + 7, center.y + 5));
}

static void draw_weather_icon_centered(GContext *ctx, GPoint center) {
  if (!s_weather_known) {
    draw_weather_cloud_icon_centered(ctx, center);
  } else if (s_weather_code == 0) {
    draw_weather_sun_icon_centered(ctx, center);
  } else if (s_weather_code == 45 || s_weather_code == 48) {
    draw_weather_fog_icon_centered(ctx, center);
  } else if ((s_weather_code >= 51 && s_weather_code <= 67) ||
             (s_weather_code >= 80 && s_weather_code <= 82)) {
    draw_weather_rain_icon_centered(ctx, center);
  } else if ((s_weather_code >= 71 && s_weather_code <= 77) ||
             (s_weather_code >= 85 && s_weather_code <= 86)) {
    draw_weather_snow_icon_centered(ctx, center);
  } else if (s_weather_code >= 95 && s_weather_code <= 99) {
    draw_weather_storm_icon_centered(ctx, center);
  } else {
    draw_weather_cloud_icon_centered(ctx, center);
  }
}

static void draw_temp_unit_icon_centered(GContext *ctx, GPoint center) {
  GPoint origin = GPoint(center.x - 7, center.y - 5);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);
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
  graphics_context_set_stroke_color(ctx, GColorWhite);
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
  graphics_context_set_fill_color(ctx, GColorWhite);
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
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, GRect(center.x - 5, center.y - 8, 10, 16));
  graphics_draw_line(ctx, GPoint(center.x - 2, center.y + 5), GPoint(center.x + 2, center.y + 5));
}

static void draw_wind_icon_centered(GContext *ctx, GPoint center) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_line(ctx, GPoint(center.x - 8, center.y - 5), GPoint(center.x + 7, center.y - 5));
  graphics_draw_line(ctx, GPoint(center.x - 5, center.y), GPoint(center.x + 5, center.y));
  graphics_draw_line(ctx, GPoint(center.x - 8, center.y + 5), GPoint(center.x + 2, center.y + 5));
}

static void draw_calendar_icon_centered(GContext *ctx, GPoint center) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 0, 2, 14, 2);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 0, 4, 2, 11);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 12, 4, 2, 11);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 0, 13, 14, 2);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 4, 7, 2, 2);
  draw_icon_block(ctx, GPoint(center.x - 7, center.y - 8), 8, 7, 2, 2);
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
    draw_sun_icon(ctx, GPoint(center.x - 8, center.y - 8));
  } else if (type == ComplicationNextEvent) {
    draw_calendar_icon_centered(ctx, center);
  } else if (type == ComplicationWeather || type == ComplicationWeatherIcon) {
    draw_weather_icon_centered(ctx, center);
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
  }
  return "--";
}

static void draw_complication(GContext *ctx, GPoint center, ComplicationType type) {
  const char *value = value_for_complication(type);

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_circle(ctx, center, COMPLICATION_RADIUS);

  if (type == ComplicationWeatherIcon) {
    draw_complication_icon(ctx, type, center);
    return;
  }

  draw_complication_icon(ctx, type, GPoint(center.x, center.y - 12));

  GFont value_font = strlen(value) > 2
      ? fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)
      : s_font_complication;
  draw_text(ctx, value, value_font,
            GRect(center.x - 23, center.y - 6, 46, 18),
            GColorWhite, GTextAlignmentCenter);
}

static void face_update_proc(Layer *layer, GContext *ctx) {
  (void)layer;

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  draw_watch_battery(ctx);
  if (s_w800_steps_top_enabled) {
    draw_w800_steps_top_bar(ctx);
  }
  draw_bt_icon(ctx, GPoint(181, 7));

  draw_text(ctx, s_date_buf, s_font_date, GRect(0, DATE_FRAME_Y, SCREEN_W, 29),
            GColorWhite, GTextAlignmentCenter);

  draw_time_row(ctx);

  draw_complication(ctx, GPoint(40, COMPLICATION_CENTER_Y), s_complication_slots[0]);
  draw_complication(ctx, GPoint(100, COMPLICATION_CENTER_Y), s_complication_slots[1]);
  draw_complication(ctx, GPoint(160, COMPLICATION_CENTER_Y), s_complication_slots[2]);

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(8, EVENT_SEPARATOR_Y), GPoint(192, EVENT_SEPARATOR_Y));

  draw_text(ctx, s_event_buf, s_font_event, GRect(8, 203, 184, 25),
            GColorWhite, GTextAlignmentCenter);
}

static void update_time_date(struct tm *t) {
  strftime(s_time_buf, sizeof(s_time_buf), "%I:%M", t);
  strftime(s_ampm_buf, sizeof(s_ampm_buf), "%p", t);

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

  t = dict_find(iter, MESSAGE_KEY_NEXT_EVENT_DELTA);
  if (t && t->type == TUPLE_CSTRING) {
    persist_write_string(PERSIST_KEY_EVENT_DELTA, t->value->cstring);
    update_event_delta(t->value->cstring);
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
  if (persist_exists(PERSIST_KEY_WIND_SPEED)) {
    s_wind_speed = (uint8_t)persist_read_int(PERSIST_KEY_WIND_SPEED);
    s_wind_known = true;
  }
  if (persist_exists(PERSIST_KEY_UV_INDEX)) {
    s_uv_index = (uint8_t)persist_read_int(PERSIST_KEY_UV_INDEX);
    s_uv_known = true;
  }
  if (persist_exists(PERSIST_KEY_TOP_STEPS)) {
    s_w800_steps_top_enabled = persist_read_bool(PERSIST_KEY_TOP_STEPS);
  }
  if (persist_exists(PERSIST_KEY_TEMP_UNIT)) {
    s_temperature_unit_celsius = persist_read_bool(PERSIST_KEY_TEMP_UNIT);
  }
  if (persist_exists(PERSIST_KEY_EVENT)) {
    persist_read_string(PERSIST_KEY_EVENT, s_event_buf, sizeof(s_event_buf));
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_EVENT_DELTA)) {
    persist_read_string(PERSIST_KEY_EVENT_DELTA, s_event_delta_buf, sizeof(s_event_delta_buf));
    s_event_delta_buf[sizeof(s_event_delta_buf) - 1] = '\0';
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
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  window_set_background_color(window, GColorBlack);

  s_font_top = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_date = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_font_time = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  s_font_complication = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_event = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_step_boot_bitmap = gbitmap_create_with_resource(RESOURCE_ID_STEP_BOOT);
  s_w800_walking_bitmap = gbitmap_create_with_resource(RESOURCE_ID_W800_WALKING_MAN);
  s_w800_digit_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_0);
  s_w800_digit_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_1);
  s_w800_digit_bitmaps[2] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_2);
  s_w800_digit_bitmaps[3] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_3);
  s_w800_digit_bitmaps[4] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_4);
  s_w800_digit_bitmaps[5] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_5);
  s_w800_digit_bitmaps[6] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_6);
  s_w800_digit_bitmaps[7] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_7);
  s_w800_digit_bitmaps[8] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_8);
  s_w800_digit_bitmaps[9] = gbitmap_create_with_resource(RESOURCE_ID_W800_STEP_DIGIT_9);

  s_face_layer = layer_create(bounds);
  layer_set_update_proc(s_face_layer, face_update_proc);
  layer_add_child(root, s_face_layer);

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
  for (int i = 0; i < 10; i++) {
    gbitmap_destroy(s_w800_digit_bitmaps[i]);
  }
  gbitmap_destroy(s_w800_walking_bitmap);
  gbitmap_destroy(s_step_boot_bitmap);
  layer_destroy(s_face_layer);
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

  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);
  app_message_open(256, 64);
}

static void deinit(void) {
  app_message_deregister_callbacks();
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
