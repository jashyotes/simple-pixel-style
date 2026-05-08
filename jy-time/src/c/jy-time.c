#include <pebble.h>
#include <string.h>

// JY Time watchface
// Pixel-style structure, emery target (Pebble Time 2, 200x228).
//
// Watch-side data: time, date, watch battery, Bluetooth, steps, HR.
// Companion-fed via AppMessage: next calendar event, phone battery %, weather code,
// temperature F, and rain chance.

#define PERSIST_KEY_PHONE_BATTERY  100
#define PERSIST_KEY_WEATHER_CODE   101
#define PERSIST_KEY_TEMPERATURE    102
#define PERSIST_KEY_EVENT          103
#define PERSIST_KEY_RAIN_CHANCE    104
#define PERSIST_KEY_TOP_STEPS      105

#define SCREEN_W 200
#define SCREEN_H 228

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
static char s_steps_buf[8];
static char s_temp_buf[8];
static char s_rain_buf[8];
static char s_bpm_buf[12];

static uint8_t s_phone_battery_pct = 0;
static bool s_phone_battery_known = false;
static bool s_phone_connected = false;

static int8_t s_temperature = 0;
static bool s_temperature_known = false;
static uint8_t s_weather_code = 0;
static bool s_weather_known = false;
static uint8_t s_rain_chance = 0;
static bool s_rain_known = false;
static bool s_w800_steps_top_enabled = false;
static int s_steps_count = 0;

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
  const GRect time_frame = GRect(0, 60, SCREEN_W, 60);
  const int time_visual_bottom = 102;
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
  draw_ampm_label(ctx, s_ampm_buf, GPoint(ampm_x, time_visual_bottom - ampm_height));
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
  draw_step_icon(ctx, GPoint(center.x - 8, center.y - 14));
}

static void draw_icon_block(GContext *ctx, GPoint origin, int x, int y, int w, int h) {
  graphics_fill_rect(ctx, GRect(origin.x + x, origin.y + y, w, h), 0, GCornerNone);
}

static void draw_cloud_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, origin, 6, 5, 7, 2);
  draw_icon_block(ctx, origin, 4, 7, 11, 2);
  draw_icon_block(ctx, origin, 2, 9, 16, 5);
  draw_icon_block(ctx, origin, 4, 14, 12, 2);
}

static void draw_sun_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, origin, 7, 1, 2, 3);
  draw_icon_block(ctx, origin, 7, 13, 2, 3);
  draw_icon_block(ctx, origin, 1, 7, 3, 2);
  draw_icon_block(ctx, origin, 12, 7, 3, 2);
  draw_icon_block(ctx, origin, 5, 5, 6, 6);
}

static void draw_weather_icon(GContext *ctx, GPoint origin) {
  if (s_weather_known && s_weather_code == 0) {
    draw_sun_icon(ctx, origin);
    return;
  }
  draw_cloud_icon(ctx, origin);
  if ((s_weather_code >= 51 && s_weather_code <= 67) ||
      (s_weather_code >= 80 && s_weather_code <= 82) ||
      s_weather_code >= 95) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_line(ctx, GPoint(origin.x + 5, origin.y + 16), GPoint(origin.x + 4, origin.y + 18));
    graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y + 16), GPoint(origin.x + 9, origin.y + 18));
    graphics_draw_line(ctx, GPoint(origin.x + 14, origin.y + 16), GPoint(origin.x + 13, origin.y + 18));
  }
}

static void draw_weather_icon_centered(GContext *ctx, GPoint center) {
  draw_weather_icon(ctx, GPoint(center.x - 8, center.y - 14));
}

static void draw_drop_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  draw_icon_block(ctx, origin, 7, 1, 2, 2);
  draw_icon_block(ctx, origin, 6, 3, 4, 2);
  draw_icon_block(ctx, origin, 5, 5, 6, 2);
  draw_icon_block(ctx, origin, 4, 7, 8, 5);
  draw_icon_block(ctx, origin, 5, 12, 6, 2);
  draw_icon_block(ctx, origin, 7, 14, 2, 1);
}

static void draw_drop_icon_centered(GContext *ctx, GPoint center) {
  draw_drop_icon(ctx, GPoint(center.x - 7, center.y - 14));
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
  draw_heart_icon(ctx, GPoint(center.x - 8, center.y - 14));
}

static void draw_complication(GContext *ctx, GPoint center, const char *value, int type) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_circle(ctx, center, 22);

  if (type == 0) {
    draw_step_icon_centered(ctx, center);
  } else if (type == 1) {
    draw_weather_icon_centered(ctx, center);
  } else if (type == 2) {
    draw_drop_icon_centered(ctx, center);
  } else {
    draw_heart_icon_centered(ctx, center);
  }

  GFont value_font = strlen(value) > 2
      ? fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)
      : s_font_complication;
  draw_text(ctx, value, value_font,
            GRect(center.x - 22, center.y + 2, 44, 18),
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

  draw_text(ctx, s_date_buf, s_font_date, GRect(0, 31, SCREEN_W, 29),
            GColorWhite, GTextAlignmentCenter);

  draw_time_row(ctx);

  if (s_w800_steps_top_enabled) {
    draw_complication(ctx, GPoint(40, 174), s_temp_buf, 1);
    draw_complication(ctx, GPoint(100, 174), s_rain_buf, 2);
    draw_complication(ctx, GPoint(160, 174), s_bpm_buf, 3);
  } else {
    draw_complication(ctx, GPoint(28, 174), s_steps_buf, 0);
    draw_complication(ctx, GPoint(76, 174), s_temp_buf, 1);
    draw_complication(ctx, GPoint(124, 174), s_rain_buf, 2);
    draw_complication(ctx, GPoint(172, 174), s_bpm_buf, 3);
  }

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(8, 200), GPoint(192, 200));

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
  snprintf(s_watch_buf, sizeof(s_watch_buf), "%d", s.charge_percent);
  mark_face_dirty();
}

static void update_weather_widget(void) {
  if (s_temperature_known) {
    snprintf(s_temp_buf, sizeof(s_temp_buf), "%d", s_temperature);
  } else {
    snprintf(s_temp_buf, sizeof(s_temp_buf), "--");
  }

  if (s_rain_known) {
    snprintf(s_rain_buf, sizeof(s_rain_buf), "%d%%", s_rain_chance);
  } else {
    snprintf(s_rain_buf, sizeof(s_rain_buf), "--");
  }
  mark_face_dirty();
}

static void update_event(const char *title) {
  if (title && title[0] != '\0') {
    strncpy(s_event_buf, title, sizeof(s_event_buf) - 1);
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
  } else {
    strncpy(s_event_buf, "No event", sizeof(s_event_buf));
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
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
  } else if (steps < 10000) {
    snprintf(s_steps_buf, sizeof(s_steps_buf), "%d.%dk", steps / 1000, (steps % 1000) / 100);
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

  t = dict_find(iter, MESSAGE_KEY_TOP_STEPS);
  if (t) {
    s_w800_steps_top_enabled = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_TOP_STEPS, s_w800_steps_top_enabled);
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
  if (persist_exists(PERSIST_KEY_TOP_STEPS)) {
    s_w800_steps_top_enabled = persist_read_bool(PERSIST_KEY_TOP_STEPS);
  }
  if (persist_exists(PERSIST_KEY_EVENT)) {
    persist_read_string(PERSIST_KEY_EVENT, s_event_buf, sizeof(s_event_buf));
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
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
  update_weather_widget();
  update_event(s_event_buf[0] ? s_event_buf : NULL);
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
