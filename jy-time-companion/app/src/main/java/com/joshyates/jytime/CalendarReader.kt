package com.joshyates.jytime

import android.content.ContentUris
import android.content.Context
import android.provider.CalendarContract
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.concurrent.TimeUnit

object CalendarReader {

    data class NextEvent(val title: String, val startMs: Long, val endMs: Long) {
        fun displayString(nowMs: Long = System.currentTimeMillis()): String {
            val prefix = if (nowMs in startMs until endMs) {
                "NOW"
            } else {
                SimpleDateFormat("ha", Locale.US).format(Date(startMs)).uppercase(Locale.US)
            }
            return "$prefix | $title"
        }

        fun countdownString(nowMs: Long = System.currentTimeMillis()): String {
            if (nowMs in startMs until endMs) return "NOW"
            val minutes = ((startMs - nowMs).coerceAtLeast(0L) + TimeUnit.MINUTES.toMillis(1) - 1) /
                TimeUnit.MINUTES.toMillis(1)
            if (minutes < 100) return "${minutes}m"
            val hours = (minutes + 59) / 60
            return if (hours < 100) "${hours}h" else "99+"
        }
    }

    /**
     * Query for the next upcoming calendar event title within the next [lookaheadHours]
     * across all visible calendars. Returns null if none found or permission missing.
     */
    fun getNextEvent(context: Context, lookaheadHours: Long = Constants.EVENT_LOOKAHEAD_HOURS): NextEvent? {
        val now = System.currentTimeMillis()
        val end = now + TimeUnit.HOURS.toMillis(lookaheadHours)

        val builder = CalendarContract.Instances.CONTENT_URI.buildUpon()
        ContentUris.appendId(builder, now)
        ContentUris.appendId(builder, end)
        val uri = builder.build()

        val projection = arrayOf(
            CalendarContract.Instances.TITLE,
            CalendarContract.Instances.BEGIN,
            CalendarContract.Instances.END,
            CalendarContract.Instances.ALL_DAY,
            CalendarContract.Instances.VISIBLE
        )

        // ALL_DAY=0 filters out full-day events which would otherwise dominate the next-event slot
        val selection = "${CalendarContract.Instances.VISIBLE}=1 " +
            "AND ${CalendarContract.Instances.ALL_DAY}=0 " +
            "AND ${CalendarContract.Instances.END}>?"

        return try {
            context.contentResolver.query(
                uri,
                projection,
                selection,
                arrayOf(now.toString()),
                "${CalendarContract.Instances.BEGIN} ASC"
            )?.use { c ->
                if (c.moveToFirst()) {
                    val title = c.getString(0) ?: return null
                    val start = c.getLong(1)
                    val endMs = c.getLong(2)
                    NextEvent(title, start, endMs)
                } else null
            }
        } catch (e: SecurityException) {
            null
        }
    }
}
