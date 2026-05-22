#include <pebble.h>
#include <stdlib.h>
#include <string.h>

// US Tidemaps app
// Standalone Pebble app showing NOAA tide predictions for a configured
// station. Persists fetched data so the chart paints immediately on
// relaunch while a fresh fetch is in flight.
//
// UP / DOWN scroll the rendered window left / right by one hour.
// SELECT toggles between 24h and 48h views.
// BACK exits.

#define SCREEN_W 200
#define SCREEN_H 228

#define TIDE_WINDOW_HOURS 48
#define TIDE_NOW_INDEX    24

#define PERSIST_KEY_HOURLY        0
#define PERSIST_KEY_NEXT_HIGH_T   1
#define PERSIST_KEY_NEXT_HIGH_L   2
#define PERSIST_KEY_NEXT_LOW_T    3
#define PERSIST_KEY_NEXT_LOW_L    4
#define PERSIST_KEY_STATION_NAME  5
#define PERSIST_KEY_UNITS_METERS  6
#define PERSIST_KEY_STATION_ID    7
#define PERSIST_KEY_VIEW_HOURS    8
#define PERSIST_KEY_CURSOR_OFFSET 9

#define MSG_KEY_HOURLY        MESSAGE_KEY_TIDE_HOURLY_LEVELS
#define MSG_KEY_NEXT_HIGH_T   MESSAGE_KEY_TIDE_NEXT_HIGH_T
#define MSG_KEY_NEXT_HIGH_L   MESSAGE_KEY_TIDE_NEXT_HIGH_LEVEL
#define MSG_KEY_NEXT_LOW_T    MESSAGE_KEY_TIDE_NEXT_LOW_T
#define MSG_KEY_NEXT_LOW_L    MESSAGE_KEY_TIDE_NEXT_LOW_LEVEL
#define MSG_KEY_STATION_NAME  MESSAGE_KEY_TIDE_STATION_NAME
#define MSG_KEY_UNITS         MESSAGE_KEY_TIDE_UNITS
#define MSG_KEY_STATION_ID    MESSAGE_KEY_TIDE_STATION_ID

static Window  *s_window;
static Layer   *s_root_layer;

static uint8_t  s_tide_hourly[TIDE_WINDOW_HOURS];
static uint32_t s_next_high_t = 0;
static uint8_t  s_next_high_l = 0xFF;
static uint32_t s_next_low_t  = 0;
static uint8_t  s_next_low_l  = 0xFF;
static char     s_station_name[24] = "";
static char     s_station_id[16]   = "";
static bool     s_units_meters     = false;

static int      s_view_hours     = 24;   // 24 or 48
static int      s_cursor_offset  = 0;    // -view/2 .. +view/2, ignored when no data

static GFont    s_font_header;
static GFont    s_font_body;
static GFont    s_font_small;

static void tide_format_signed_hundredths(int value, const char *unit,
                                          char *out_buf, size_t out_len) {
  int abs_value = abs(value);
  const char *sign = value < 0 ? "-" : "";
  snprintf(out_buf, out_len, "%s%d.%02d%s", sign, abs_value / 100,
           abs_value % 100, unit);
}

static void format_level(uint8_t encoded, char *out_buf, size_t out_len) {
  if (encoded == 0xFF) {
    snprintf(out_buf, out_len, "--");
    return;
  }
  int feet_eighths = (int)encoded - 80;
  if (s_units_meters) {
    tide_format_signed_hundredths((feet_eighths * 381) / 100, "m",
                                  out_buf, out_len);
  } else {
    tide_format_signed_hundredths((feet_eighths * 100) / 8, "ft",
                                  out_buf, out_len);
  }
}

static void format_time_short(char *buf, size_t len, time_t timestamp) {
  if (timestamp <= 0) {
    snprintf(buf, len, "--:--");
    return;
  }
  struct tm *t = localtime(&timestamp);
  if (!t) {
    snprintf(buf, len, "--:--");
    return;
  }
  char tmp[12];
  strftime(tmp, sizeof(tmp), "%I:%M%p", t);
  if (tmp[0] == '0') {
    memmove(tmp, tmp + 1, strlen(tmp));
  }
  snprintf(buf, len, "%s", tmp);
}

