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
#define PERSIST_KEY_WEATHER_SUMMARY_COMPACT 238
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
#define PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP 171
#define PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_TOMORROW 172
#define PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_YESTERDAY 173
#define PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP 174
#define PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_TOMORROW 175
#define PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_YESTERDAY 176
#define PERSIST_KEY_YOUR_DAY_HALF_HOUR_PIPS 177
#define PERSIST_KEY_EMPTY_EVENT_LABEL 178
#define PERSIST_KEY_TIDE_HOURLY_LEVELS 180
#define PERSIST_KEY_TIDE_NEXT_HIGH_T 181
#define PERSIST_KEY_TIDE_NEXT_HIGH_LEVEL 182
#define PERSIST_KEY_TIDE_NEXT_LOW_T 183
#define PERSIST_KEY_TIDE_NEXT_LOW_LEVEL 184
#define PERSIST_KEY_TIDE_STATION_NAME 185
#define PERSIST_KEY_TIDE_UNITS 186
#define PERSIST_KEY_TIDE_STATION_ID 187
#define PERSIST_KEY_TIDE_DATA_VERSION 192
#define PERSIST_KEY_PRICES_STOCK_1_SYMBOL 193
#define PERSIST_KEY_PRICES_STOCK_2_SYMBOL 194
#define PERSIST_KEY_PRICES_CRYPTO_SYMBOL 195
#define PERSIST_KEY_PRICES_CADENCE_MIN 196
#define PERSIST_KEY_PRICES_POSITIVE_COLOR_LIGHT 197
#define PERSIST_KEY_PRICES_POSITIVE_COLOR_DARK 198
#define PERSIST_KEY_PRICES_NEGATIVE_COLOR_LIGHT 199
#define PERSIST_KEY_PRICES_NEGATIVE_COLOR_DARK 200
#define PERSIST_KEY_PRICES_STOCK_1_PRICE 201
#define PERSIST_KEY_PRICES_STOCK_2_PRICE 202
#define PERSIST_KEY_PRICES_CRYPTO_PRICE 203
#define PERSIST_KEY_PRICES_STOCK_1_DELTA_X100 204
#define PERSIST_KEY_PRICES_STOCK_2_DELTA_X100 205
#define PERSIST_KEY_PRICES_CRYPTO_DELTA_X100 206
#define PERSIST_KEY_PRICES_LAST_UPDATE_T 207
#define PERSIST_KEY_PRICES_SHOW_STOCK_1 208
#define PERSIST_KEY_PRICES_SHOW_STOCK_2 209
#define PERSIST_KEY_PRICES_SHOW_CRYPTO 210
#define PERSIST_KEY_VIBRATE_ON_DISCONNECT 211
#define PERSIST_KEY_TIME_FONT 212
#define PERSIST_KEY_FORECAST_TEMP_F 213
#define PERSIST_KEY_FORECAST_PRECIP_PCT 214
#define PERSIST_KEY_FORECAST_START_T 215
#define PERSIST_KEY_FORECAST_LAST_UPDATE_T 216
#define PERSIST_KEY_CASIO_PHANTOM 217
#define PERSIST_KEY_WEATHER_PROVIDER 218
#define PERSIST_KEY_NWS_HOURLY_TEMPS_F 219
#define PERSIST_KEY_NWS_HOURLY_PRECIP_PCT 220
#define PERSIST_KEY_NWS_HOURLY_START_T 221
#define PERSIST_KEY_NWS_P1_LABEL 222
#define PERSIST_KEY_NWS_P1_SHORT 223
#define PERSIST_KEY_NWS_P1_DETAILED 224
#define PERSIST_KEY_NWS_P1_TEMP 225
#define PERSIST_KEY_NWS_P2_LABEL 226
#define PERSIST_KEY_NWS_P2_SHORT 227
#define PERSIST_KEY_NWS_P2_DETAILED 228
#define PERSIST_KEY_NWS_P2_TEMP 229
#define PERSIST_KEY_NWS_P3_LABEL 230
#define PERSIST_KEY_NWS_P3_SHORT 231
#define PERSIST_KEY_NWS_P3_DETAILED 232
#define PERSIST_KEY_NWS_P3_TEMP 233
#define PERSIST_KEY_NWS_ALERT_TITLE 234
#define PERSIST_KEY_NWS_LOCATION_LABEL 235
#define PERSIST_KEY_NWS_LAST_UPDATE_T 236
#define PERSIST_KEY_NWS_FORECAST_STYLE 237

#define NWS_HOURLY_HOURS 24
#define NWS_LABEL_LEN 24
#define NWS_SHORT_LEN 40
#define NWS_DETAILED_LEN 192
#define NWS_ALERT_LEN 64
#define NWS_LOCATION_LEN 32

typedef enum {
  WeatherProviderOpenMeteo = 0,
  WeatherProviderNws = 1,
} WeatherProvider;

typedef enum {
  NwsForecastStyleChartHeavy = 0,
  NwsForecastStyleNarrative = 1,
} NwsForecastStyle;

static WeatherProvider s_weather_provider = WeatherProviderOpenMeteo;
static NwsForecastStyle s_nws_forecast_style = NwsForecastStyleChartHeavy;

static int8_t s_nws_hourly_temps_f[NWS_HOURLY_HOURS];
static uint8_t s_nws_hourly_precip_pct[NWS_HOURLY_HOURS];
static uint32_t s_nws_hourly_start_t = 0;
static char s_nws_p1_label[NWS_LABEL_LEN];
static char s_nws_p1_short[NWS_SHORT_LEN];
static char s_nws_p1_detailed[NWS_DETAILED_LEN];
static int8_t s_nws_p1_temp = 0;
static char s_nws_p2_label[NWS_LABEL_LEN];
static char s_nws_p2_short[NWS_SHORT_LEN];
static char s_nws_p2_detailed[NWS_DETAILED_LEN];
static int8_t s_nws_p2_temp = 0;
static char s_nws_p3_label[NWS_LABEL_LEN];
static char s_nws_p3_short[NWS_SHORT_LEN];
static char s_nws_p3_detailed[NWS_DETAILED_LEN];
static int8_t s_nws_p3_temp = 0;
static char s_nws_alert_title[NWS_ALERT_LEN];
static char s_nws_location_label[NWS_LOCATION_LEN];
static uint32_t s_nws_last_update_t = 0;

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
#define FORECAST_HOURS 24
#define CASIO_GAP 3
#define CASIO_AMPM_LABEL_W 20
#define WV58A_AMPM_W 12
#define WV58A_AMPM_H 14

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
  ComplicationStock1 = 21,
  ComplicationStock2 = 22,
  ComplicationBitcoin = 23,
} ComplicationType;

