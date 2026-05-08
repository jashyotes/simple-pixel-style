#include <pebble.h>

// JY Time watchface
// emery target (Pebble Time 2, 200x228), black bg, white fg
//
// Watch-side data: time, date, weekday, watch battery, BT connection, steps, HR.
// Companion-fed via AppMessage: next calendar event, phone battery %, weather code, temp F.

#define DIVIDER_TOP_Y    34
#define DIVIDER_BOTTOM_Y 190
#define DIVIDER_STROKE   3
#define MARGIN           6
#define TOP_COLS         3

#define PERSIST_KEY_PHONE_BATTERY  100
#define PERSIST_KEY_WEATHER_CODE   101
#define PERSIST_KEY_TEMPERATURE    102
#define PERSIST_KEY_EVENT          103

static Window *s_window;

static TextLayer *s_widget_phone_layer;
static TextLayer *s_widget_weather_layer;
static TextLayer *s_widget_watch_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_date_layer;
static TextLayer *s_time_layer;
static TextLayer *s_event_layer;
static TextLayer *s_steps_layer;
static TextLayer *s_bpm_layer;

static Layer *s_dividers_layer;
static Layer *s_widgets_layer;

static char s_weekday_buf[16];
static char s_date_buf[16];
static char s_time_buf[8];
static char s_phone_buf[12];
static char s_weather_buf[12];
static char s_watch_buf[12];
static char s_steps_buf[16];
static char s_bpm_buf[12];
static char s_event_buf[80];

static uint8_t s_phone_battery_pct = 0;
static bool s_phone_battery_known = false;
static bool s_phone_connected = false;

static int8_t s_temperature = 0;
static bool s_temperature_known = false;
static uint8_t s_weather_code = 0;
static bool s_weather_known = false;

static void to_upper(char *s) {
  for (; *s; s++) {
    if (*s >= 'a' && *s <= 'z') *s -= 32;
  }
}

static void update_time_date(struct tm *t) {
  if (clock_is_24h_style()) {
    strftime(s_time_buf, sizeof(s_time_buf), "%H:%M", t);
  } else {
    strftime(s_time_buf, sizeof(s_time_buf), "%I:%M", t);
  }
  text_layer_set_text(s_time_layer, s_time_buf);

  strftime(s_weekday_buf, sizeof(s_weekday_buf), "%A", t);
  to_upper(s_weekday_buf);
  text_layer_set_text(s_weekday_layer, s_weekday_buf);

  strftime(s_date_buf, sizeof(s_date_buf), "%b %d %Y", t);
  to_upper(s_date_buf);
  text_layer_set_text(s_date_layer, s_date_buf);
}

static void update_phone_widget(void) {
  if (!s_phone_connected) {
    snprintf(s_phone_buf, sizeof(s_phone_buf), "X");
  } else if (s_phone_battery_known) {
    snprintf(s_phone_buf, sizeof(s_phone_buf), "%d%%", s_phone_battery_pct);
  } else {
    snprintf(s_phone_buf, sizeof(s_phone_buf), "...");
  }
  text_layer_set_text(s_widget_phone_layer, s_phone_buf);
  layer_mark_dirty(s_widgets_layer);
}

static void update_weather_widget(void) {
  if (s_weather_known && s_temperature_known) {
    snprintf(s_weather_buf, sizeof(s_weather_buf), "%d°", s_temperature);
  } else {
    snprintf(s_weather_buf, sizeof(s_weather_buf), "--");
  }
  text_layer_set_text(s_widget_weather_layer, s_weather_buf);
  layer_mark_dirty(s_widgets_layer);
}

static void update_watch_battery(BatteryChargeState s) {
  snprintf(s_watch_buf, sizeof(s_watch_buf), "%d%%", s.charge_percent);
  text_layer_set_text(s_widget_watch_layer, s_watch_buf);
  layer_mark_dirty(s_widgets_layer);
}

static void update_event(const char *title) {
  if (title && title[0] != '\0') {
    strncpy(s_event_buf, title, sizeof(s_event_buf) - 1);
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
  } else {
    strncpy(s_event_buf, "No event", sizeof(s_event_buf));
  }
  text_layer_set_text(s_event_layer, s_event_buf);
}

static void update_stats(void) {
  int steps = (int) health_service_sum_today(HealthMetricStepCount);
  HealthValue bpm = health_service_peek_current_value(HealthMetricHeartRateBPM);

  if (steps < 1000) {
    snprintf(s_steps_buf, sizeof(s_steps_buf), "ST %d", steps);
  } else {
    snprintf(s_steps_buf, sizeof(s_steps_buf), "ST %d.%dk",
             steps / 1000, (steps % 1000) / 100);
  }
  snprintf(s_bpm_buf, sizeof(s_bpm_buf), "BPM %ld", (long)bpm);
  text_layer_set_text(s_steps_layer, s_steps_buf);
  text_layer_set_text(s_bpm_layer, s_bpm_buf);
}

// --- service handlers ---

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
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
  update_phone_widget();
}