static bool tide_has_data(void) {
  if (s_tide_hourly[TIDE_NOW_INDEX] == 0xFF) {
    return false;
  }
  int valid = 0;
  for (int i = 0; i < TIDE_WINDOW_HOURS; i++) {
    if (s_tide_hourly[i] != 0xFF) {
      valid++;
    }
  }
  return valid >= 12;
}

static int clamp_int(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static void clamp_cursor(void) {
  // Cursor is allowed to slide as long as the visible window stays inside
  // the 48-sample buffer.
  int half = s_view_hours / 2;
  int min_offset = -(TIDE_NOW_INDEX - half);
  int max_offset = (TIDE_WINDOW_HOURS - TIDE_NOW_INDEX - half);
  s_cursor_offset = clamp_int(s_cursor_offset, min_offset, max_offset);
}

static void draw_placeholder(GContext *ctx, const char *line1,
                             const char *line2) {
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, line1, s_font_body,
                     GRect(8, 90, SCREEN_W - 16, 28),
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  if (line2 && line2[0]) {
    graphics_draw_text(ctx, line2, s_font_body,
                       GRect(8, 120, SCREEN_W - 16, 28),
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
}

static void draw_chart(GContext *ctx) {
  const int chart_left  = 8;
  const int chart_right = SCREEN_W - 8;
  const int chart_top   = 30;
  const int chart_bot   = 150;
  const int chart_w     = chart_right - chart_left;
  const int chart_h     = chart_bot - chart_top;

  int half = s_view_hours / 2;
  int start = TIDE_NOW_INDEX - half + s_cursor_offset;
  int end   = start + s_view_hours;
  if (start < 0) {
    start = 0;
    end = start + s_view_hours;
  }
  if (end > TIDE_WINDOW_HOURS) {
    end = TIDE_WINDOW_HOURS;
    start = end - s_view_hours;
  }

  // Compute min / max across the visible slice for vertical scaling.
  int min_e = 255, max_e = 0;
  for (int i = start; i < end; i++) {
    uint8_t v = s_tide_hourly[i];
    if (v == 0xFF) continue;
    if (v < min_e) min_e = v;
    if (v > max_e) max_e = v;
  }
  if (max_e <= min_e) max_e = min_e + 1;

  GPoint points[TIDE_WINDOW_HOURS];
  int span = s_view_hours - 1;
  if (span < 1) span = 1;
  for (int i = start; i < end; i++) {
    int slot = i - start;
    uint8_t v = s_tide_hourly[i];
    if (v == 0xFF) {
      points[i] = GPoint(-1, -1);
      continue;
    }
    int x = chart_left + (slot * chart_w) / span;
    int y = chart_bot - ((v - min_e) * chart_h) / (max_e - min_e);
    points[i] = GPoint(x, y);
  }

  // Chart frame
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, GRect(chart_left, chart_top, chart_w, chart_h));

  // Tide curve with thicker stroke for the future side.
  graphics_context_set_stroke_width(ctx, 2);
  for (int i = start; i < end - 1; i++) {
    if (points[i].x < 0 || points[i + 1].x < 0) continue;
    graphics_draw_line(ctx, points[i], points[i + 1]);
  }

  // Dotted vertical "now" indicator (only if "now" is in the visible range).
  if (TIDE_NOW_INDEX >= start && TIDE_NOW_INDEX < end) {
    int slot = TIDE_NOW_INDEX - start;
    int now_x = chart_left + (slot * chart_w) / span;
    graphics_context_set_stroke_width(ctx, 1);
    for (int y = chart_top + 2; y < chart_bot - 2; y += 4) {
      graphics_draw_pixel(ctx, GPoint(now_x, y));
      graphics_draw_pixel(ctx, GPoint(now_x, y + 1));
    }
  }

  // If the cursor has been scrolled away from "now", show a thin solid
  // indicator at the center of the visible window so the user knows where
  // their cursor is pointing.
  if (s_cursor_offset != 0) {
    int cursor_x = chart_left + ((s_view_hours / 2) * chart_w) / span;
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(cursor_x, chart_top - 2),
                       GPoint(cursor_x, chart_top - 6));
  }
}

static void draw_status_footer(GContext *ctx) {
  char now_buf[12];
  char now_line[32];
  char high_line[40];
  char low_line[40];
  char time_buf[12];
  char level_buf[12];

  format_level(s_tide_hourly[TIDE_NOW_INDEX], now_buf, sizeof(now_buf));
  snprintf(now_line, sizeof(now_line), "Now: %s", now_buf);

  format_time_short(time_buf, sizeof(time_buf), (time_t)s_next_high_t);
  format_level(s_next_high_l, level_buf, sizeof(level_buf));
  snprintf(high_line, sizeof(high_line), "High %s  %s", time_buf, level_buf);

  format_time_short(time_buf, sizeof(time_buf), (time_t)s_next_low_t);
  format_level(s_next_low_l, level_buf, sizeof(level_buf));
  snprintf(low_line, sizeof(low_line), "Low  %s  %s", time_buf, level_buf);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, now_line, s_font_body,
                     GRect(8, 152, SCREEN_W - 16, 20),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, high_line, s_font_body,
                     GRect(8, 174, SCREEN_W - 16, 20),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, low_line, s_font_body,
                     GRect(8, 196, SCREEN_W - 16, 20),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void update_proc(Layer *layer, GContext *ctx) {
  (void)layer;

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  // Header: station name on the left, view mode on the right.
  const char *station_label =
      s_station_name[0] ? s_station_name :
      (s_station_id[0] ? s_station_id : "NO STATION");
  char mode_buf[8];
  snprintf(mode_buf, sizeof(mode_buf), "%dh", s_view_hours);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, station_label, s_font_header,
                     GRect(8, 2, SCREEN_W - 60, 22),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, mode_buf, s_font_header,
                     GRect(SCREEN_W - 52, 2, 44, 22),
                     GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);

  if (s_station_id[0] == '\0') {
    draw_placeholder(ctx, "Set a tide station ID",
                     "in Pebble app settings");
    return;
  }

  if (!tide_has_data()) {
    draw_placeholder(ctx, "Waiting for tide data...", "");
    return;
  }

  draw_chart(ctx);
  draw_status_footer(ctx);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  (void)context;
  Tuple *t;

  t = dict_find(iter, MSG_KEY_HOURLY);
  if (t && t->type == TUPLE_BYTE_ARRAY) {
    uint16_t n = t->length;
    if (n > sizeof(s_tide_hourly)) n = sizeof(s_tide_hourly);
    memcpy(s_tide_hourly, t->value->data, n);
    for (size_t i = n; i < sizeof(s_tide_hourly); i++) {
      s_tide_hourly[i] = 0xFF;
    }
    persist_write_data(PERSIST_KEY_HOURLY, s_tide_hourly,
                       sizeof(s_tide_hourly));
  }

  t = dict_find(iter, MSG_KEY_NEXT_HIGH_T);
  if (t) {
    s_next_high_t = (uint32_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NEXT_HIGH_T, (int)s_next_high_t);
  }

  t = dict_find(iter, MSG_KEY_NEXT_HIGH_L);
  if (t) {
    s_next_high_l = (uint8_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NEXT_HIGH_L, (int)s_next_high_l);
  }

  t = dict_find(iter, MSG_KEY_NEXT_LOW_T);
  if (t) {
    s_next_low_t = (uint32_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NEXT_LOW_T, (int)s_next_low_t);
  }

  t = dict_find(iter, MSG_KEY_NEXT_LOW_L);
  if (t) {
    s_next_low_l = (uint8_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NEXT_LOW_L, (int)s_next_low_l);
  }

  t = dict_find(iter, MSG_KEY_STATION_NAME);
  if (t && t->type == TUPLE_CSTRING) {
    strncpy(s_station_name, t->value->cstring, sizeof(s_station_name) - 1);
    s_station_name[sizeof(s_station_name) - 1] = '\0';
    persist_write_string(PERSIST_KEY_STATION_NAME, s_station_name);
  }

  t = dict_find(iter, MSG_KEY_STATION_ID);
  if (t && t->type == TUPLE_CSTRING) {
    strncpy(s_station_id, t->value->cstring, sizeof(s_station_id) - 1);
    s_station_id[sizeof(s_station_id) - 1] = '\0';
    persist_write_string(PERSIST_KEY_STATION_ID, s_station_id);
  }

  t = dict_find(iter, MSG_KEY_UNITS);
  if (t) {
    s_units_meters = ((int)t->value->int32) != 0;
    persist_write_bool(PERSIST_KEY_UNITS_METERS, s_units_meters);
  }

  if (s_root_layer) {
    layer_mark_dirty(s_root_layer);
  }
}

static void load_persisted(void) {
  memset(s_tide_hourly, 0xFF, sizeof(s_tide_hourly));
  if (persist_exists(PERSIST_KEY_HOURLY)) {
    persist_read_data(PERSIST_KEY_HOURLY, s_tide_hourly,
                      sizeof(s_tide_hourly));
  }
  if (persist_exists(PERSIST_KEY_NEXT_HIGH_T)) {
    s_next_high_t = (uint32_t)persist_read_int(PERSIST_KEY_NEXT_HIGH_T);
  }
  if (persist_exists(PERSIST_KEY_NEXT_HIGH_L)) {
    s_next_high_l = (uint8_t)persist_read_int(PERSIST_KEY_NEXT_HIGH_L);
  }
  if (persist_exists(PERSIST_KEY_NEXT_LOW_T)) {
    s_next_low_t = (uint32_t)persist_read_int(PERSIST_KEY_NEXT_LOW_T);
  }
  if (persist_exists(PERSIST_KEY_NEXT_LOW_L)) {
    s_next_low_l = (uint8_t)persist_read_int(PERSIST_KEY_NEXT_LOW_L);
  }
  if (persist_exists(PERSIST_KEY_STATION_NAME)) {
    persist_read_string(PERSIST_KEY_STATION_NAME, s_station_name,
                        sizeof(s_station_name));
  }
  if (persist_exists(PERSIST_KEY_STATION_ID)) {
    persist_read_string(PERSIST_KEY_STATION_ID, s_station_id,
                        sizeof(s_station_id));
  }
  if (persist_exists(PERSIST_KEY_UNITS_METERS)) {
    s_units_meters = persist_read_bool(PERSIST_KEY_UNITS_METERS);
  }
  if (persist_exists(PERSIST_KEY_VIEW_HOURS)) {
    int v = persist_read_int(PERSIST_KEY_VIEW_HOURS);
    s_view_hours = (v == 48) ? 48 : 24;
  }
  if (persist_exists(PERSIST_KEY_CURSOR_OFFSET)) {
    s_cursor_offset = persist_read_int(PERSIST_KEY_CURSOR_OFFSET);
  }
  clamp_cursor();
}

static void save_view_state(void) {
  persist_write_int(PERSIST_KEY_VIEW_HOURS, s_view_hours);
  persist_write_int(PERSIST_KEY_CURSOR_OFFSET, s_cursor_offset);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  (void)recognizer;
  (void)context;
  s_cursor_offset -= 1;
  clamp_cursor();
  save_view_state();
  if (s_root_layer) layer_mark_dirty(s_root_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  (void)recognizer;
  (void)context;
  s_cursor_offset += 1;
  clamp_cursor();
  save_view_state();
  if (s_root_layer) layer_mark_dirty(s_root_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  (void)recognizer;
  (void)context;
  s_view_hours = (s_view_hours == 24) ? 48 : 24;
  s_cursor_offset = 0;
  clamp_cursor();
  save_view_state();
  if (s_root_layer) layer_mark_dirty(s_root_layer);
}

static void click_config_provider(void *context) {
  (void)context;
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  window_set_background_color(window, GColorBlack);

  s_font_header = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_body   = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_small  = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  (void)s_font_small;

  s_root_layer = layer_create(bounds);
  layer_set_update_proc(s_root_layer, update_proc);
  layer_add_child(root, s_root_layer);
}

static void window_unload(Window *window) {
  (void)window;
  if (s_root_layer) {
    layer_destroy(s_root_layer);
    s_root_layer = NULL;
  }
}

static void init(void) {
  load_persisted();

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_set_click_config_provider(s_window, click_config_provider);
  window_stack_push(s_window, true);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(512, 64);
}

static void deinit(void) {
  app_message_deregister_callbacks();
  window_destroy(s_window);
  s_window = NULL;
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
