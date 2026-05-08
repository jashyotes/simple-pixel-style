package com.joshyates.jytime

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.Context
import android.content.Intent
import android.content.pm.ServiceInfo
import android.os.Build
import android.os.IBinder
import android.util.Log
import io.rebble.pebblekit2.client.DefaultPebbleSender
import io.rebble.pebblekit2.common.model.PebbleDictionaryItem
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Job
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.nio.charset.StandardCharsets

/**
 * Foreground service that loops every SYNC_INTERVAL_MS, gathers calendar + battery + weather,
 * and pushes them to the JY Time watchface via PebbleSender.
 */
class WatchSyncService : Service() {

    companion object {
        private const val TAG = "JYSync"
        private const val ACTION_SYNC_NOW = "com.joshyates.jytime.SYNC_NOW"

        fun start(context: Context) {
            val intent = Intent(context, WatchSyncService::class.java)
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                context.startForegroundService(intent)
            } else {
                context.startService(intent)
            }
        }

        fun stop(context: Context) {
            context.stopService(Intent(context, WatchSyncService::class.java))
        }

        fun syncNow(context: Context) {
            val intent = Intent(context, WatchSyncService::class.java).setAction(ACTION_SYNC_NOW)
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                context.startForegroundService(intent)
            } else {
                context.startService(intent)
            }
        }
    }

    private val scope = CoroutineScope(SupervisorJob())
    private var loopJob: Job? = null
    private lateinit var sender: DefaultPebbleSender
    private lateinit var weather: WeatherFetcher

    @Volatile var lastSyncMs: Long = 0L
    @Volatile var lastEventTitle: String? = null
    @Volatile var lastBatteryPct: Int? = null
    @Volatile var lastWmoCode: Int? = null
    @Volatile var lastTempF: Int? = null
    @Volatile var lastRainChance: Int? = null

    override fun onBind(intent: Intent?): IBinder? = null

    override fun onCreate() {
        super.onCreate()
        sender = DefaultPebbleSender(this)
        weather = WeatherFetcher(this)
        startForegroundNotification()
        loopJob = scope.launch { runLoop() }
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if (intent?.action == ACTION_SYNC_NOW) {
            scope.launch {
                try {
                    syncOnce()
                } catch (t: Throwable) {
                    Log.w(TAG, "syncNow failed", t)
                }
            }
        }
        return START_STICKY
    }

    override fun onDestroy() {
        loopJob?.cancel()
        scope.cancel()
        try { sender.close() } catch (_: Throwable) {}
        super.onDestroy()
    }

    private suspend fun runLoop() {
        while (true) {
            try {
                syncOnce()
            } catch (t: Throwable) {
                Log.w(TAG, "syncOnce failed", t)
            }
            delay(Constants.SYNC_INTERVAL_MS)
        }
    }

    private suspend fun syncOnce() {
        val ev = CalendarReader.getNextEvent(this)
        val battery = BatteryReader.getBatteryPercent(this)
        val w = weather.fetch()
        val eventDisplay = ev?.displayString()?.truncateUtf8(Constants.MAX_EVENT_DISPLAY_BYTES) ?: ""
        val topSteps = getSharedPreferences(Constants.PREFS_NAME, Context.MODE_PRIVATE)
            .getBoolean(Constants.PREF_TOP_STEPS, false)

        val data = buildMap<UInt, PebbleDictionaryItem> {
            put(Constants.KEY_NEXT_EVENT, PebbleDictionaryItem.Text(eventDisplay))
            put(Constants.KEY_TOP_STEPS, PebbleDictionaryItem.UInt8(if (topSteps) 1u else 0u))
            if (battery != null) put(Constants.KEY_PHONE_BATTERY, PebbleDictionaryItem.UInt8(battery.toUByte()))
            if (w != null) {
                put(Constants.KEY_WEATHER_CODE, PebbleDictionaryItem.UInt8(w.wmoCode.coerceIn(0, 255).toUByte()))
                put(Constants.KEY_TEMPERATURE, PebbleDictionaryItem.Int8(w.temperatureF.coerceIn(-128, 127).toByte()))
                w.rainChance?.let {
                    put(Constants.KEY_RAIN_CHANCE, PebbleDictionaryItem.UInt8(it.coerceIn(0, 100).toUByte()))
                }
            }
        }

        val result = sender.sendDataToPebble(Constants.WATCHFACE_UUID, data)
        Log.d(TAG, "sync sent: event=$eventDisplay, batt=$battery, wmo=${w?.wmoCode}, tempF=${w?.temperatureF}, rain=${w?.rainChance}, result=$result")

        lastSyncMs = System.currentTimeMillis()
        lastEventTitle = eventDisplay.ifBlank { null }
        lastBatteryPct = battery
        lastWmoCode = w?.wmoCode
        lastTempF = w?.temperatureF
        lastRainChance = w?.rainChance
        saveLastSync()
    }

    private fun saveLastSync() {
        getSharedPreferences(Constants.PREFS_NAME, Context.MODE_PRIVATE).edit()
            .putLong(Constants.PREF_LAST_SYNC_MS, lastSyncMs)
            .putString(Constants.PREF_LAST_EVENT, lastEventTitle)
            .putInt(Constants.PREF_LAST_BATTERY, lastBatteryPct ?: -1)
            .putInt(Constants.PREF_LAST_WMO, lastWmoCode ?: -1)
            .putInt(Constants.PREF_LAST_TEMP, lastTempF ?: Int.MIN_VALUE)
            .putInt(Constants.PREF_LAST_RAIN, lastRainChance ?: -1)
            .apply()
    }

    private fun String.truncateUtf8(maxBytes: Int): String {
        if (toByteArray(StandardCharsets.UTF_8).size <= maxBytes) return this
        var end = length
        while (end > 0 && substring(0, end).toByteArray(StandardCharsets.UTF_8).size > maxBytes) {
            end--
        }
        return substring(0, end).trimEnd()
    }

    private fun startForegroundNotification() {
        val nm = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val ch = NotificationChannel(
                Constants.NOTIFICATION_CHANNEL_ID,
                getString(R.string.notification_channel_name),
                NotificationManager.IMPORTANCE_LOW
            ).apply {
                description = getString(R.string.notification_channel_desc)
            }
            nm.createNotificationChannel(ch)
        }

        val openIntent = Intent(this, MainActivity::class.java)
        val pi = PendingIntent.getActivity(
            this, 0, openIntent,
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )

        val n: Notification = Notification.Builder(this, Constants.NOTIFICATION_CHANNEL_ID)
            .setContentTitle(getString(R.string.app_name))
            .setContentText(getString(R.string.notification_text))
            .setSmallIcon(android.R.drawable.ic_popup_sync)
            .setContentIntent(pi)
            .setOngoing(true)
            .build()

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            startForeground(Constants.NOTIFICATION_ID, n, ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC)
        } else {
            startForeground(Constants.NOTIFICATION_ID, n)
        }
    }
}