// --- AppMessage inbox ---

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *t;

  t = dict_find(iter, MESSAGE_KEY_NEXT_EVENT);
  if (t && t->type == TUPLE_CSTRING) {
    persist_write_string(PERSIST_KEY_EVENT, t->value->cstring);
    update_event(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_PHONE_BATTERY);
  if (t) {
    s_phone_battery_pct = (uint8_t) t->value->uint8;
    s_phone_battery_known = true;
    persist_write_int(PERSIST_KEY_PHONE_BATTERY, s_phone_battery_pct);
    update_phone_widget();
  }

  t = dict_find(iter, MESSAGE_KEY_WEATHER_CODE);
  if (t) {
    s_weather_code = (uint8_t) t->value->uint8;
    s_weather_known = true;
    persist_write_int(PERSIST_KEY_WEATHER_CODE, s_weather_code);
  }

  t = dict_find(iter, MESSAGE_KEY_TEMPERATURE);
  if (t) {
    s_temperature = (int8_t) t->value->int8;
    s_temperature_known = true;
    persist_write_int(PERSIST_KEY_TEMPERATURE, s_temperature);
  }

  update_weather_widget();
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "AppMessage inbox dropped: %d", (int)reason);
}

// --- layout/window ---

static void draw_phone_icon(GContext *ctx, GPoint origin) {
  GRect body = GRect(origin.x + 2, origin.y, 10, 18);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_draw_round_rect(ctx, body, 2);
  graphics_fill_rect(ctx, GRect(origin.x + 5, origin.y + 2, 4, 1), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(origin.x + 6, origin.y + 15, 2, 2), 0, GCornerNone);
}

static void draw_watch_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_draw_rect(ctx, GRect(origin.x + 2, origin.y + 4, 14, 10));
  graphics_fill_rect(ctx, GRect(origin.x + 5, origin.y, 8, 4), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(origin.x + 5, origin.y + 14, 8, 4), 0, GCornerNone);
}

static void draw_cloud_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(origin.x + 6, origin.y + 11), 5);
  graphics_fill_circle(ctx, GPoint(origin.x + 12, origin.y + 8), 6);
  graphics_fill_circle(ctx, GPoint(origin.x + 18, origin.y + 11), 5);
  graphics_fill_rect(ctx, GRect(origin.x + 4, origin.y + 11, 17, 5), 0, GCornerNone);
}

static void draw_sun_icon(GContext *ctx, GPoint origin) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(origin.x + 10, origin.y + 9), 5);
  graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y), GPoint(origin.x + 10, origin.y + 3));
  graphics_draw_line(ctx, GPoint(origin.x + 10, origin.y + 15), GPoint(origin.x + 10, origin.y + 18));
  graphics_draw_line(ctx, GPoint(origin.x + 1, origin.y + 9), GPoint(origin.x + 4, origin.y + 9));
  graphics_draw_line(ctx, GPoint(origin.x + 16, origin.y + 9), GPoint(origin.x + 19, origin.y + 9));
}

static void draw_weather_icon(GContext *ctx, GPoint origin) {
  if (!s_weather_known) {
    draw_cloud_icon(ctx, origin);
    return;
  }
  if (s_weather_code == 0) {
    draw_sun_icon(ctx, origin);
    return;
  }
  draw_cloud_icon(ctx, origin);
  if ((s_weather_code >= 51 && s_weather_code <= 67) ||
      (s_weather_code >= 80 && s_weather_code <= 82) ||
      s_weather_code >= 95) {
    graphics_draw_line(ctx, GPoint(origin.x + 7, origin.y + 18), GPoint(origin.x + 5, origin.y + 22));
    graphics_draw_line(ctx, GPoint(origin.x + 13, origin.y + 18), GPoint(origin.x + 11, origin.y + 22));
    graphics_draw_line(ctx, GPoint(origin.x + 19, origin.y + 18), GPoint(origin.x + 17, origin.y + 22));
  }
}

static void widgets_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int col_w = bounds.size.w / TOP_COLS;

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, DIVIDER_STROKE);
  graphics_draw_line(ctx, GPoint(col_w, 0), GPoint(col_w, DIVIDER_TOP_Y));
  graphics_draw_line(ctx, GPoint(2 * col_w, 0), GPoint(2 * col_w, DIVIDER_TOP_Y));

  draw_phone_icon(ctx, GPoint(8, 7));
  draw_watch_icon(ctx, GPoint(col_w + 9, 7));
  draw_weather_icon(ctx, GPoint(2 * col_w + 7, 5));
}

static void dividers_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, DIVIDER_STROKE);
  graphics_draw_line(ctx,
    GPoint(MARGIN, DIVIDER_TOP_Y),
    GPoint(bounds.size.w - MARGIN, DIVIDER_TOP_Y));
  graphics_draw_line(ctx,
    GPoint(MARGIN, DIVIDER_BOTTOM_Y),
    GPoint(bounds.size.w - MARGIN, DIVIDER_BOTTOM_Y));
  graphics_draw_line(ctx,
    GPoint(bounds.size.w / 2, DIVIDER_BOTTOM_Y),
    GPoint(bounds.size.w / 2, bounds.size.h));
}

