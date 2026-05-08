package com.joshyates.jytime

import android.annotation.SuppressLint
import android.content.Context
import android.location.Location
import com.google.android.gms.location.LocationServices
import kotlinx.coroutines.suspendCancellableCoroutine
import okhttp3.OkHttpClient
import okhttp3.Request
import org.json.JSONObject
import java.util.concurrent.TimeUnit
import kotlin.coroutines.resume
import kotlin.math.roundToInt

class WeatherFetcher(private val context: Context) {

    data class Weather(
        val wmoCode: Int,
        val temperatureF: Int,
        val rainChance: Int?,
        val feelsLikeF: Int?,
        val highTempF: Int?,
        val windMph: Int?,
        val uvIndex: Int?,
        val fetchedAtMs: Long
    )

    private val client = OkHttpClient.Builder()
        .connectTimeout(15, TimeUnit.SECONDS)
        .readTimeout(15, TimeUnit.SECONDS)
        .build()

    @Volatile private var cached: Weather? = null

    suspend fun fetch(): Weather? {
        cached?.let {
            if (System.currentTimeMillis() - it.fetchedAtMs < Constants.WEATHER_CACHE_MS) return it
        }
        val loc = lastKnownLocation() ?: return cached
        val w = fetchOpenMeteo(loc.latitude, loc.longitude)
        if (w != null) cached = w
        return w ?: cached
    }

    @SuppressLint("MissingPermission")
    private suspend fun lastKnownLocation(): Location? = suspendCancellableCoroutine { cont ->
        try {
            val client = LocationServices.getFusedLocationProviderClient(context)
            client.lastLocation
                .addOnSuccessListener { loc -> cont.resume(loc) }
                .addOnFailureListener { cont.resume(null) }
        } catch (e: SecurityException) {
            cont.resume(null)
        }
    }

    private fun fetchOpenMeteo(lat: Double, lon: Double): Weather? {
        val url = "https://api.open-meteo.com/v1/forecast?" +
            "latitude=$lat&longitude=$lon&current=temperature_2m,apparent_temperature,weather_code,wind_speed_10m" +
            "&hourly=precipitation_probability&daily=temperature_2m_max,uv_index_max" +
            "&forecast_days=1&temperature_unit=fahrenheit&wind_speed_unit=mph&timezone=auto"
        return try {
            client.newCall(Request.Builder().url(url).build()).execute().use { resp ->
                if (!resp.isSuccessful) return null
                val body = resp.body?.string() ?: return null
                val json = JSONObject(body)
                val current = json.optJSONObject("current") ?: return null
                val tempF = current.optDouble("temperature_2m", Double.NaN)
                val code = current.optInt("weather_code", -1)
                if (tempF.isNaN() || code < 0) return null
                val feelsLike = current.roundedOrNull("apparent_temperature")
                val wind = current.roundedOrNull("wind_speed_10m")
                val hourly = json.optJSONObject("hourly")
                val times = hourly?.optJSONArray("time")
                val pops = hourly?.optJSONArray("precipitation_probability")
                val daily = json.optJSONObject("daily")
                val highTemp = daily?.firstRoundedOrNull("temperature_2m_max")
                val uvIndex = daily?.firstRoundedOrNull("uv_index_max")
                val currentTime = current.optString("time")
                var rain: Int? = null
                if (times != null && pops != null && currentTime.isNotBlank()) {
                    for (i in 0 until minOf(times.length(), pops.length())) {
                        if (times.optString(i) >= currentTime) {
                            rain = pops.optInt(i, -1).takeIf { it >= 0 }
                            break
                        }
                    }
                }
                Weather(
                    code,
                    tempF.roundToInt(),
                    rain,
                    feelsLike,
                    highTemp,
                    wind,
                    uvIndex,
                    System.currentTimeMillis()
                )
            }
        } catch (e: Exception) {
            null
        }
    }

    private fun JSONObject.roundedOrNull(name: String): Int? {
        val value = optDouble(name, Double.NaN)
        return if (value.isNaN()) null else value.roundToInt()
    }

    private fun JSONObject.firstRoundedOrNull(name: String): Int? {
        val values = optJSONArray(name) ?: return null
        if (values.length() == 0) return null
        val value = values.optDouble(0, Double.NaN)
        return if (value.isNaN()) null else value.roundToInt()
    }
}