typedef enum {
  ShakeBehaviorOff = 0,
  ShakeBehaviorFitnessRings = 1,
  ShakeBehaviorCalendarEvents = 2,
  ShakeBehaviorYourDay = 3,
  ShakeBehaviorDetailedWeather = 4,
  ShakeBehaviorAltTimezone = 5,
  ShakeBehaviorHeartRate = 6,
  ShakeBehaviorPrices = 7,
  ShakeBehaviorTideChart = 8,
  // Value 9 is reserved (was ShakeBehaviorBatteryHistory in 0.88, removed in
  // 0.89). Do not reuse 9 — keeps persisted 0.88 SHAKE_BEHAVIOR=10 picks
  // (Step history) working on upgrade. sanitize_shake_behavior falls 9 to Off.
  ShakeBehaviorStepHistory = 10,
  ShakeBehaviorNwsForecast = 11,
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
static GFont s_font_casio_55;
static GFont s_font_casio_70;
static GFont s_font_casio_90;
static GBitmap *s_step_boot_bitmaps[BitmapThemeCount];
static GBitmap *s_w800_walking_bitmaps[BitmapThemeCount];
static GBitmap *s_quiet_time_mouse_bitmaps[BitmapThemeCount];
static GBitmap *s_w800_digit_bitmaps[BitmapThemeCount][10];
static GBitmap *s_weather_icon_small_bitmaps[BitmapThemeCount][WeatherIconCount];
static GBitmap *s_weather_icon_large_bitmaps[BitmapThemeCount][WeatherIconCount];
static GBitmap *s_icon_bitcoin_bubble;
static GBitmap *s_icon_stocks_bubble;
static GBitmap *s_wv58a_am_bitmap;
static GBitmap *s_wv58a_pm_bitmap;

static char s_date_buf[32];
static char s_time_buf[8];
static char s_time_buf_casio[8];
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
static char s_weather_summary_compact_buf[32] = "";
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
static int8_t s_forecast_temp[FORECAST_HOURS] = {0};
static uint8_t s_forecast_precip[FORECAST_HOURS] = {0};
static time_t s_forecast_start_t = 0;
static time_t s_forecast_last_update_t = 0;
static bool s_forecast_data_loaded = false;
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

typedef enum {
  TIME_FONT_DEFAULT = 0,
  TIME_FONT_CASIO = 1,
  TIME_FONT_ROBOTO = 2,
  TIME_FONT_LECO = 3,
} TimeFont;

static TimeFont s_time_font = TIME_FONT_DEFAULT;
static bool s_casio_phantom = true;
static GFont s_font_roboto;
static GFont s_font_leco;
static bool s_invert_weather = false;
static bool s_invert_meeting_bar = false;
static bool s_vibrate_on_disconnect = false;
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
static int s_day_event_half_hours_first_bitmap = 0;
static int s_day_event_half_hours_first_bitmap_tomorrow = 0;
static int s_day_event_half_hours_first_bitmap_yesterday = 0;
static int s_day_event_half_hours_second_bitmap = 0;
static int s_day_event_half_hours_second_bitmap_tomorrow = 0;
static int s_day_event_half_hours_second_bitmap_yesterday = 0;
static int s_day_event_count_today = 0;
static int s_alt_tz_offset_min = 0;
static int s_your_day_window_mode = 0;
static int s_your_day_window_hours = 10;
static int s_your_day_start_hour = 8;
static int s_your_day_end_hour = 17;
static bool s_your_day_half_hour_pips_enabled = false;
static char s_empty_event_label[32] = "[None]";
// Tide chart sends a 24-byte hourly window centered on "now":
// indices [0..11] = past 12 hours, [TIDE_NOW_INDEX] = current hour,
// [13..23] = next 11 hours. JS keeps the same convention.
#define TIDE_WINDOW_HOURS 24
#define TIDE_NOW_INDEX 12
// Bump TIDE_DATA_VERSION whenever the byte-array layout / window-centering
// changes. On boot, a mismatched persisted version triggers a one-shot wipe
// of the tide bytes so stale data from an older release can't render as a
// misleading partial chart while the fresh fetch is still in flight.
//   1 = 0.72: forward-only 24h window starting at "now"
//   2 = 0.73: centered window with TIDE_NOW_INDEX=12 (12h past + 12h future)
#define TIDE_DATA_VERSION 2

static uint8_t s_tide_hourly_levels[TIDE_WINDOW_HOURS] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
static uint32_t s_tide_next_high_t = 0;
static uint8_t s_tide_next_high_level = 0xFF;
static uint32_t s_tide_next_low_t = 0;
static uint8_t s_tide_next_low_level = 0xFF;
static char s_tide_station_name[24] = "";
static bool s_tide_units_meters = false;
static char s_tide_station_id[16] = "";
static char s_prices_stock_1_symbol[12] = "SPY";
static char s_prices_stock_2_symbol[12] = "QQQ";
static char s_prices_crypto_symbol[16] = "bitcoin";
static char s_prices_stock_1_price[12] = "--";
static char s_prices_stock_2_price[12] = "--";
static char s_prices_crypto_price[12] = "--";
static int s_prices_stock_1_delta_x100 = 0;
static int s_prices_stock_2_delta_x100 = 0;
static int s_prices_crypto_delta_x100 = 0;
static int s_prices_cadence_min = 30;
static bool s_prices_show_stock_1 = true;
static bool s_prices_show_stock_2 = true;
static bool s_prices_show_crypto = true;
static int s_prices_positive_color_light_hex = 0x000000;
static int s_prices_positive_color_dark_hex = 0xFFFFFF;
static int s_prices_negative_color_light_hex = 0x000000;
static int s_prices_negative_color_dark_hex = 0xFFFFFF;
static time_t s_prices_last_update_t = 0;
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

// True when the verbose-weather row should render in two-row "large" mode.
// Honors the user's VERBOSE_WEATHER_STYLE pick literally — one_line stays
// one-line for both providers, large stays large for both providers. PKJS
// adapts the NWS summary string to the chosen layout's character budget.
static bool verbose_weather_layout_is_large(void) {
  return s_verbose_weather_large;
}

static int weather_band_y(void) {
  if (s_verbose_weather_enabled) {
    return verbose_weather_layout_is_large() ? 148 : 162;
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
  return value >= ComplicationTemperature && value <= ComplicationBitcoin
      ? (ComplicationType)value
      : fallback;
}

static ShakeBehavior sanitize_shake_behavior(int value) {
  switch (value) {
    case ShakeBehaviorOff:
    case ShakeBehaviorFitnessRings:
    case ShakeBehaviorCalendarEvents:
    case ShakeBehaviorYourDay:
    case ShakeBehaviorDetailedWeather:
    case ShakeBehaviorAltTimezone:
    case ShakeBehaviorHeartRate:
    case ShakeBehaviorPrices:
    case ShakeBehaviorTideChart:
    case ShakeBehaviorStepHistory:
    case ShakeBehaviorNwsForecast:
      return (ShakeBehavior)value;
    default:
      return ShakeBehaviorOff;
  }
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

static int prices_sanitize_cadence_min(int value) {
  switch (value) {
    case 1:
    case 5:
    case 10:
    case 30:
    case 60:
    case 1440:
      return value;
    default:
      return 30;
  }
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

  // 12 o'clock reference notch -- small outward tick to anchor the
  // "loop closes here" position, so users can read "almost complete"
  // vs "complete" at a glance without comparing arc endpoints.
  graphics_context_set_stroke_color(ctx, fitness_track_color());
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_line(ctx,
                     GPoint(center.x, center.y - radius - 6),
                     GPoint(center.x, center.y - radius - 9));

  if (target < 1) {
    return;
  }
  if (value < 1) {
    // Zero-state indicator: small colored dot at the 12 o'clock start
    // position so "loaded, you're at zero" is distinguishable from
    // "data missing." Uses the metric color so it's clearly tied to
    // this specific ring even when all three are stacked.
    graphics_context_set_fill_color(ctx, color);
    graphics_fill_circle(ctx,
                         GPoint(center.x, center.y - radius),
                         3);
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

  if (s_shake_behavior == ShakeBehaviorFitnessRings) {
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

static GColor casio_phantom_color(void) {
  return section_uses_light_palette(s_draw_section) ? GColorLightGray : GColorDarkGray;
}

static bool casio_ampm_visible(void) {
  return s_ampm_buf[0] != '\0' && !s_military_time_enabled;
}

/*
 * CASIO render uses the SAME GRect layout as the Default and Roboto paths.
 * Pebble's text engine handles horizontal centering; vertical placement is
 * top-of-frame just like the non-CASIO branch. AM/PM anchored to digit bottom.
 * Mouse icon at the standard non-CASIO Y.
 */
static void draw_casio_time_row_at(GContext *ctx, int frame_y, int slot_bottom) {
  (void)slot_bottom;
  GFont font = s_font_casio_55 ? s_font_casio_55 : s_font_time;
  const GRect time_frame = GRect(0, frame_y, SCREEN_W, TIME_FRAME_H);
  const bool show_ampm = casio_ampm_visible();

  // Phantom segments: render all-segments-lit "88:88" backdrop first.
  if (s_casio_phantom) {
    graphics_context_set_text_color(ctx, casio_phantom_color());
    graphics_draw_text(ctx, "88:88", font, time_frame,
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }

  // Real time on top.
  graphics_context_set_text_color(ctx, draw_fg_color());
  graphics_draw_text(ctx, s_time_buf_casio, font, time_frame,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  if (show_ampm) {
    GSize text_size = graphics_text_layout_get_content_size(
        s_time_buf_casio, font, time_frame,
        GTextOverflowModeWordWrap, GTextAlignmentCenter);
    const int ampm_height = 10;
    const int digit_bottom_y = frame_y + text_size.h;
    draw_ampm_label(ctx, s_ampm_buf,
                    GPoint(CASIO_GAP, digit_bottom_y - ampm_height));
  }

  draw_quiet_time_icon(ctx,
                       GPoint(QUIET_TIME_ICON_X,
                              TIME_VISUAL_BOTTOM - QUIET_TIME_ICON_SIZE));
}

static void draw_time_row_at(GContext *ctx, int frame_y, int visual_bottom) {
  if (s_time_font == TIME_FONT_CASIO) {
    draw_casio_time_row_at(ctx, frame_y, visual_bottom);
    return;
  }

  GFont font = s_font_time;
  if (s_time_font == TIME_FONT_ROBOTO && s_font_roboto) {
    font = s_font_roboto;
  } else if (s_time_font == TIME_FONT_LECO && s_font_leco) {
    font = s_font_leco;
  }

  const GRect time_frame = GRect(0, frame_y, SCREEN_W, TIME_FRAME_H);
  const int ampm_width = 20;
  const int ampm_height = 10;
  const int ampm_gap = 5;

  GSize time_size = graphics_text_layout_get_content_size(
      s_time_buf, font, time_frame,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
  int time_left = (SCREEN_W - time_size.w) / 2;
  int ampm_x = time_left - ampm_gap - ampm_width;
  if (ampm_x < 0) {
    ampm_x = 0;
  }

  draw_text(ctx, s_time_buf, font, time_frame,
            draw_fg_color(), GTextAlignmentCenter);
  if (s_ampm_buf[0] != '\0') {
    int ampm_anchor_bottom = visual_bottom;
    if (s_time_font == TIME_FONT_ROBOTO || s_time_font == TIME_FONT_LECO) {
      // Anchor AM/PM bottom to the digit bottom pixel for Roboto and ForecasWatch.
      ampm_anchor_bottom = frame_y + time_size.h;
    }
    draw_ampm_label(ctx, s_ampm_buf, GPoint(ampm_x, ampm_anchor_bottom - ampm_height));
  }

  draw_quiet_time_icon(ctx,
                       GPoint(QUIET_TIME_ICON_X, visual_bottom - QUIET_TIME_ICON_SIZE));
}

static void draw_time_row(GContext *ctx) {
  if (s_time_font == TIME_FONT_CASIO) {
    draw_time_row_at(ctx, TIME_FRAME_Y, weather_band_y());
    return;
  }
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
  uint8_t code = s_weather_code;
  if (code == 0) {
    return WeatherIconSunny;
  } else if (code == 1 || code == 2) {
    return WeatherIconPartlyCloudy;
  } else if (code == 3) {
    return WeatherIconCloudy;
  } else if (code == 45 || code == 48) {
    return WeatherIconFog;
  } else if ((code >= 51 && code <= 67) ||
             (code >= 80 && code <= 82)) {
    return WeatherIconRain;
  } else if ((code >= 71 && code <= 77) ||
             (code >= 85 && code <= 86)) {
    return WeatherIconSnow;
  } else if (code >= 95 && code <= 99) {
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

static void format_ticker_delta(char *buf, size_t buf_len, int delta_x100) {
  int abs_delta = delta_x100 < 0 ? -delta_x100 : delta_x100;
  if (abs_delta > 99999) {
    abs_delta = 99999;
  }
  snprintf(buf, buf_len, "%c%d.%02d%%",
           delta_x100 < 0 ? '-' : '+',
           abs_delta / 100,
           abs_delta % 100);
}

static GColor prices_delta_color(int delta_x100) {
  if (s_color_mode != ColorModeColor) {
    return draw_fg_color();
  }

  bool positive = delta_x100 >= 0;
  int color = positive
      ? (s_light_mode_enabled ? s_prices_positive_color_light_hex
                              : s_prices_positive_color_dark_hex)
      : (s_light_mode_enabled ? s_prices_negative_color_light_hex
                              : s_prices_negative_color_dark_hex);
  return gcolor_from_packed_int(color);
}

static void draw_ticker_bubble(GContext *ctx, GPoint center, GBitmap *icon,
                               int delta_x100) {
  if (icon) {
    GSize sz = gbitmap_get_bounds(icon).size;
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, icon,
                                 GRect(center.x - (sz.w / 2), center.y - 17,
                                       sz.w, sz.h));
  }

  char delta_buf[10];
  format_ticker_delta(delta_buf, sizeof(delta_buf), delta_x100);
  draw_text(ctx, delta_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(center.x - 23, center.y + 3, 46, 16),
            prices_delta_color(delta_x100), GTextAlignmentCenter);
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
             type == ComplicationDistanceToday ||
             type == ComplicationStock1 ||
             type == ComplicationStock2 ||
             type == ComplicationBitcoin) {
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
  } else if (type == ComplicationStock1 || type == ComplicationStock2) {
    draw_ticker_bubble(ctx, center, s_icon_stocks_bubble,
                       type == ComplicationStock1
                           ? s_prices_stock_1_delta_x100
                           : s_prices_stock_2_delta_x100);
    return;
  } else if (type == ComplicationBitcoin) {
    draw_ticker_bubble(ctx, center, s_icon_bitcoin_bubble,
                       s_prices_crypto_delta_x100);
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
      tmp[0] = ' ';
    }
    snprintf(buf, len, "%s", tmp);
  }
}

static void draw_overlay_title(GContext *ctx, const char *title) {
  draw_text(ctx, title, s_font_top, GRect(8, 2, SCREEN_W - 16, 24),
            fitness_muted_text_color(), GTextAlignmentCenter);
}

typedef struct {
  const char *symbol;
  int delta_x100;
} PricesTickerRow;

static void prices_crypto_label(char *buf, size_t len) {
  if (strcmp(s_prices_crypto_symbol, "bitcoin") == 0) {
    snprintf(buf, len, "BTC");
    return;
  }

  size_t i = 0;
  for (; i < len - 1 && s_prices_crypto_symbol[i] != '\0'; i++) {
    char c = s_prices_crypto_symbol[i];
    buf[i] = c >= 'a' && c <= 'z' ? (char)(c - 32) : c;
  }
  buf[i] = '\0';
}

static void prices_draw_hero(GContext *ctx, int y, int h,
                             const PricesTickerRow *row) {
  (void)h;
  char symbol_buf[16];
  snprintf(symbol_buf, sizeof(symbol_buf), "%s:", row->symbol);
  draw_text(ctx, symbol_buf, s_font_top, GRect(8, y + 48, SCREEN_W - 16, 24),
            theme_fg_color(), GTextAlignmentCenter);

  char delta_buf[12];
  format_ticker_delta(delta_buf, sizeof(delta_buf), row->delta_x100);
  draw_text(ctx, delta_buf, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
            GRect(8, y + 78, SCREEN_W - 16, 38),
            prices_delta_color(row->delta_x100), GTextAlignmentCenter);
}

static void prices_draw_medium_row(GContext *ctx, int y, int h,
                                   const PricesTickerRow *row) {
  char delta_buf[12];
  format_ticker_delta(delta_buf, sizeof(delta_buf), row->delta_x100);
  char line_buf[28];
  snprintf(line_buf, sizeof(line_buf), "%s: %s", row->symbol, delta_buf);
  draw_text(ctx, line_buf, s_font_top,
            GRect(8, y + ((h - 24) / 2), SCREEN_W - 16, 24),
            prices_delta_color(row->delta_x100), GTextAlignmentCenter);
}

static void prices_draw_compact_row(GContext *ctx, int y, int h,
                                    const PricesTickerRow *row) {
  char delta_buf[12];
  format_ticker_delta(delta_buf, sizeof(delta_buf), row->delta_x100);
  char line_buf[28];
  snprintf(line_buf, sizeof(line_buf), "%s: %s", row->symbol, delta_buf);
  draw_text(ctx, line_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(8, y + ((h - 22) / 2), SCREEN_W - 16, 22),
            prices_delta_color(row->delta_x100), GTextAlignmentCenter);
}

static void prices_draw_overlay(GContext *ctx) {
  draw_overlay_title(ctx, "PRICES");

  char crypto_label[12];
  prices_crypto_label(crypto_label, sizeof(crypto_label));

  PricesTickerRow rows[3];
  int count = 0;
  if (s_prices_show_stock_1) {
    rows[count++] = (PricesTickerRow) {
      s_prices_stock_1_symbol, s_prices_stock_1_delta_x100
    };
  }
  if (s_prices_show_stock_2) {
    rows[count++] = (PricesTickerRow) {
      s_prices_stock_2_symbol, s_prices_stock_2_delta_x100
    };
  }
  if (s_prices_show_crypto) {
    rows[count++] = (PricesTickerRow) {
      crypto_label, s_prices_crypto_delta_x100
    };
  }

  const int content_top = 28;
  const int content_bottom = SCREEN_H - 24;
  const int content_h = content_bottom - content_top;
  if (count == 0) {
    draw_text(ctx, "No tickers enabled", fonts_get_system_font(FONT_KEY_GOTHIC_18),
              GRect(8, 101, SCREEN_W - 16, 24),
              fitness_muted_text_color(), GTextAlignmentCenter);
  } else if (count == 1) {
    prices_draw_hero(ctx, content_top, content_h, &rows[0]);
  } else if (count == 2) {
    int row_h = content_h / 2;
    prices_draw_medium_row(ctx, content_top, row_h, &rows[0]);
    prices_draw_medium_row(ctx, content_top + row_h, row_h, &rows[1]);
  } else {
    int row_h = content_h / 3;
    prices_draw_compact_row(ctx, content_top, row_h, &rows[0]);
    prices_draw_compact_row(ctx, content_top + row_h, row_h, &rows[1]);
    prices_draw_compact_row(ctx, content_top + (row_h * 2), row_h, &rows[2]);
  }

  char footer[28];
  if (s_prices_last_update_t > 0) {
    struct tm *t = localtime(&s_prices_last_update_t);
    if (t) {
      snprintf(footer, sizeof(footer), "Updated %02d:%02d", t->tm_hour, t->tm_min);
    } else {
      snprintf(footer, sizeof(footer), "Waiting for data...");
    }
  } else {
    snprintf(footer, sizeof(footer), "Waiting for data...");
  }
  draw_text(ctx, footer, fonts_get_system_font(FONT_KEY_GOTHIC_14),
            GRect(8, SCREEN_H - 21, SCREEN_W - 16, 18),
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

static const char *event_display_text(const char *buf) {
  return calendar_title_is_empty(buf) ? s_empty_event_label : buf;
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
    draw_text(ctx, s_empty_event_label, s_font_complication,
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

static bool day_event_half_hour_busy(int day_offset, int hour, bool second_half) {
  int bitmap;
  if (second_half) {
    if (day_offset < 0) {
      bitmap = s_day_event_half_hours_second_bitmap_yesterday;
    } else if (day_offset > 0) {
      bitmap = s_day_event_half_hours_second_bitmap_tomorrow;
    } else {
      bitmap = s_day_event_half_hours_second_bitmap;
    }
  } else {
    if (day_offset < 0) {
      bitmap = s_day_event_half_hours_first_bitmap_yesterday;
    } else if (day_offset > 0) {
      bitmap = s_day_event_half_hours_first_bitmap_tomorrow;
    } else {
      bitmap = s_day_event_half_hours_first_bitmap;
    }
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

static void draw_your_day_hour_pips(GContext *ctx, const char *now_text, GFont now_font) {
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
  const int elbow_y = y + 43;
  GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  int current_x = -1;

  for (int i = 0; i < pip_count; i++) {
    int hour = start_hour + i;
    int day_offset = 0;
    normalize_display_hour(&hour, &day_offset);
    int x = start_x + (i * gap_x);
    bool current = day_offset == 0 && hour == current_hour;

    if (current) {
      current_x = x;
    }

    graphics_context_set_stroke_color(ctx, theme_fg_color());
    graphics_context_set_fill_color(ctx, theme_fg_color());
    graphics_context_set_stroke_width(ctx, 1);

    if (s_your_day_half_hour_pips_enabled) {
      bool busy_first = day_event_half_hour_busy(day_offset, hour, false);
      bool busy_second = day_event_half_hour_busy(day_offset, hour, true);
      const GRect pip_frame = GRect(x - 5, y - 5, 11, 11);
      if (busy_first && busy_second) {
        graphics_fill_circle(ctx, GPoint(x, y), 5);
      } else if (busy_first) {
        // Left half (9 o'clock side). Pebble angles: 0=top, clockwise.
        // Sweep from 180 (bottom) clockwise through 270 (left) to 360 (top).
        graphics_fill_radial(ctx, pip_frame, GOvalScaleModeFitCircle, 11,
                             DEG_TO_TRIGANGLE(180), DEG_TO_TRIGANGLE(360));
        graphics_draw_circle(ctx, GPoint(x, y), 5);
      } else if (busy_second) {
        // Right half (3 o'clock side). 0 (top) clockwise through 90 to 180 (bottom).
        graphics_fill_radial(ctx, pip_frame, GOvalScaleModeFitCircle, 11,
                             DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(180));
        graphics_draw_circle(ctx, GPoint(x, y), 5);
      } else {
        graphics_draw_circle(ctx, GPoint(x, y), 5);
      }
    } else {
      bool busy = day_event_hour_busy(day_offset, hour);
      if (busy) {
        graphics_fill_circle(ctx, GPoint(x, y), 5);
      } else {
        graphics_draw_circle(ctx, GPoint(x, y), 5);
      }
    }

    if (!current) {
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

  if (current_x < 0) {
    return;
  }

  GSize now_size = graphics_text_layout_get_content_size(
      now_text ? now_text : "", now_font, GRect(0, 0, SCREEN_W - 16, 22),
      GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
  int now_left = (SCREEN_W - now_size.w) / 2;
  int now_right = now_left + now_size.w;
  if (now_left < 8) {
    now_left = 8;
  }
  if (now_right > SCREEN_W - 8) {
    now_right = SCREEN_W - 8;
  }

  const int target_x = current_x <= ((now_left + now_right) / 2)
      ? now_left - 4
      : now_right + 4;

  graphics_context_set_stroke_color(ctx, theme_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  // Drop down from the bottom edge of the current-hour pip (y + 5) with a
  // 1-pixel gap so the line doesn't overlap the pip outline.
  graphics_draw_line(ctx, GPoint(current_x, y + 7), GPoint(current_x, elbow_y));
  graphics_draw_line(ctx, GPoint(current_x, elbow_y), GPoint(target_x, elbow_y));
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
    snprintf(title_buf, sizeof(title_buf), "FUTURE");
  }
  draw_text(ctx, title_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(8, 64, SCREEN_W - 16, 18),
            fitness_muted_text_color(), GTextAlignmentCenter);

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
  GFont now_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  draw_your_day_hour_pips(ctx, now_buf, now_font);
  draw_text(ctx, now_buf, now_font, GRect(8, 126, SCREEN_W - 16, 22),
            theme_fg_color(), GTextAlignmentCenter);

  graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
  graphics_draw_line(ctx, GPoint(8, 154), GPoint(192, 154));

  char count_buf[28];
  snprintf(count_buf, sizeof(count_buf), "%d events in window", s_day_event_count_today);
  draw_text(ctx, count_buf, s_font_complication, GRect(8, 162, SCREEN_W - 16, 24),
            theme_fg_color(), GTextAlignmentCenter);

  char next_buf[96];
  snprintf(next_buf, sizeof(next_buf), "Next: %s", event_display_text(s_event_buf));
  draw_text(ctx, next_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(8, 190, SCREEN_W - 16, 28), fitness_muted_text_color(),
            GTextAlignmentCenter);
}

static bool forecast_has_data(void) {
  return s_forecast_data_loaded && s_forecast_start_t > 0 &&
      s_forecast_last_update_t > 0;
}

static int forecast_x_for_index(GRect plot, int index) {
  if (index < 0) {
    index = 0;
  } else if (index >= FORECAST_HOURS) {
    index = FORECAST_HOURS - 1;
  }
  return plot.origin.x + (index * (plot.size.w - 1)) / (FORECAST_HOURS - 1);
}

static int forecast_x_for_time(GRect plot, time_t t) {
  if (s_forecast_start_t <= 0 || t <= 0) {
    return -1;
  }

  int span_s = (FORECAST_HOURS - 1) * 60 * 60;
  int delta_s = (int)(t - s_forecast_start_t);
  if (delta_s < 0 || delta_s > span_s) {
    return -1;
  }
  return plot.origin.x + (delta_s * (plot.size.w - 1)) / span_s;
}

static int forecast_temp_y(int temp, int min_temp, int max_temp, GRect plot) {
  int range = max_temp - min_temp;
  if (range < 1) {
    range = 1;
  }
  int usable_h = plot.size.h - 8;
  int rel = ((temp - min_temp) * usable_h) / range;
  return plot.origin.y + plot.size.h - 4 - rel;
}

// Faithful port of forecaswatch2's night-hatch pattern (see
// forecast_layer.c::draw_night_hatch_rect): one pixel every NIGHT_HATCH_SPACING
// in both axes, aligned via (x + y_start) % spacing so the diagonals stay
// continuous across adjacent night segments.
#define FCW_NIGHT_HATCH_SPACING 6

static int fcw_aligned_hatch_start_y(int x, int y_start, int spacing) {
  int modulo = ((x + y_start) % spacing + spacing) % spacing;
  if (modulo == 0) {
    return y_start;
  }
  return y_start + (spacing - modulo);
}

static void fcw_hatch_rect(GContext *ctx, GRect rect, int spacing) {
  if (spacing <= 0 || rect.size.w <= 0 || rect.size.h <= 0) {
    return;
  }
  int x_end = rect.origin.x + rect.size.w;
  int y_end = rect.origin.y + rect.size.h;
  for (int x = rect.origin.x; x < x_end; x++) {
    int hatch_y = fcw_aligned_hatch_start_y(x, rect.origin.y, spacing);
    for (int y = hatch_y; y < y_end; y += spacing) {
      graphics_draw_pixel(ctx, GPoint(x, y));
    }
  }
}

// Returns up to 2 night segments inside the 24-hour forecast window using the
// stored sunrise_t / sunset_t. Forecaswatch2 supports 3 day-offsets; we have
// one sunrise/sunset pair, so the segments are: pre-sunrise night and
// post-sunset night within the window.
typedef struct {
  time_t start;
  time_t end;
} FcwNightSegment;

static int fcw_compute_night_segments(time_t graph_start, time_t graph_end,
                                       FcwNightSegment *out) {
  int count = 0;
  if (!s_sunrise_known || !s_sunset_known ||
      s_sunrise_t <= 0 || s_sunset_t <= 0 ||
      s_sunset_t <= s_sunrise_t) {
    return 0;
  }

  // Pre-sunrise night
  if (graph_start < s_sunrise_t) {
    out[count].start = graph_start;
    out[count].end = (s_sunrise_t < graph_end) ? s_sunrise_t : graph_end;
    count++;
  }
  // Post-sunset night
  if (s_sunset_t < graph_end && count < 2) {
    out[count].start = (s_sunset_t > graph_start) ? s_sunset_t : graph_start;
    out[count].end = graph_end;
    count++;
  }
  return count;
}

static int fcw_x_for_time(time_t t, time_t graph_start, time_t graph_end, GRect plot) {
  if (t <= graph_start) {
    return plot.origin.x;
  }
  if (t >= graph_end) {
    return plot.origin.x + plot.size.w;
  }
  int64_t elapsed = (int64_t)t - graph_start;
  int64_t total = (int64_t)graph_end - graph_start;
  return plot.origin.x + (int)((elapsed * plot.size.w) / total);
}

static void draw_forecast_graph(GContext *ctx, GRect bounds) {
  if (!forecast_has_data()) {
    draw_text(ctx, "Waiting for forecast...",
              fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
              bounds, fitness_muted_text_color(), GTextAlignmentCenter);
    return;
  }

  // Layout: leave 12 px on bottom for hour labels, no border drawn.
  const int bottom_axis_h = 12;
  GRect plot = GRect(bounds.origin.x, bounds.origin.y,
                    bounds.size.w, bounds.size.h - bottom_axis_h);

  // Min/max temp with a minimum range of 10.
  int min_temp = s_forecast_temp[0];
  int max_temp = s_forecast_temp[0];
  for (int i = 1; i < FORECAST_HOURS; i++) {
    if (s_forecast_temp[i] < min_temp) min_temp = s_forecast_temp[i];
    if (s_forecast_temp[i] > max_temp) max_temp = s_forecast_temp[i];
  }
  if (max_temp - min_temp < 10) {
    int missing = 10 - (max_temp - min_temp);
    min_temp -= missing / 2;
    max_temp += missing - (missing / 2);
  }

  const time_t graph_start = s_forecast_start_t;
  const time_t graph_end = graph_start + (FORECAST_HOURS - 1) * 60 * 60;
  const int plot_bottom = plot.origin.y + plot.size.h - 1;

  // 1. Night regions (hatched background) first, behind everything.
  FcwNightSegment night[2];
  int night_count = fcw_compute_night_segments(graph_start, graph_end, night);
  if (night_count > 0) {
    graphics_context_set_stroke_color(ctx, GColorLightGray);
    for (int i = 0; i < night_count; i++) {
      int x0 = fcw_x_for_time(night[i].start, graph_start, graph_end, plot);
      int x1 = fcw_x_for_time(night[i].end, graph_start, graph_end, plot);
      if (x1 <= x0) continue;
      GRect night_rect = GRect(x0, plot.origin.y, x1 - x0, plot.size.h);
      fcw_hatch_rect(ctx, night_rect, FCW_NIGHT_HATCH_SPACING);
    }
  }

  // 2. Precipitation filled area under the curve.
  GPoint precip_points[FORECAST_HOURS + 2];
  precip_points[0] = GPoint(plot.origin.x, plot_bottom);
  for (int i = 0; i < FORECAST_HOURS; i++) {
    int x = forecast_x_for_index(plot, i);
    int precip_h = (s_forecast_precip[i] * (plot.size.h - 3)) / 100;
    precip_points[i + 1] = GPoint(x, plot_bottom - precip_h);
  }
  precip_points[FORECAST_HOURS + 1] =
      GPoint(plot.origin.x + plot.size.w - 1, plot_bottom);
  GPathInfo precip_info = {
    .num_points = FORECAST_HOURS + 2,
    .points = precip_points
  };
  GPath *precip_path = gpath_create(&precip_info);
  if (precip_path) {
    graphics_context_set_fill_color(ctx, GColorPictonBlue);
    gpath_draw_filled(ctx, precip_path);
    gpath_destroy(precip_path);
  }

  // 3. Sunrise / sunset boundary lines (thin vertical, full plot height).
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  if (s_sunrise_known && s_sunrise_t > graph_start && s_sunrise_t < graph_end) {
    int sx = fcw_x_for_time(s_sunrise_t, graph_start, graph_end, plot);
    graphics_draw_line(ctx, GPoint(sx, plot.origin.y), GPoint(sx, plot_bottom));
  }
  if (s_sunset_known && s_sunset_t > graph_start && s_sunset_t < graph_end) {
    int sx = fcw_x_for_time(s_sunset_t, graph_start, graph_end, plot);
    graphics_draw_line(ctx, GPoint(sx, plot.origin.y), GPoint(sx, plot_bottom));
  }

  // 4. Temperature line on top, stroke 3 like forecaswatch2.
  GPoint temp_points[FORECAST_HOURS];
  for (int i = 0; i < FORECAST_HOURS; i++) {
    temp_points[i] = GPoint(forecast_x_for_index(plot, i),
                            forecast_temp_y(s_forecast_temp[i], min_temp, max_temp, plot));
  }
  GPathInfo temp_info = {
    .num_points = FORECAST_HOURS,
    .points = temp_points
  };
  GPath *temp_path = gpath_create(&temp_info);
  if (temp_path) {
    graphics_context_set_stroke_color(ctx, GColorRed);
    graphics_context_set_stroke_width(ctx, 3);
    gpath_draw_outline_open(ctx, temp_path);
    gpath_destroy(temp_path);
  }
  graphics_context_set_stroke_width(ctx, 1);

  // 5. Hour labels along bottom (0, 3, 6, ... 21) using the forecast's local
  // start hour, not 0. Tick marks only anchor labeled hours.
  GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  struct tm *start_tm = localtime(&graph_start);
  int start_hour = start_tm ? start_tm->tm_hour : 0;
  graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
  for (int i = 0; i < FORECAST_HOURS; i++) {
    int x = forecast_x_for_index(plot, i);
    int is_label = (i % 3) == 0;
    if (!is_label) {
      continue;
    }
    int tick_h = 4;
    graphics_draw_line(ctx, GPoint(x, plot_bottom + 1),
                       GPoint(x, plot_bottom + 1 + tick_h));
    char label[4];
    snprintf(label, sizeof(label), "%d", (start_hour + i) % 24);
    draw_text(ctx, label, label_font, GRect(x - 12, plot_bottom + 4, 24, 14),
              fitness_muted_text_color(), GTextAlignmentCenter);
  }
}

static void draw_small_sun(GContext *ctx, GPoint center, GColor color) {
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_fill_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, center, 4);
  graphics_fill_circle(ctx, center, 2);
  graphics_draw_line(ctx, GPoint(center.x - 8, center.y), GPoint(center.x - 6, center.y));
  graphics_draw_line(ctx, GPoint(center.x + 6, center.y), GPoint(center.x + 8, center.y));
  graphics_draw_line(ctx, GPoint(center.x, center.y - 8), GPoint(center.x, center.y - 6));
  graphics_draw_line(ctx, GPoint(center.x, center.y + 6), GPoint(center.x, center.y + 8));
  graphics_draw_line(ctx, GPoint(center.x - 6, center.y - 6),
                     GPoint(center.x - 4, center.y - 4));
  graphics_draw_line(ctx, GPoint(center.x + 4, center.y + 4),
                     GPoint(center.x + 6, center.y + 6));
  graphics_draw_line(ctx, GPoint(center.x + 6, center.y - 6),
                     GPoint(center.x + 4, center.y - 4));
  graphics_draw_line(ctx, GPoint(center.x - 4, center.y + 4),
                     GPoint(center.x - 6, center.y + 6));
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
  format_time_short(sunrise_buf, sizeof(sunrise_buf), s_sunrise_t, s_sunrise_known);
  format_time_short(sunset_buf, sizeof(sunset_buf), s_sunset_t, s_sunset_known);

  draw_weather_icon_centered(ctx, GPoint(100, 24), true);
  draw_text(ctx, temp_buf, s_font_time, GRect(0, 48, SCREEN_W, 48),
            theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, feels_buf, s_font_complication, GRect(8, 91, SCREEN_W - 16, 22),
            fitness_muted_text_color(), GTextAlignmentCenter);
  draw_text(ctx, forecast_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(6, 112, SCREEN_W - 12, 20), theme_fg_color(), GTextAlignmentCenter);

  draw_forecast_graph(ctx, GRect(4, 132, 192, 62));

  draw_small_sun(ctx, GPoint(13, 209), theme_fg_color());
  draw_text(ctx, sunrise_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(24, 199, 72, 24), theme_fg_color(), GTextAlignmentLeft);
  draw_small_moon(ctx, GPoint(110, 209), theme_fg_color());
  draw_text(ctx, sunset_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(121, 199, 76, 24), theme_fg_color(), GTextAlignmentLeft);
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

static void tide_format_signed_hundredths(int value, const char *unit,
                                          char *out_buf, size_t out_len) {
  int abs_value = abs(value);
  const char *sign = value < 0 ? "-" : "";
  snprintf(out_buf, out_len, "%s%d.%02d%s", sign, abs_value / 100,
           abs_value % 100, unit);
}

static void tide_format_level(uint8_t encoded, char *out_buf, size_t out_len) {
  if (encoded == 0xFF) {
    snprintf(out_buf, out_len, "--");
    return;
  }

  int feet_eighths = (int)encoded - 80;
  if (s_tide_units_meters) {
    tide_format_signed_hundredths((feet_eighths * 381) / 100, "m",
                                  out_buf, out_len);
  } else {
    tide_format_signed_hundredths((feet_eighths * 100) / 8, "ft",
                                  out_buf, out_len);
  }
}

static bool tide_has_hourly_data(void) {
  // Require the current-hour sample plus enough surrounding context before
  // committing to draw the chart. A partial array (e.g. stale data left over
  // from an older release that only populated a few hours) renders as a
  // misleading "single spike". Better to keep the Waiting placeholder until
  // the next NOAA fetch lands a full window.
  if (s_tide_hourly_levels[TIDE_NOW_INDEX] == 0xFF) {
    return false;
  }
  int valid = 0;
  for (int i = 0; i < TIDE_WINDOW_HOURS; i++) {
    if (s_tide_hourly_levels[i] != 0xFF) {
      valid++;
    }
  }
  return valid >= 12;
}

static void tide_draw_placeholder(GContext *ctx, const char *line1,
                                  const char *line2) {
  draw_text(ctx, line1, s_font_complication, GRect(8, 88, SCREEN_W - 16, 28),
            theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, line2, s_font_complication, GRect(8, 116, SCREEN_W - 16, 28),
            fitness_muted_text_color(), GTextAlignmentCenter);
}

static void tide_format_event_line(char *out_buf, size_t out_len,
                                   const char *label, uint32_t timestamp,
                                   uint8_t level) {
  char time_buf[12];
  char level_buf[12];
  format_time_short(time_buf, sizeof(time_buf), (time_t)timestamp, timestamp > 0);
  tide_format_level(level, level_buf, sizeof(level_buf));
  snprintf(out_buf, out_len, "%s %s  %s", label, time_buf, level_buf);
}

static void tide_chart_draw_overlay(GContext *ctx) {
  const int chart_left = 8;
  const int chart_right = SCREEN_W - 8;
  const int chart_top = 24;
  const int chart_bottom = 140;
  const int chart_h = chart_bottom - chart_top;

  if (s_tide_station_id[0] == '\0') {
    tide_draw_placeholder(ctx, "Set a tide station ID", "in Pebble app settings");
    return;
  }

  const char *station_label =
      s_tide_station_name[0] ? s_tide_station_name : s_tide_station_id;
  draw_text(ctx, "TIDE", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(8, 4, 60, 16), fitness_muted_text_color(), GTextAlignmentLeft);
  draw_text(ctx, station_label, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(60, 4, SCREEN_W - 68, 16), fitness_muted_text_color(),
            GTextAlignmentRight);

  if (!tide_has_hourly_data()) {
    tide_draw_placeholder(ctx, "Waiting for tide data...", "");
    return;
  }

  int min_e = 255;
  int max_e = 0;
  for (int i = 0; i < TIDE_WINDOW_HOURS; i++) {
    if (s_tide_hourly_levels[i] == 0xFF) {
      continue;
    }
    if (s_tide_hourly_levels[i] < min_e) {
      min_e = s_tide_hourly_levels[i];
    }
    if (s_tide_hourly_levels[i] > max_e) {
      max_e = s_tide_hourly_levels[i];
    }
  }
  if (max_e == min_e) {
    max_e = min_e + 1;
  }

  GPoint points[TIDE_WINDOW_HOURS];
  for (int i = 0; i < TIDE_WINDOW_HOURS; i++) {
    if (s_tide_hourly_levels[i] == 0xFF) {
      points[i] = GPoint(-1, -1);
      continue;
    }
    int x = chart_left + (i * (chart_right - chart_left)) / (TIDE_WINDOW_HOURS - 1);
    int y = chart_bottom
        - ((s_tide_hourly_levels[i] - min_e) * chart_h) / (max_e - min_e);
    points[i] = GPoint(x, y);
  }

  // Dotted vertical "now" indicator at the TIDE_NOW_INDEX column. Show it
  // whether or not that exact sample has data. The user still wants to see
  // where "now" is on the timeline.
  int now_x;
  if (points[TIDE_NOW_INDEX].x >= 0) {
    now_x = points[TIDE_NOW_INDEX].x;
  } else {
    now_x = chart_left + (TIDE_NOW_INDEX * (chart_right - chart_left))
        / (TIDE_WINDOW_HOURS - 1);
  }
  graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
  graphics_context_set_stroke_width(ctx, 1);
  for (int y = chart_top; y < chart_bottom; y += 4) {
    graphics_draw_pixel(ctx, GPoint(now_x, y));
    graphics_draw_pixel(ctx, GPoint(now_x, y + 1));
  }

  // Past hours (indices < TIDE_NOW_INDEX) draw in muted color; future hours
  // draw in the strong theme foreground so "what's coming" reads as primary.
  graphics_context_set_stroke_width(ctx, 2);
  for (int i = 0; i < TIDE_WINDOW_HOURS - 1; i++) {
    if (points[i].x < 0 || points[i + 1].x < 0) {
      continue;
    }
    GColor stroke = (i < TIDE_NOW_INDEX) ? fitness_muted_text_color() : theme_fg_color();
    graphics_context_set_stroke_color(ctx, stroke);
    graphics_draw_line(ctx, points[i], points[i + 1]);
  }

  char level_buf[12];
  char now_line[24];
  char high_line[32];
  char low_line[32];
  tide_format_level(s_tide_hourly_levels[TIDE_NOW_INDEX], level_buf,
                    sizeof(level_buf));
  snprintf(now_line, sizeof(now_line), "Now %s", level_buf);
  tide_format_event_line(high_line, sizeof(high_line), "High",
                         s_tide_next_high_t, s_tide_next_high_level);
  tide_format_event_line(low_line, sizeof(low_line), "Low ",
                         s_tide_next_low_t, s_tide_next_low_level);

  draw_text(ctx, now_line, s_font_complication, GRect(8, 148, SCREEN_W - 16, 20),
            theme_fg_color(), GTextAlignmentLeft);
  draw_text(ctx, high_line, s_font_complication, GRect(8, 172, SCREEN_W - 16, 20),
            theme_fg_color(), GTextAlignmentLeft);
  draw_text(ctx, low_line, s_font_complication, GRect(8, 196, SCREEN_W - 16, 20),
            fitness_muted_text_color(), GTextAlignmentLeft);
}

// Pebble tm_wday: 0=Sun .. 6=Sat. Map to label table {M,T,W,T,F,S,S}.
// Mon->0, Tue->1, Wed->2, Thu->3, Fri->4, Sat->5, Sun->6.
static int shake_dow_to_label_idx(int dow) {
  return (dow + 6) % 7;
}

// "!" alert badge rendered as a 14x14 inset square with a centered "!".
// Mirrors the style Codex used on the approved mockups so the live render
// matches the visual reference.
static void nws_draw_alert_badge(GContext *ctx, GRect frame) {
  if (s_nws_alert_title[0] == '\0') return;
  graphics_context_set_stroke_color(ctx, theme_fg_color());
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, frame);
  draw_text(ctx, "!", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(frame.origin.x, frame.origin.y - 1,
                  frame.size.w, frame.size.h),
            theme_fg_color(), GTextAlignmentCenter);
}

static void nws_draw_header(GContext *ctx) {
  const char *location = s_nws_location_label[0]
      ? s_nws_location_label : "NWS FORECAST";
  draw_text(ctx, location, s_font_top, GRect(8, 0, SCREEN_W - 38, 22),
            theme_fg_color(), GTextAlignmentLeft);
  nws_draw_alert_badge(ctx, GRect(SCREEN_W - 24, 4, 16, 16));
}

// Render the dual-line chart (24h temp + 24h precip%) used by the Chart
// Heavy layout. `frame` is the chart-only rectangle (no labels — labels
// are positioned by the caller). Temp scales to its own min/max range;
// precip scales 0-100% fixed.
static void nws_draw_dual_chart(GContext *ctx, GRect frame) {
  const int n = NWS_HOURLY_HOURS;
  int t_min = 127, t_max = -127;
  for (int i = 0; i < n; i++) {
    int v = (int)s_nws_hourly_temps_f[i];
    if (v < t_min) t_min = v;
    if (v > t_max) t_max = v;
  }
  if (t_max <= t_min) t_max = t_min + 1;

  // Pad the temp range so the line doesn't kiss the chart frame.
  int span = t_max - t_min;
  int pad = (span < 6) ? 2 : (span / 6);
  if (pad < 1) pad = 1;
  int y_min = t_min - pad;
  int y_max = t_max + pad;

  if (s_nws_hourly_start_t > 0) {
    time_t graph_start = (time_t)s_nws_hourly_start_t;
    time_t graph_end = graph_start + (n - 1) * 60 * 60;
    int64_t total = (int64_t)graph_end - graph_start;
    graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
    graphics_context_set_stroke_width(ctx, 1);
    if (s_sunrise_known && s_sunrise_t > graph_start && s_sunrise_t < graph_end) {
      int64_t elapsed = (int64_t)s_sunrise_t - graph_start;
      int sx = frame.origin.x + (int)((elapsed * (frame.size.w - 1)) / total);
      graphics_draw_line(ctx, GPoint(sx, frame.origin.y),
                         GPoint(sx, frame.origin.y + frame.size.h - 1));
    }
    if (s_sunset_known && s_sunset_t > graph_start && s_sunset_t < graph_end) {
      int64_t elapsed = (int64_t)s_sunset_t - graph_start;
      int sx = frame.origin.x + (int)((elapsed * (frame.size.w - 1)) / total);
      graphics_draw_line(ctx, GPoint(sx, frame.origin.y),
                         GPoint(sx, frame.origin.y + frame.size.h - 1));
    }
  }

  // Precip line — blue, solid. Drawn first so temp overlays it.
  graphics_context_set_stroke_color(ctx, GColorPictonBlue);
  graphics_context_set_stroke_width(ctx, 2);
  for (int i = 0; i < n - 1; i++) {
    int x1 = frame.origin.x + (i * (frame.size.w - 1)) / (n - 1);
    int x2 = frame.origin.x + ((i + 1) * (frame.size.w - 1)) / (n - 1);
    int p1 = s_nws_hourly_precip_pct[i];
    int p2 = s_nws_hourly_precip_pct[i + 1];
    if (p1 > 100) p1 = 100;
    if (p2 > 100) p2 = 100;
    int y1 = frame.origin.y + frame.size.h
        - (p1 * frame.size.h) / 100;
    int y2 = frame.origin.y + frame.size.h
        - (p2 * frame.size.h) / 100;
    graphics_draw_line(ctx, GPoint(x1, y1), GPoint(x2, y2));
  }
  graphics_context_set_stroke_width(ctx, 1);

  // Temp line — deep red. GColorDarkCandyAppleRed (#AA0000) reads as a
  // clear red on emery without the neon/pink cast GColorRed picks up
  // against the dark overlay background.
  graphics_context_set_stroke_color(ctx, GColorDarkCandyAppleRed);
  graphics_context_set_stroke_width(ctx, 2);
  for (int i = 0; i < n - 1; i++) {
    int x1 = frame.origin.x + (i * (frame.size.w - 1)) / (n - 1);
    int x2 = frame.origin.x + ((i + 1) * (frame.size.w - 1)) / (n - 1);
    int t1 = (int)s_nws_hourly_temps_f[i];
    int t2 = (int)s_nws_hourly_temps_f[i + 1];
    int y1 = frame.origin.y + frame.size.h
        - ((t1 - y_min) * frame.size.h) / (y_max - y_min);
    int y2 = frame.origin.y + frame.size.h
        - ((t2 - y_min) * frame.size.h) / (y_max - y_min);
    graphics_draw_line(ctx, GPoint(x1, y1), GPoint(x2, y2));
  }
  graphics_context_set_stroke_width(ctx, 1);
}

// Compact NWS period label for tight one-line contexts (the Chart Heavy
// sub-header). The full label is preserved in s_nws_p1_label and rendered
// untruncated in Narrative Weather. NWS-canonical labels are uppercase.
static void nws_compact_period_label(const char *src, char *dst, size_t dst_len) {
  if (!src || !dst || dst_len == 0) return;
  // Order longest-first so prefixes don't shadow more-specific matches.
  static const struct { const char *full; const char *abbr; } kMap[] = {
    {"THIS AFTERNOON",  "AFTRN"},
    {"THIS MORNING",    "MORN"},
    {"THIS EVENING",    "EVE"},
    {"OVERNIGHT",       "OVNT"},
    {"TONIGHT",         "TNGHT"},
    {"TODAY",           "TODAY"},
    {"MONDAY NIGHT",    "MON NT"},
    {"TUESDAY NIGHT",   "TUE NT"},
    {"WEDNESDAY NIGHT", "WED NT"},
    {"THURSDAY NIGHT",  "THU NT"},
    {"FRIDAY NIGHT",    "FRI NT"},
    {"SATURDAY NIGHT",  "SAT NT"},
    {"SUNDAY NIGHT",    "SUN NT"},
    {"MONDAY",          "MON"},
    {"TUESDAY",         "TUE"},
    {"WEDNESDAY",       "WED"},
    {"THURSDAY",        "THU"},
    {"FRIDAY",          "FRI"},
    {"SATURDAY",        "SAT"},
    {"SUNDAY",          "SUN"},
  };
  for (size_t i = 0; i < sizeof(kMap) / sizeof(kMap[0]); i++) {
    if (strcmp(src, kMap[i].full) == 0) {
      strncpy(dst, kMap[i].abbr, dst_len - 1);
      dst[dst_len - 1] = '\0';
      return;
    }
  }
  strncpy(dst, src, dst_len - 1);
  dst[dst_len - 1] = '\0';
}

// Layout A — Chart Heavy. Header + sub-header + dual-line chart + hour
// legend (right below chart) + narrative block at the bottom.
static void nws_forecast_chart_heavy_draw(GContext *ctx) {
  nws_draw_header(ctx);

  char compact_label[12];
  nws_compact_period_label(s_nws_p1_label, compact_label,
                           sizeof(compact_label));
  char sub[40];
  snprintf(sub, sizeof(sub), "%s  %d\xC2\xB0""F  H%d\xC2\xB0""/L%d\xC2\xB0",
           compact_label, (int)s_nws_hourly_temps_f[0],
           (int)s_nws_p1_temp, (int)s_nws_p2_temp);
  draw_text(ctx, sub, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(8, 22, SCREEN_W - 16, 18),
            theme_fg_color(), GTextAlignmentLeft);

  const GRect chart = GRect(28, 42, SCREEN_W - 56, 90);

  // Y-axis temp labels (left), aligned to chart's top / bottom.
  int t_min = 127, t_max = -127;
  for (int i = 0; i < NWS_HOURLY_HOURS; i++) {
    int v = (int)s_nws_hourly_temps_f[i];
    if (v < t_min) t_min = v;
    if (v > t_max) t_max = v;
  }
  if (t_max <= t_min) t_max = t_min + 1;
  char y_lbl[8];
  snprintf(y_lbl, sizeof(y_lbl), "%d", t_max);
  draw_text(ctx, y_lbl, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(2, 38, 24, 16),
            fitness_muted_text_color(), GTextAlignmentRight);
  snprintf(y_lbl, sizeof(y_lbl), "%d", t_min);
  draw_text(ctx, y_lbl, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(2, 118, 24, 16),
            fitness_muted_text_color(), GTextAlignmentRight);

  // Y-axis precip labels (right), aligned to chart's top / bottom.
  draw_text(ctx, "100", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(SCREEN_W - 28, 38, 26, 16),
            fitness_muted_text_color(), GTextAlignmentLeft);
  draw_text(ctx, "0", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(SCREEN_W - 28, 118, 26, 16),
            fitness_muted_text_color(), GTextAlignmentLeft);

  nws_draw_dual_chart(ctx, chart);

  // Hour legend immediately below the chart — anchors the X axis instead of
  // being separated from the chart by the narrative block.
  if (s_nws_hourly_start_t > 0) {
    time_t base = (time_t)s_nws_hourly_start_t;
    const int idxs[6] = {0, 4, 8, 12, 16, 20};
    const int chart_left = 28;
    const int chart_w = SCREEN_W - 56;
    for (int i = 0; i < 6; i++) {
      int idx = idxs[i];
      time_t t = base + idx * 3600;
      struct tm *tm_t = localtime(&t);
      int hh = tm_t ? tm_t->tm_hour : 0;
      int disp = hh % 12; if (disp == 0) disp = 12;
      const char *ampm = (hh < 12) ? "A" : "P";
      char label[6];
      snprintf(label, sizeof(label), "%d%s", disp, ampm);
      int label_x = chart_left
          + (idx * (chart_w - 1)) / (NWS_HOURLY_HOURS - 1);
      draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
                GRect(label_x - 14, 134, 28, 18),
                fitness_muted_text_color(), GTextAlignmentCenter);
    }
  }

  // Narrative block (short forecast + detailed prose) gets the bottom
  // half of the screen now that the hour legend moved up.
  draw_text(ctx, s_nws_p1_short, s_font_complication,
            GRect(8, 156, SCREEN_W - 16, 22),
            theme_fg_color(), GTextAlignmentLeft);
  draw_text(ctx, s_nws_p1_detailed,
            fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(8, 178, SCREEN_W - 16, 50),
            fitness_muted_text_color(), GTextAlignmentLeft);
}

// Render a single period block for the Narrative layout. Returns the y
// where the next block should start.
static int nws_draw_period_block(GContext *ctx, int y, const char *label,
                                  int temp_high, int temp_low_or_none,
                                  const char *detailed, int line_count) {
  char header[40];
  if (temp_low_or_none == INT16_MIN) {
    snprintf(header, sizeof(header), "%s  %d\xC2\xB0", label, temp_high);
  } else {
    snprintf(header, sizeof(header), "%s  %d\xC2\xB0/%d\xC2\xB0",
             label, temp_high, temp_low_or_none);
  }
  draw_text(ctx, header, s_font_complication,
            GRect(8, y, SCREEN_W - 16, 22),
            theme_fg_color(), GTextAlignmentLeft);
  draw_text(ctx, detailed,
            fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
            GRect(8, y + 22, SCREEN_W - 16, line_count * 18 + 2),
            fitness_muted_text_color(), GTextAlignmentLeft);
  int next_y = y + 22 + line_count * 18 + 6;
  // Horizontal rule
  graphics_context_set_stroke_color(ctx, fitness_muted_text_color());
  graphics_draw_line(ctx, GPoint(8, next_y - 2),
                     GPoint(SCREEN_W - 8, next_y - 2));
  return next_y;
}

// Layout B — Narrative Weather. Header + three stacked period blocks.
static void nws_forecast_narrative_draw(GContext *ctx) {
  nws_draw_header(ctx);
  int y = 26;
  // Block 1: current period — pair with the next period's low (assumed to
  // be Tonight when current is daytime) to render H/L when available.
  int p1_low = (s_nws_p2_temp != 0) ? (int)s_nws_p2_temp : INT16_MIN;
  y = nws_draw_period_block(ctx, y, s_nws_p1_label,
                            (int)s_nws_p1_temp, p1_low,
                            s_nws_p1_detailed, 3);
  if (s_nws_p2_label[0]) {
    y = nws_draw_period_block(ctx, y, s_nws_p2_label,
                              (int)s_nws_p2_temp, INT16_MIN,
                              s_nws_p2_detailed, 2);
  }
  if (s_nws_p3_label[0]) {
    int p3_low = (s_nws_p3_temp != 0 && s_nws_p3_temp < s_nws_p2_temp)
        ? (int)s_nws_p3_temp : INT16_MIN;
    nws_draw_period_block(ctx, y, s_nws_p3_label,
                          (int)s_nws_p3_temp, p3_low,
                          s_nws_p3_detailed, 2);
  }
}

static void nws_forecast_draw_overlay(GContext *ctx) {
  if (s_weather_provider != WeatherProviderNws) {
    nws_draw_header(ctx);
    draw_text(ctx, "Set provider to NWS", s_font_complication,
              GRect(8, 60, SCREEN_W - 16, 24),
              theme_fg_color(), GTextAlignmentCenter);
    draw_text(ctx, "in Pebble app settings", s_font_complication,
              GRect(8, 86, SCREEN_W - 16, 24),
              fitness_muted_text_color(), GTextAlignmentCenter);
    return;
  }

  if (s_nws_p1_label[0] == '\0') {
    nws_draw_header(ctx);
    draw_text(ctx, "Waiting for NWS data...", s_font_complication,
              GRect(8, 80, SCREEN_W - 16, 28),
              theme_fg_color(), GTextAlignmentCenter);
    return;
  }

  if (s_nws_forecast_style == NwsForecastStyleNarrative) {
    nws_forecast_narrative_draw(ctx);
  } else {
    nws_forecast_chart_heavy_draw(ctx);
  }
}

static void step_history_draw_overlay(GContext *ctx) {
  draw_text(ctx, "STEPS TODAY", s_font_top, GRect(8, 4, SCREEN_W - 16, 22),
            theme_fg_color(), GTextAlignmentLeft);

  HealthValue totals[7] = {0};
#if defined(PBL_HEALTH)
  // Walk back 7 daily buckets ending with today. health_service_sum_today
  // returns today's running total; previous days use explicit start/end.
  time_t today_start = time_start_of_today();
  totals[6] = health_service_sum_today(HealthMetricStepCount);
  for (int i = 0; i < 6; i++) {
    time_t end = today_start - (i * 86400);
    time_t start = end - 86400;
    int slot = 5 - i;
    totals[slot] = health_service_sum(HealthMetricStepCount, start, end);
  }
#endif

  // Today's count: prefer the live update_stats-maintained value, fall back to
  // the rightmost history slot if Health hasn't published a fresh "today" yet.
  int today_count = s_fitness_steps_value;
  if (today_count <= 0 && totals[6] > 0) {
    today_count = (int)totals[6];
  }
  totals[6] = today_count;

  char steps_buf[16];
  char goal_buf[16];
  char goal_line[28];
  format_number_commas(steps_buf, sizeof(steps_buf), today_count);
  format_number_commas(goal_buf, sizeof(goal_buf), s_fitness_target_steps);
  snprintf(goal_line, sizeof(goal_line), "/ %s", goal_buf);

  // s_font_roboto is FONT_KEY_ROBOTO_BOLD_SUBSET_49, which omits punctuation
  // (only carries the digits + colon used by the time face). The comma in
  // 1,234-style step counts shows as the missing-glyph box. Use Bitham 42,
  // which carries the full Latin set.
  draw_text(ctx, steps_buf, s_font_time,
            GRect(0, 28, SCREEN_W, 56),
            theme_fg_color(), GTextAlignmentCenter);
  draw_text(ctx, goal_line, s_font_complication,
            GRect(0, 82, SCREEN_W, 20),
            fitness_muted_text_color(), GTextAlignmentCenter);

  const int chart_left = 8;
  const int chart_right = SCREEN_W - 8;
  const int chart_w = chart_right - chart_left;
  const int chart_top = 108;
  const int chart_bottom = 195;
  const int chart_h = chart_bottom - chart_top;
  const int n_bars = 7;
  const int slot_w = chart_w / n_bars;
  const int bar_w = slot_w - 4;

  HealthValue max_steps = 1;
  for (int i = 0; i < n_bars; i++) {
    if (totals[i] > max_steps) {
      max_steps = totals[i];
    }
  }
  HealthValue target = s_fitness_target_steps > 0
      ? (HealthValue)s_fitness_target_steps : 10000;

  graphics_context_set_stroke_color(ctx, theme_fg_color());
  graphics_context_set_fill_color(ctx, theme_fg_color());
  graphics_context_set_stroke_width(ctx, 1);

  for (int i = 0; i < n_bars; i++) {
    HealthValue v = totals[i];
    int bar_x = chart_left + (i * slot_w) + (slot_w - bar_w) / 2;
    bool is_today = (i == n_bars - 1);
    bool hit_goal = v >= target;

    if (v <= 0) {
      // 1-pixel baseline marker for zero-step days.
      graphics_draw_line(ctx, GPoint(bar_x, chart_bottom),
                         GPoint(bar_x + bar_w - 1, chart_bottom));
      continue;
    }

    int bar_h = max_steps > 0
        ? (int)((v * chart_h) / max_steps) : 0;
    if (bar_h < 2) {
      bar_h = 2;
    }
    int bar_y = chart_bottom - bar_h;

    if (hit_goal) {
      graphics_fill_rect(ctx, GRect(bar_x, bar_y, bar_w, bar_h),
                         0, GCornerNone);
    } else {
      graphics_draw_rect(ctx, GRect(bar_x, bar_y, bar_w, bar_h));
    }

    if (is_today) {
      // Top-right inset notch always means "today". Carve out of the
      // existing fill / outline so goal-hit status is still visible.
      const int notch = 5;
      int notch_x = bar_x + bar_w - notch;
      int notch_y = bar_y;
      if (hit_goal) {
        graphics_context_set_fill_color(ctx, theme_bg_color());
        graphics_fill_rect(ctx, GRect(notch_x, notch_y, notch, notch),
                           0, GCornerNone);
        graphics_context_set_fill_color(ctx, theme_fg_color());
      } else {
        graphics_fill_rect(ctx, GRect(notch_x, notch_y, notch, notch),
                           0, GCornerNone);
      }
    }
  }

  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);
  int today_dow = tm_now ? tm_now->tm_wday : 0;
  static const char * const k_labels[7] = {"M", "T", "W", "T", "F", "S", "S"};
  for (int pos = 0; pos < n_bars; pos++) {
    int days_ago = (n_bars - 1) - pos;
    int dow = (today_dow - days_ago + 70) % 7;
    int label_idx = shake_dow_to_label_idx(dow);
    int x = chart_left + (pos * slot_w) + slot_w / 2;
    draw_text(ctx, k_labels[label_idx],
              fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
              GRect(x - 12, 200, 24, 18),
              theme_fg_color(), GTextAlignmentCenter);
    if (pos == n_bars - 1) {
      graphics_draw_line(ctx, GPoint(x - 5, 215), GPoint(x + 5, 215));
    }
  }
}

static void shake_overlay_update_proc(Layer *layer, GContext *ctx) {
  (void)layer;

  graphics_context_set_fill_color(ctx, theme_bg_color());
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  // Shake overlays always follow LIGHT_MODE only, never the main-face section
  // INVERT toggles. Reset the active section so bitmap theme lookups
  // (draw_bitmap_theme) pick the right variant.
  set_draw_section(ColorSectionBase);

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
    case ShakeBehaviorPrices:
      prices_draw_overlay(ctx);
      break;
    case ShakeBehaviorTideChart:
      tide_chart_draw_overlay(ctx);
      break;
    case ShakeBehaviorStepHistory:
      step_history_draw_overlay(ctx);
      break;
    case ShakeBehaviorNwsForecast:
      nws_forecast_draw_overlay(ctx);
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

  // Two-line layout has the summary on its own 184-px row; one-line layout
  // shares a 170-px row with the temperature, so it gets a shorter variant
  // pre-sized in pkjs against the per-char widths of FONT_KEY_GOTHIC_18_BOLD.
  // Fall back to the full summary if compact is empty (Open-Meteo path,
  // first boot before any pkjs delivery, or persist not yet populated).
  const char *summary;
  if (verbose_weather_layout_is_large()) {
    summary = s_weather_summary_buf[0] ? s_weather_summary_buf : "WEATHER";
  } else {
    summary = s_weather_summary_compact_buf[0]
                ? s_weather_summary_compact_buf
                : (s_weather_summary_buf[0] ? s_weather_summary_buf : "WEATHER");
  }

  if (verbose_weather_layout_is_large()) {
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

  const int content_offset_y =
      (s_verbose_weather_enabled && s_time_font != TIME_FONT_CASIO) ? VERBOSE_WEATHER_OFFSET_Y : 0;
  const int time_frame_y = TIME_FRAME_Y + content_offset_y;
  const int weather_y = weather_band_y();
  const GRect date_frame = GRect(0, DATE_FRAME_Y + content_offset_y, SCREEN_W, 29);
  const bool verbose_weather_meeting_color_break =
      s_verbose_weather_enabled &&
      section_backgrounds_differ(ColorSectionWeather, ColorSectionMeetingBar);
  const bool large_weather_meeting_color_break =
      verbose_weather_layout_is_large() && verbose_weather_meeting_color_break;
  const bool large_weather_layout = s_verbose_weather_enabled && verbose_weather_layout_is_large();
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
    draw_time_row_at(ctx, time_frame_y,
                     s_time_font == TIME_FONT_CASIO
                         ? weather_y
                         : TIME_VISUAL_BOTTOM + VERBOSE_WEATHER_OFFSET_Y);
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

  draw_text(ctx, event_display_text(s_event_buf), s_font_event,
            GRect(8, 203, 184, 25), draw_fg_color(), GTextAlignmentCenter);
}

static void update_time_date(struct tm *t) {
  if (s_military_time_enabled) {
    strftime(s_time_buf, sizeof(s_time_buf), "%H:%M", t);
    strftime(s_time_buf_casio, sizeof(s_time_buf_casio), "%H:%M", t);
    s_ampm_buf[0] = '\0';
  } else {
    strftime(s_time_buf, sizeof(s_time_buf), "%I:%M", t);
    strftime(s_time_buf_casio, sizeof(s_time_buf_casio), "%I:%M", t);
    strftime(s_ampm_buf, sizeof(s_ampm_buf), "%p", t);
  }
  // s_time_buf honors REMOVE_LEADING_ZERO; s_time_buf_casio always keeps
  // the leading zero so phantom segments align with "88:88" width.
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
  bool was_connected = s_phone_connected;
  s_phone_connected = connected;
  if (!connected) {
    if (was_connected && s_vibrate_on_disconnect) {
      vibes_double_pulse();
    }
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
  bool prices_changed = false;
  bool forecast_changed = false;

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

  t = dict_find(iter, MESSAGE_KEY_FORECAST_TEMP_F);
  if (t && t->length >= FORECAST_HOURS) {
    memcpy(s_forecast_temp, t->value->data, sizeof(s_forecast_temp));
    persist_write_data(PERSIST_KEY_FORECAST_TEMP_F,
                       s_forecast_temp, sizeof(s_forecast_temp));
    forecast_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FORECAST_PRECIP_PCT);
  if (t && t->length >= FORECAST_HOURS) {
    memcpy(s_forecast_precip, t->value->data, sizeof(s_forecast_precip));
    for (int i = 0; i < FORECAST_HOURS; i++) {
      if (s_forecast_precip[i] > 100) {
        s_forecast_precip[i] = 100;
      }
    }
    persist_write_data(PERSIST_KEY_FORECAST_PRECIP_PCT,
                       s_forecast_precip, sizeof(s_forecast_precip));
    forecast_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FORECAST_START_T);
  if (t) {
    s_forecast_start_t = (time_t)t->value->int32;
    persist_write_int(PERSIST_KEY_FORECAST_START_T, (int)s_forecast_start_t);
    forecast_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_FORECAST_LAST_UPDATE_T);
  if (t) {
    s_forecast_last_update_t = (time_t)t->value->int32;
    persist_write_int(PERSIST_KEY_FORECAST_LAST_UPDATE_T,
                      (int)s_forecast_last_update_t);
    forecast_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_WEATHER_PROVIDER);
  if (t) {
    s_weather_provider = (t->value->int32 == 1)
        ? WeatherProviderNws : WeatherProviderOpenMeteo;
    persist_write_int(PERSIST_KEY_WEATHER_PROVIDER, (int)s_weather_provider);
  }

  t = dict_find(iter, MESSAGE_KEY_NWS_HOURLY_TEMPS_F);
  if (t && t->type == TUPLE_BYTE_ARRAY) {
    uint16_t n = t->length;
    if (n > sizeof(s_nws_hourly_temps_f)) n = sizeof(s_nws_hourly_temps_f);
    memcpy(s_nws_hourly_temps_f, t->value->data, n);
    persist_write_data(PERSIST_KEY_NWS_HOURLY_TEMPS_F, s_nws_hourly_temps_f,
                       sizeof(s_nws_hourly_temps_f));
  }

  t = dict_find(iter, MESSAGE_KEY_NWS_HOURLY_PRECIP_PCT);
  if (t && t->type == TUPLE_BYTE_ARRAY) {
    uint16_t n = t->length;
    if (n > sizeof(s_nws_hourly_precip_pct)) n = sizeof(s_nws_hourly_precip_pct);
    memcpy(s_nws_hourly_precip_pct, t->value->data, n);
    persist_write_data(PERSIST_KEY_NWS_HOURLY_PRECIP_PCT, s_nws_hourly_precip_pct,
                       sizeof(s_nws_hourly_precip_pct));
  }

  t = dict_find(iter, MESSAGE_KEY_NWS_HOURLY_START_T);
  if (t) {
    s_nws_hourly_start_t = (uint32_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NWS_HOURLY_START_T,
                      (int)s_nws_hourly_start_t);
  }

  t = dict_find(iter, MESSAGE_KEY_NWS_P1_LABEL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p1_label, sizeof(s_nws_p1_label),
                         PERSIST_KEY_NWS_P1_LABEL, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P1_SHORT);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p1_short, sizeof(s_nws_p1_short),
                         PERSIST_KEY_NWS_P1_SHORT, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P1_DETAILED);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p1_detailed, sizeof(s_nws_p1_detailed),
                         PERSIST_KEY_NWS_P1_DETAILED, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P1_TEMP);
  if (t) {
    s_nws_p1_temp = (int8_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NWS_P1_TEMP, (int)s_nws_p1_temp);
  }

  t = dict_find(iter, MESSAGE_KEY_NWS_P2_LABEL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p2_label, sizeof(s_nws_p2_label),
                         PERSIST_KEY_NWS_P2_LABEL, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P2_SHORT);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p2_short, sizeof(s_nws_p2_short),
                         PERSIST_KEY_NWS_P2_SHORT, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P2_DETAILED);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p2_detailed, sizeof(s_nws_p2_detailed),
                         PERSIST_KEY_NWS_P2_DETAILED, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P2_TEMP);
  if (t) {
    s_nws_p2_temp = (int8_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NWS_P2_TEMP, (int)s_nws_p2_temp);
  }

  t = dict_find(iter, MESSAGE_KEY_NWS_P3_LABEL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p3_label, sizeof(s_nws_p3_label),
                         PERSIST_KEY_NWS_P3_LABEL, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P3_SHORT);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p3_short, sizeof(s_nws_p3_short),
                         PERSIST_KEY_NWS_P3_SHORT, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P3_DETAILED);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_p3_detailed, sizeof(s_nws_p3_detailed),
                         PERSIST_KEY_NWS_P3_DETAILED, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_P3_TEMP);
  if (t) {
    s_nws_p3_temp = (int8_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NWS_P3_TEMP, (int)s_nws_p3_temp);
  }

  t = dict_find(iter, MESSAGE_KEY_NWS_ALERT_TITLE);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_alert_title, sizeof(s_nws_alert_title),
                         PERSIST_KEY_NWS_ALERT_TITLE, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_LOCATION_LABEL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_nws_location_label, sizeof(s_nws_location_label),
                         PERSIST_KEY_NWS_LOCATION_LABEL, t->value->cstring);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_LAST_UPDATE_T);
  if (t) {
    s_nws_last_update_t = (uint32_t)t->value->int32;
    persist_write_int(PERSIST_KEY_NWS_LAST_UPDATE_T,
                      (int)s_nws_last_update_t);
  }
  t = dict_find(iter, MESSAGE_KEY_NWS_FORECAST_STYLE);
  if (t) {
    s_nws_forecast_style = (t->value->int32 == 1)
        ? NwsForecastStyleNarrative : NwsForecastStyleChartHeavy;
    persist_write_int(PERSIST_KEY_NWS_FORECAST_STYLE,
                      (int)s_nws_forecast_style);
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

  t = dict_find(iter, MESSAGE_KEY_TIME_FONT);
  if (t) {
    int v = (int)t->value->int32;
    if (v == TIME_FONT_CASIO) {
      s_time_font = TIME_FONT_CASIO;
    } else if (v == TIME_FONT_ROBOTO) {
      s_time_font = TIME_FONT_ROBOTO;
    } else if (v == TIME_FONT_LECO) {
      s_time_font = TIME_FONT_LECO;
    } else {
      s_time_font = TIME_FONT_DEFAULT;
    }
    persist_write_int(PERSIST_KEY_TIME_FONT, (int)s_time_font);
    mark_face_dirty();
  }

  t = dict_find(iter, MESSAGE_KEY_CASIO_PHANTOM);
  if (t) {
    s_casio_phantom = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_CASIO_PHANTOM, s_casio_phantom);
    mark_face_dirty();
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

  t = dict_find(iter, MESSAGE_KEY_VIBRATE_ON_DISCONNECT);
  if (t) {
    s_vibrate_on_disconnect = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_VIBRATE_ON_DISCONNECT, s_vibrate_on_disconnect);
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

  t = dict_find(iter, MESSAGE_KEY_WEATHER_SUMMARY_COMPACT);
  if (t && t->type == TUPLE_CSTRING) {
    strncpy(s_weather_summary_compact_buf, t->value->cstring,
            sizeof(s_weather_summary_compact_buf) - 1);
    s_weather_summary_compact_buf[sizeof(s_weather_summary_compact_buf) - 1] = '\0';
    persist_write_string(PERSIST_KEY_WEATHER_SUMMARY_COMPACT,
                         s_weather_summary_compact_buf);
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

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP);
  if (t) {
    s_day_event_half_hours_first_bitmap = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP,
                      s_day_event_half_hours_first_bitmap);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_TOMORROW);
  if (t) {
    s_day_event_half_hours_first_bitmap_tomorrow = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_TOMORROW,
                      s_day_event_half_hours_first_bitmap_tomorrow);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_YESTERDAY);
  if (t) {
    s_day_event_half_hours_first_bitmap_yesterday = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_YESTERDAY,
                      s_day_event_half_hours_first_bitmap_yesterday);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP);
  if (t) {
    s_day_event_half_hours_second_bitmap = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP,
                      s_day_event_half_hours_second_bitmap);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_TOMORROW);
  if (t) {
    s_day_event_half_hours_second_bitmap_tomorrow = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_TOMORROW,
                      s_day_event_half_hours_second_bitmap_tomorrow);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_YESTERDAY);
  if (t) {
    s_day_event_half_hours_second_bitmap_yesterday = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_YESTERDAY,
                      s_day_event_half_hours_second_bitmap_yesterday);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_YOUR_DAY_HALF_HOUR_PIPS);
  if (t) {
    s_your_day_half_hour_pips_enabled = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_YOUR_DAY_HALF_HOUR_PIPS,
                       s_your_day_half_hour_pips_enabled);
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

  t = dict_find(iter, MESSAGE_KEY_EMPTY_EVENT_LABEL);
  if (t && t->type == TUPLE_CSTRING) {
    const char *value = t->value->cstring;
    if (value && value[0]) {
      store_overlay_string(s_empty_event_label, sizeof(s_empty_event_label),
                           PERSIST_KEY_EMPTY_EVENT_LABEL, value);
    } else {
      strncpy(s_empty_event_label, "[None]", sizeof(s_empty_event_label) - 1);
      s_empty_event_label[sizeof(s_empty_event_label) - 1] = '\0';
      persist_write_string(PERSIST_KEY_EMPTY_EVENT_LABEL, s_empty_event_label);
    }
    mark_face_dirty();
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

  t = dict_find(iter, MESSAGE_KEY_TIDE_HOURLY_LEVELS);
  if (t && t->type == TUPLE_BYTE_ARRAY) {
    size_t n = t->length;
    if (n > sizeof(s_tide_hourly_levels)) {
      n = sizeof(s_tide_hourly_levels);
    }
    memcpy(s_tide_hourly_levels, t->value->data, n);
    for (size_t i = n; i < sizeof(s_tide_hourly_levels); i++) {
      s_tide_hourly_levels[i] = 0xFF;
    }
    persist_write_data(PERSIST_KEY_TIDE_HOURLY_LEVELS, s_tide_hourly_levels,
                       sizeof(s_tide_hourly_levels));
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_TIDE_NEXT_HIGH_T);
  if (t) {
    s_tide_next_high_t = (uint32_t)t->value->int32;
    persist_write_int(PERSIST_KEY_TIDE_NEXT_HIGH_T, (int)s_tide_next_high_t);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_TIDE_NEXT_HIGH_LEVEL);
  if (t) {
    s_tide_next_high_level = (uint8_t)t->value->int32;
    persist_write_int(PERSIST_KEY_TIDE_NEXT_HIGH_LEVEL, (int)s_tide_next_high_level);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_TIDE_NEXT_LOW_T);
  if (t) {
    s_tide_next_low_t = (uint32_t)t->value->int32;
    persist_write_int(PERSIST_KEY_TIDE_NEXT_LOW_T, (int)s_tide_next_low_t);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_TIDE_NEXT_LOW_LEVEL);
  if (t) {
    s_tide_next_low_level = (uint8_t)t->value->int32;
    persist_write_int(PERSIST_KEY_TIDE_NEXT_LOW_LEVEL, (int)s_tide_next_low_level);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_TIDE_STATION_NAME);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_tide_station_name, sizeof(s_tide_station_name),
                         PERSIST_KEY_TIDE_STATION_NAME, t->value->cstring);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_TIDE_UNITS);
  if (t) {
    s_tide_units_meters = t->value->int32 == 1;
    persist_write_bool(PERSIST_KEY_TIDE_UNITS, s_tide_units_meters);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_TIDE_STATION_ID);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_tide_station_id, sizeof(s_tide_station_id),
                         PERSIST_KEY_TIDE_STATION_ID, t->value->cstring);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_STOCK_1_SYMBOL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_prices_stock_1_symbol, sizeof(s_prices_stock_1_symbol),
                         PERSIST_KEY_PRICES_STOCK_1_SYMBOL, t->value->cstring);
    fitness_settings_changed = true;
    prices_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_STOCK_2_SYMBOL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_prices_stock_2_symbol, sizeof(s_prices_stock_2_symbol),
                         PERSIST_KEY_PRICES_STOCK_2_SYMBOL, t->value->cstring);
    fitness_settings_changed = true;
    prices_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_CRYPTO_SYMBOL);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_prices_crypto_symbol, sizeof(s_prices_crypto_symbol),
                         PERSIST_KEY_PRICES_CRYPTO_SYMBOL, t->value->cstring);
    fitness_settings_changed = true;
    prices_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_SHOW_STOCK_1);
  if (t) {
    s_prices_show_stock_1 = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_PRICES_SHOW_STOCK_1, s_prices_show_stock_1);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_SHOW_STOCK_2);
  if (t) {
    s_prices_show_stock_2 = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_PRICES_SHOW_STOCK_2, s_prices_show_stock_2);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_SHOW_CRYPTO);
  if (t) {
    s_prices_show_crypto = t->value->int32 != 0;
    persist_write_bool(PERSIST_KEY_PRICES_SHOW_CRYPTO, s_prices_show_crypto);
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_CADENCE_MIN);
  if (t) {
    s_prices_cadence_min = prices_sanitize_cadence_min((int)t->value->int32);
    persist_write_int(PERSIST_KEY_PRICES_CADENCE_MIN, s_prices_cadence_min);
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_POSITIVE_COLOR_LIGHT);
  if (t) {
    s_prices_positive_color_light_hex = sanitize_packed_color((int)t->value->int32);
    persist_write_int(PERSIST_KEY_PRICES_POSITIVE_COLOR_LIGHT,
                      s_prices_positive_color_light_hex);
    prices_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_POSITIVE_COLOR_DARK);
  if (t) {
    s_prices_positive_color_dark_hex = sanitize_packed_color((int)t->value->int32);
    persist_write_int(PERSIST_KEY_PRICES_POSITIVE_COLOR_DARK,
                      s_prices_positive_color_dark_hex);
    prices_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_NEGATIVE_COLOR_LIGHT);
  if (t) {
    s_prices_negative_color_light_hex = sanitize_packed_color((int)t->value->int32);
    persist_write_int(PERSIST_KEY_PRICES_NEGATIVE_COLOR_LIGHT,
                      s_prices_negative_color_light_hex);
    prices_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_NEGATIVE_COLOR_DARK);
  if (t) {
    s_prices_negative_color_dark_hex = sanitize_packed_color((int)t->value->int32);
    persist_write_int(PERSIST_KEY_PRICES_NEGATIVE_COLOR_DARK,
                      s_prices_negative_color_dark_hex);
    prices_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_STOCK_1_PRICE);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_prices_stock_1_price, sizeof(s_prices_stock_1_price),
                         PERSIST_KEY_PRICES_STOCK_1_PRICE, t->value->cstring);
    prices_changed = true;
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_STOCK_2_PRICE);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_prices_stock_2_price, sizeof(s_prices_stock_2_price),
                         PERSIST_KEY_PRICES_STOCK_2_PRICE, t->value->cstring);
    prices_changed = true;
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_CRYPTO_PRICE);
  if (t && t->type == TUPLE_CSTRING) {
    store_overlay_string(s_prices_crypto_price, sizeof(s_prices_crypto_price),
                         PERSIST_KEY_PRICES_CRYPTO_PRICE, t->value->cstring);
    prices_changed = true;
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_STOCK_1_DELTA_X100);
  if (t) {
    s_prices_stock_1_delta_x100 = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_PRICES_STOCK_1_DELTA_X100,
                      s_prices_stock_1_delta_x100);
    prices_changed = true;
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_STOCK_2_DELTA_X100);
  if (t) {
    s_prices_stock_2_delta_x100 = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_PRICES_STOCK_2_DELTA_X100,
                      s_prices_stock_2_delta_x100);
    prices_changed = true;
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_CRYPTO_DELTA_X100);
  if (t) {
    s_prices_crypto_delta_x100 = (int)t->value->int32;
    persist_write_int(PERSIST_KEY_PRICES_CRYPTO_DELTA_X100,
                      s_prices_crypto_delta_x100);
    prices_changed = true;
    fitness_settings_changed = true;
  }

  t = dict_find(iter, MESSAGE_KEY_PRICES_LAST_UPDATE_T);
  if (t) {
    s_prices_last_update_t = (time_t)t->value->int32;
    persist_write_int(PERSIST_KEY_PRICES_LAST_UPDATE_T, (int)s_prices_last_update_t);
    prices_changed = true;
    fitness_settings_changed = true;
  }

  if (prices_changed) {
    mark_face_dirty();
  }

  if (forecast_changed) {
    s_forecast_data_loaded = s_forecast_start_t > 0 && s_forecast_last_update_t > 0;
    mark_face_dirty();
  }

  if ((fitness_settings_changed || prices_changed || forecast_changed) &&
      s_shake_overlay_visible && s_shake_overlay_layer) {
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
  bool forecast_temp_loaded = false;
  bool forecast_precip_loaded = false;
  if (persist_exists(PERSIST_KEY_FORECAST_TEMP_F)) {
    int bytes = persist_read_data(PERSIST_KEY_FORECAST_TEMP_F,
                                  s_forecast_temp, sizeof(s_forecast_temp));
    forecast_temp_loaded = bytes == (int)sizeof(s_forecast_temp);
  }
  if (persist_exists(PERSIST_KEY_FORECAST_PRECIP_PCT)) {
    int bytes = persist_read_data(PERSIST_KEY_FORECAST_PRECIP_PCT,
                                  s_forecast_precip, sizeof(s_forecast_precip));
    forecast_precip_loaded = bytes == (int)sizeof(s_forecast_precip);
  }
  if (persist_exists(PERSIST_KEY_FORECAST_START_T)) {
    s_forecast_start_t = (time_t)persist_read_int(PERSIST_KEY_FORECAST_START_T);
  }
  if (persist_exists(PERSIST_KEY_FORECAST_LAST_UPDATE_T)) {
    s_forecast_last_update_t =
        (time_t)persist_read_int(PERSIST_KEY_FORECAST_LAST_UPDATE_T);
  }
  s_forecast_data_loaded =
      forecast_temp_loaded && forecast_precip_loaded &&
      s_forecast_start_t > 0 && s_forecast_last_update_t > 0;
  if (persist_exists(PERSIST_KEY_TOP_STEPS)) {
    s_w800_steps_top_enabled = persist_read_bool(PERSIST_KEY_TOP_STEPS);
  }
  if (persist_exists(PERSIST_KEY_VIBRATE_ON_DISCONNECT)) {
    s_vibrate_on_disconnect = persist_read_bool(PERSIST_KEY_VIBRATE_ON_DISCONNECT);
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
  if (persist_exists(PERSIST_KEY_TIME_FONT)) {
    int v = persist_read_int(PERSIST_KEY_TIME_FONT);
    if (v == TIME_FONT_CASIO) {
      s_time_font = TIME_FONT_CASIO;
    } else if (v == TIME_FONT_ROBOTO) {
      s_time_font = TIME_FONT_ROBOTO;
    } else if (v == TIME_FONT_LECO) {
      s_time_font = TIME_FONT_LECO;
    } else {
      s_time_font = TIME_FONT_DEFAULT;
    }
  }
  if (persist_exists(PERSIST_KEY_CASIO_PHANTOM)) {
    s_casio_phantom = persist_read_bool(PERSIST_KEY_CASIO_PHANTOM);
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
  if (persist_exists(PERSIST_KEY_WEATHER_SUMMARY_COMPACT)) {
    persist_read_string(PERSIST_KEY_WEATHER_SUMMARY_COMPACT,
                        s_weather_summary_compact_buf,
                        sizeof(s_weather_summary_compact_buf));
    s_weather_summary_compact_buf[sizeof(s_weather_summary_compact_buf) - 1] = '\0';
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
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP)) {
    s_day_event_half_hours_first_bitmap =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_TOMORROW)) {
    s_day_event_half_hours_first_bitmap_tomorrow =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_TOMORROW);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_YESTERDAY)) {
    s_day_event_half_hours_first_bitmap_yesterday =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_FIRST_BITMAP_YESTERDAY);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP)) {
    s_day_event_half_hours_second_bitmap =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_TOMORROW)) {
    s_day_event_half_hours_second_bitmap_tomorrow =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_TOMORROW);
  }
  if (persist_exists(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_YESTERDAY)) {
    s_day_event_half_hours_second_bitmap_yesterday =
        persist_read_int(PERSIST_KEY_DAY_EVENT_HALF_HOURS_SECOND_BITMAP_YESTERDAY);
  }
  if (persist_exists(PERSIST_KEY_YOUR_DAY_HALF_HOUR_PIPS)) {
    s_your_day_half_hour_pips_enabled =
        persist_read_bool(PERSIST_KEY_YOUR_DAY_HALF_HOUR_PIPS);
  }
  if (persist_exists(PERSIST_KEY_EMPTY_EVENT_LABEL)) {
    persist_read_string(PERSIST_KEY_EMPTY_EVENT_LABEL, s_empty_event_label,
                        sizeof(s_empty_event_label));
    s_empty_event_label[sizeof(s_empty_event_label) - 1] = '\0';
    if (s_empty_event_label[0] == '\0') {
      strncpy(s_empty_event_label, "[None]", sizeof(s_empty_event_label) - 1);
      s_empty_event_label[sizeof(s_empty_event_label) - 1] = '\0';
    }
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

  // One-shot tide-data schema migration: if the persisted version doesn't
  // match TIDE_DATA_VERSION, wipe the byte array + next-high / next-low so
  // the chart shows the Waiting placeholder until the new fetch lands. This
  // is what prevents 0.72-format leftovers from rendering as a misleading
  // partial chart after upgrading to 0.73.
  int tide_data_version = persist_exists(PERSIST_KEY_TIDE_DATA_VERSION)
      ? persist_read_int(PERSIST_KEY_TIDE_DATA_VERSION) : 0;
  if (tide_data_version != TIDE_DATA_VERSION) {
    persist_delete(PERSIST_KEY_TIDE_HOURLY_LEVELS);
    persist_delete(PERSIST_KEY_TIDE_NEXT_HIGH_T);
    persist_delete(PERSIST_KEY_TIDE_NEXT_HIGH_LEVEL);
    persist_delete(PERSIST_KEY_TIDE_NEXT_LOW_T);
    persist_delete(PERSIST_KEY_TIDE_NEXT_LOW_LEVEL);
    persist_write_int(PERSIST_KEY_TIDE_DATA_VERSION, TIDE_DATA_VERSION);
  }

  if (persist_exists(PERSIST_KEY_TIDE_HOURLY_LEVELS)) {
    persist_read_data(PERSIST_KEY_TIDE_HOURLY_LEVELS, s_tide_hourly_levels,
                      sizeof(s_tide_hourly_levels));
  }
  if (persist_exists(PERSIST_KEY_TIDE_NEXT_HIGH_T)) {
    s_tide_next_high_t = (uint32_t)persist_read_int(PERSIST_KEY_TIDE_NEXT_HIGH_T);
  }
  if (persist_exists(PERSIST_KEY_TIDE_NEXT_HIGH_LEVEL)) {
    s_tide_next_high_level =
        (uint8_t)persist_read_int(PERSIST_KEY_TIDE_NEXT_HIGH_LEVEL);
  }
  if (persist_exists(PERSIST_KEY_TIDE_NEXT_LOW_T)) {
    s_tide_next_low_t = (uint32_t)persist_read_int(PERSIST_KEY_TIDE_NEXT_LOW_T);
  }
  if (persist_exists(PERSIST_KEY_TIDE_NEXT_LOW_LEVEL)) {
    s_tide_next_low_level =
        (uint8_t)persist_read_int(PERSIST_KEY_TIDE_NEXT_LOW_LEVEL);
  }
  if (persist_exists(PERSIST_KEY_TIDE_STATION_NAME)) {
    persist_read_string(PERSIST_KEY_TIDE_STATION_NAME, s_tide_station_name,
                        sizeof(s_tide_station_name));
    s_tide_station_name[sizeof(s_tide_station_name) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_TIDE_UNITS)) {
    s_tide_units_meters = persist_read_bool(PERSIST_KEY_TIDE_UNITS);
  }
  if (persist_exists(PERSIST_KEY_TIDE_STATION_ID)) {
    persist_read_string(PERSIST_KEY_TIDE_STATION_ID, s_tide_station_id,
                        sizeof(s_tide_station_id));
    s_tide_station_id[sizeof(s_tide_station_id) - 1] = '\0';
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
  if (persist_exists(PERSIST_KEY_PRICES_STOCK_1_SYMBOL)) {
    persist_read_string(PERSIST_KEY_PRICES_STOCK_1_SYMBOL, s_prices_stock_1_symbol,
                        sizeof(s_prices_stock_1_symbol));
    s_prices_stock_1_symbol[sizeof(s_prices_stock_1_symbol) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_PRICES_STOCK_2_SYMBOL)) {
    persist_read_string(PERSIST_KEY_PRICES_STOCK_2_SYMBOL, s_prices_stock_2_symbol,
                        sizeof(s_prices_stock_2_symbol));
    s_prices_stock_2_symbol[sizeof(s_prices_stock_2_symbol) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_PRICES_CRYPTO_SYMBOL)) {
    persist_read_string(PERSIST_KEY_PRICES_CRYPTO_SYMBOL, s_prices_crypto_symbol,
                        sizeof(s_prices_crypto_symbol));
    s_prices_crypto_symbol[sizeof(s_prices_crypto_symbol) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_PRICES_CADENCE_MIN)) {
    s_prices_cadence_min =
        prices_sanitize_cadence_min(persist_read_int(PERSIST_KEY_PRICES_CADENCE_MIN));
  }
  if (persist_exists(PERSIST_KEY_PRICES_POSITIVE_COLOR_LIGHT)) {
    s_prices_positive_color_light_hex =
        sanitize_packed_color(persist_read_int(PERSIST_KEY_PRICES_POSITIVE_COLOR_LIGHT));
  }
  if (persist_exists(PERSIST_KEY_PRICES_POSITIVE_COLOR_DARK)) {
    s_prices_positive_color_dark_hex =
        sanitize_packed_color(persist_read_int(PERSIST_KEY_PRICES_POSITIVE_COLOR_DARK));
  }
  if (persist_exists(PERSIST_KEY_PRICES_NEGATIVE_COLOR_LIGHT)) {
    s_prices_negative_color_light_hex =
        sanitize_packed_color(persist_read_int(PERSIST_KEY_PRICES_NEGATIVE_COLOR_LIGHT));
  }
  if (persist_exists(PERSIST_KEY_PRICES_NEGATIVE_COLOR_DARK)) {
    s_prices_negative_color_dark_hex =
        sanitize_packed_color(persist_read_int(PERSIST_KEY_PRICES_NEGATIVE_COLOR_DARK));
  }
  if (persist_exists(PERSIST_KEY_PRICES_STOCK_1_PRICE)) {
    persist_read_string(PERSIST_KEY_PRICES_STOCK_1_PRICE, s_prices_stock_1_price,
                        sizeof(s_prices_stock_1_price));
    s_prices_stock_1_price[sizeof(s_prices_stock_1_price) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_PRICES_STOCK_2_PRICE)) {
    persist_read_string(PERSIST_KEY_PRICES_STOCK_2_PRICE, s_prices_stock_2_price,
                        sizeof(s_prices_stock_2_price));
    s_prices_stock_2_price[sizeof(s_prices_stock_2_price) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_PRICES_CRYPTO_PRICE)) {
    persist_read_string(PERSIST_KEY_PRICES_CRYPTO_PRICE, s_prices_crypto_price,
                        sizeof(s_prices_crypto_price));
    s_prices_crypto_price[sizeof(s_prices_crypto_price) - 1] = '\0';
  }
  if (persist_exists(PERSIST_KEY_PRICES_STOCK_1_DELTA_X100)) {
    s_prices_stock_1_delta_x100 =
        persist_read_int(PERSIST_KEY_PRICES_STOCK_1_DELTA_X100);
  }
  if (persist_exists(PERSIST_KEY_PRICES_STOCK_2_DELTA_X100)) {
    s_prices_stock_2_delta_x100 =
        persist_read_int(PERSIST_KEY_PRICES_STOCK_2_DELTA_X100);
  }
  if (persist_exists(PERSIST_KEY_PRICES_CRYPTO_DELTA_X100)) {
    s_prices_crypto_delta_x100 =
        persist_read_int(PERSIST_KEY_PRICES_CRYPTO_DELTA_X100);
  }
  if (persist_exists(PERSIST_KEY_PRICES_LAST_UPDATE_T)) {
    s_prices_last_update_t =
        (time_t)persist_read_int(PERSIST_KEY_PRICES_LAST_UPDATE_T);
  }
  if (persist_exists(PERSIST_KEY_PRICES_SHOW_STOCK_1)) {
    s_prices_show_stock_1 = persist_read_bool(PERSIST_KEY_PRICES_SHOW_STOCK_1);
  }
  if (persist_exists(PERSIST_KEY_PRICES_SHOW_STOCK_2)) {
    s_prices_show_stock_2 = persist_read_bool(PERSIST_KEY_PRICES_SHOW_STOCK_2);
  }
  if (persist_exists(PERSIST_KEY_PRICES_SHOW_CRYPTO)) {
    s_prices_show_crypto = persist_read_bool(PERSIST_KEY_PRICES_SHOW_CRYPTO);
  }

  if (persist_exists(PERSIST_KEY_WEATHER_PROVIDER)) {
    s_weather_provider =
        (persist_read_int(PERSIST_KEY_WEATHER_PROVIDER) == 1)
            ? WeatherProviderNws : WeatherProviderOpenMeteo;
  }
  memset(s_nws_hourly_temps_f, 0, sizeof(s_nws_hourly_temps_f));
  memset(s_nws_hourly_precip_pct, 0, sizeof(s_nws_hourly_precip_pct));
  if (persist_exists(PERSIST_KEY_NWS_HOURLY_TEMPS_F)) {
    persist_read_data(PERSIST_KEY_NWS_HOURLY_TEMPS_F, s_nws_hourly_temps_f,
                      sizeof(s_nws_hourly_temps_f));
  }
  if (persist_exists(PERSIST_KEY_NWS_HOURLY_PRECIP_PCT)) {
    persist_read_data(PERSIST_KEY_NWS_HOURLY_PRECIP_PCT,
                      s_nws_hourly_precip_pct,
                      sizeof(s_nws_hourly_precip_pct));
  }
  if (persist_exists(PERSIST_KEY_NWS_HOURLY_START_T)) {
    s_nws_hourly_start_t =
        (uint32_t)persist_read_int(PERSIST_KEY_NWS_HOURLY_START_T);
  }
  if (persist_exists(PERSIST_KEY_NWS_P1_LABEL)) {
    persist_read_string(PERSIST_KEY_NWS_P1_LABEL, s_nws_p1_label,
                        sizeof(s_nws_p1_label));
  }
  if (persist_exists(PERSIST_KEY_NWS_P1_SHORT)) {
    persist_read_string(PERSIST_KEY_NWS_P1_SHORT, s_nws_p1_short,
                        sizeof(s_nws_p1_short));
  }
  if (persist_exists(PERSIST_KEY_NWS_P1_DETAILED)) {
    persist_read_string(PERSIST_KEY_NWS_P1_DETAILED, s_nws_p1_detailed,
                        sizeof(s_nws_p1_detailed));
  }
  if (persist_exists(PERSIST_KEY_NWS_P1_TEMP)) {
    s_nws_p1_temp = (int8_t)persist_read_int(PERSIST_KEY_NWS_P1_TEMP);
  }
  if (persist_exists(PERSIST_KEY_NWS_P2_LABEL)) {
    persist_read_string(PERSIST_KEY_NWS_P2_LABEL, s_nws_p2_label,
                        sizeof(s_nws_p2_label));
  }
  if (persist_exists(PERSIST_KEY_NWS_P2_SHORT)) {
    persist_read_string(PERSIST_KEY_NWS_P2_SHORT, s_nws_p2_short,
                        sizeof(s_nws_p2_short));
  }
  if (persist_exists(PERSIST_KEY_NWS_P2_DETAILED)) {
    persist_read_string(PERSIST_KEY_NWS_P2_DETAILED, s_nws_p2_detailed,
                        sizeof(s_nws_p2_detailed));
  }
  if (persist_exists(PERSIST_KEY_NWS_P2_TEMP)) {
    s_nws_p2_temp = (int8_t)persist_read_int(PERSIST_KEY_NWS_P2_TEMP);
  }
  if (persist_exists(PERSIST_KEY_NWS_P3_LABEL)) {
    persist_read_string(PERSIST_KEY_NWS_P3_LABEL, s_nws_p3_label,
                        sizeof(s_nws_p3_label));
  }
  if (persist_exists(PERSIST_KEY_NWS_P3_SHORT)) {
    persist_read_string(PERSIST_KEY_NWS_P3_SHORT, s_nws_p3_short,
                        sizeof(s_nws_p3_short));
  }
  if (persist_exists(PERSIST_KEY_NWS_P3_DETAILED)) {
    persist_read_string(PERSIST_KEY_NWS_P3_DETAILED, s_nws_p3_detailed,
                        sizeof(s_nws_p3_detailed));
  }
  if (persist_exists(PERSIST_KEY_NWS_P3_TEMP)) {
    s_nws_p3_temp = (int8_t)persist_read_int(PERSIST_KEY_NWS_P3_TEMP);
  }
  if (persist_exists(PERSIST_KEY_NWS_ALERT_TITLE)) {
    persist_read_string(PERSIST_KEY_NWS_ALERT_TITLE, s_nws_alert_title,
                        sizeof(s_nws_alert_title));
  }
  if (persist_exists(PERSIST_KEY_NWS_LOCATION_LABEL)) {
    persist_read_string(PERSIST_KEY_NWS_LOCATION_LABEL,
                        s_nws_location_label,
                        sizeof(s_nws_location_label));
  }
  if (persist_exists(PERSIST_KEY_NWS_LAST_UPDATE_T)) {
    s_nws_last_update_t =
        (uint32_t)persist_read_int(PERSIST_KEY_NWS_LAST_UPDATE_T);
  }
  if (persist_exists(PERSIST_KEY_NWS_FORECAST_STYLE)) {
    s_nws_forecast_style =
        (persist_read_int(PERSIST_KEY_NWS_FORECAST_STYLE) == 1)
            ? NwsForecastStyleNarrative : NwsForecastStyleChartHeavy;
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
  s_icon_bitcoin_bubble =
      gbitmap_create_with_resource(RESOURCE_ID_ICON_BITCOIN_BUBBLE);
  s_icon_stocks_bubble =
      gbitmap_create_with_resource(RESOURCE_ID_ICON_STOCKS_BUBBLE);
}

static void destroy_theme_bitmaps(void) {
  destroy_weather_icon_bitmaps();
  destroy_bitmap(&s_icon_bitcoin_bubble);
  destroy_bitmap(&s_icon_stocks_bubble);
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
  s_font_roboto = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
  // Pebble ships LECO at fixed point sizes: 20, 26 (with AM/PM), 28 light,
  // 32 (this), 36, 42 (digits only), 60 (with AM/PM). 32 is the largest
  // bold-numbers variant that still includes the colon glyph; 60 is huge
  // and would not fit the time slot.
  s_font_leco = fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS);
  s_font_complication = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_event = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_casio_55 =
      fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WV58A_DIGITS_55));
  s_font_casio_70 =
      fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WV58A_DIGITS_70));
  s_font_casio_90 =
      fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WV58A_DIGITS_90));
  s_wv58a_am_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WV58A_AM);
  s_wv58a_pm_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WV58A_PM);
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
  destroy_bitmap(&s_wv58a_am_bitmap);
  destroy_bitmap(&s_wv58a_pm_bitmap);
  if (s_font_casio_55) {
    fonts_unload_custom_font(s_font_casio_55);
    s_font_casio_55 = NULL;
  }
  if (s_font_casio_70) {
    fonts_unload_custom_font(s_font_casio_70);
    s_font_casio_70 = NULL;
  }
  if (s_font_casio_90) {
    fonts_unload_custom_font(s_font_casio_90);
    s_font_casio_90 = NULL;
  }
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
  app_message_open(1024, 64);
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