static TextLayer* make_text_layer(Layer *parent, GRect frame, const char *text,
                                  const char *font_key, GTextAlignment align) {
  TextLayer *l = text_layer_create(frame);
  text_layer_set_background_color(l, GColorBlack);
  text_layer_set_text_color(l, GColorWhite);
  text_layer_set_font(l, fonts_get_system_font(font_key));
  text_layer_set_text_alignment(l, align);
  text_layer_set_text(l, text);
  layer_add_child(parent, text_layer_get_layer(l));
  return l;
}

static void load_persisted(void) {
  if (persist_exists(PERSIST_KEY_PHONE_BATTERY)) {
    s_phone_battery_pct = (uint8_t) persist_read_int(PERSIST_KEY_PHONE_BATTERY);
    s_phone_battery_known = true;
  }
  if (persist_exists(PERSIST_KEY_WEATHER_CODE)) {
    s_weather_code = (uint8_t) persist_read_int(PERSIST_KEY_WEATHER_CODE);
    s_weather_known = true;
  }
  if (persist_exists(PERSIST_KEY_TEMPERATURE)) {
    s_temperature = (int8_t) persist_read_int(PERSIST_KEY_TEMPERATURE);
    s_temperature_known = true;
  }
  if (persist_exists(PERSIST_KEY_EVENT)) {
    char buf[80];
    persist_read_string(PERSIST_KEY_EVENT, buf, sizeof(buf));
    strncpy(s_event_buf, buf, sizeof(s_event_buf));
    s_event_buf[sizeof(s_event_buf) - 1] = '\0';
  }
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  window_set_background_color(window, GColorBlack);

  int col_w = bounds.size.w / 3;
  s_widgets_layer = layer_create(GRect(0, 0, bounds.size.w, DIVIDER_TOP_Y + 1));
  layer_set_update_proc(s_widgets_layer, widgets_update_proc);
  layer_add_child(root, s_widgets_layer);

  s_widget_phone_layer = make_text_layer(root, GRect(24, 6, col_w - 24, 22),
    "...", FONT_KEY_GOTHIC_18_BOLD, GTextAlignmentCenter);
  s_widget_watch_layer = make_text_layer(root, GRect(col_w + 26, 6, col_w - 26, 22),
    "--", FONT_KEY_GOTHIC_18_BOLD, GTextAlignmentCenter);
  s_widget_weather_layer = make_text_layer(root, GRect(2 * col_w + 29, 6, col_w - 29, 22),
    "--", FONT_KEY_GOTHIC_18_BOLD, GTextAlignmentCenter);

  s_weekday_layer = make_text_layer(root, GRect(0, 40, bounds.size.w, 32),
    "", FONT_KEY_GOTHIC_24_BOLD, GTextAlignmentCenter);

  s_date_layer = make_text_layer(root, GRect(0, 70, bounds.size.w, 28),
    "", FONT_KEY_GOTHIC_24_BOLD, GTextAlignmentCenter);

  s_time_layer = make_text_layer(root, GRect(0, 102, bounds.size.w, 58),
    "", FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter);

  s_event_layer = make_text_layer(root, GRect(MARGIN, 162, bounds.size.w - 2 * MARGIN, 24),
    "No event", FONT_KEY_GOTHIC_18, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_event_layer, GTextOverflowModeTrailingEllipsis);

  s_steps_layer = make_text_layer(root, GRect(0, 194, bounds.size.w / 2, 30),
    "", FONT_KEY_GOTHIC_24_BOLD, GTextAlignmentCenter);
  s_bpm_layer = make_text_layer(root, GRect(bounds.size.w / 2, 194, bounds.size.w / 2, 30),
    "", FONT_KEY_GOTHIC_24_BOLD, GTextAlignmentCenter);

  s_dividers_layer = layer_create(bounds);
  layer_set_update_proc(s_dividers_layer, dividers_update_proc);
  layer_add_child(root, s_dividers_layer);

  // Initial population
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time_date(t);
  update_watch_battery(battery_state_service_peek());
  s_phone_connected = connection_service_peek_pebble_app_connection();
  update_phone_widget();
  update_weather_widget();
  update_event(s_event_buf[0] ? s_event_buf : NULL);
  update_stats();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_widget_phone_layer);
  text_layer_destroy(s_widget_weather_layer);
  text_layer_destroy(s_widget_watch_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_event_layer);
  text_layer_destroy(s_steps_layer);
  text_layer_destroy(s_bpm_layer);
  layer_destroy(s_widgets_layer);
  layer_destroy(s_dividers_layer);
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
    .pebble_app_connection_handler = connection_handler
  });
  health_service_set_heart_rate_sample_period(60);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);
  app_message_open(256, 64);
}

static void deinit(void) {
  app_message_deregister_callbacks();
  health_service_set_heart_rate_sample_period(0);
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
