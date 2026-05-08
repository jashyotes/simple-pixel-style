package com.joshyates.jytime

import java.util.UUID

object Constants {
    val WATCHFACE_UUID: UUID = UUID.fromString("02eeda8d-2c95-4b0c-8e10-d5df05cce87a")

    // Auto-assigned by Pebble SDK from package.json messageKeys array
    const val KEY_NEXT_EVENT: UInt = 10000u
    const val KEY_PHONE_BATTERY: UInt = 10001u
    const val KEY_WEATHER_CODE: UInt = 10002u
    const val KEY_TEMPERATURE: UInt = 10003u
    const val KEY_RAIN_CHANCE: UInt = 10004u
    const val KEY_TOP_STEPS: UInt = 10005u
    const val KEY_FEELS_LIKE: UInt = 10006u
    const val KEY_HIGH_TEMP: UInt = 10007u
    const val KEY_WIND_SPEED: UInt = 10008u
    const val KEY_UV_INDEX: UInt = 10009u
    const val KEY_NEXT_EVENT_DELTA: UInt = 10010u
    const val KEY_COMPLICATION_1: UInt = 10011u
    const val KEY_COMPLICATION_2: UInt = 10012u
    const val KEY_COMPLICATION_3: UInt = 10013u

    const val SYNC_INTERVAL_MS: Long = 60_000L
    const val WEATHER_CACHE_MS: Long = 30 * 60_000L

    const val MAX_EVENT_DISPLAY_BYTES: Int = 72
    const val EVENT_LOOKAHEAD_HOURS: Long = 24

    const val NOTIFICATION_CHANNEL_ID = "jytime_sync"
    const val NOTIFICATION_ID = 1001

    const val PREFS_NAME = "jytime"
    const val PREF_ENABLED = "enabled"
    const val PREF_LAST_SYNC_MS = "last_sync_ms"
    const val PREF_LAST_EVENT = "last_event"
    const val PREF_LAST_BATTERY = "last_battery"
    const val PREF_LAST_WMO = "last_wmo"
    const val PREF_LAST_TEMP = "last_temp"
    const val PREF_LAST_RAIN = "last_rain"
    const val PREF_TOP_STEPS = "top_steps"
}
