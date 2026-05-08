package com.joshyates.jytime

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent

class BootReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == Intent.ACTION_BOOT_COMPLETED ||
            intent.action == "android.intent.action.QUICKBOOT_POWERON") {
            // Only start if user previously enabled — read from preferences
            val prefs = context.getSharedPreferences(Constants.PREFS_NAME, Context.MODE_PRIVATE)
            if (prefs.getBoolean(Constants.PREF_ENABLED, false)) {
                WatchSyncService.start(context)
            }
        }
    }
}
