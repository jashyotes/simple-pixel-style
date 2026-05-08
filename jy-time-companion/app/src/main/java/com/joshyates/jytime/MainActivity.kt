package com.joshyates.jytime

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.core.content.ContextCompat
import java.text.DateFormat
import java.util.Date

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { padding ->
                    AppRoot(modifier = Modifier.fillMaxSize().padding(padding))
                }
            }
        }
    }
}

private fun missingPermissions(context: Context): List<String> {
    val needed = mutableListOf<String>()
    if (ContextCompat.checkSelfPermission(context, Manifest.permission.READ_CALENDAR)
        != PackageManager.PERMISSION_GRANTED) needed += Manifest.permission.READ_CALENDAR
    if (ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION)
        != PackageManager.PERMISSION_GRANTED) needed += Manifest.permission.ACCESS_COARSE_LOCATION
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
        if (ContextCompat.checkSelfPermission(context, Manifest.permission.POST_NOTIFICATIONS)
            != PackageManager.PERMISSION_GRANTED) needed += Manifest.permission.POST_NOTIFICATIONS
    }
    return needed
}

private data class SyncStatus(
    val lastSync: Long,
    val lastEvent: String?,
    val lastBattery: Int?,
    val lastWmo: Int?,
    val lastTemp: Int?,
    val lastRain: Int?
)

private fun readSyncStatus(context: Context): SyncStatus {
    val prefs = context.getSharedPreferences(Constants.PREFS_NAME, Context.MODE_PRIVATE)
    return SyncStatus(
        lastSync = prefs.getLong(Constants.PREF_LAST_SYNC_MS, 0L),
        lastEvent = prefs.getString(Constants.PREF_LAST_EVENT, null),
        lastBattery = prefs.getInt(Constants.PREF_LAST_BATTERY, -1).takeIf { it >= 0 },
        lastWmo = prefs.getInt(Constants.PREF_LAST_WMO, -1).takeIf { it >= 0 },
        lastTemp = prefs.getInt(Constants.PREF_LAST_TEMP, Int.MIN_VALUE).takeIf { it != Int.MIN_VALUE },
        lastRain = prefs.getInt(Constants.PREF_LAST_RAIN, -1).takeIf { it >= 0 }
    )
}

@Composable
private fun AppRoot(modifier: Modifier = Modifier) {
    val context = androidx.compose.ui.platform.LocalContext.current
    val prefs = remember { context.getSharedPreferences(Constants.PREFS_NAME, Context.MODE_PRIVATE) }

    var enabled by remember { mutableStateOf(prefs.getBoolean(Constants.PREF_ENABLED, false)) }
    var topSteps by remember { mutableStateOf(prefs.getBoolean(Constants.PREF_TOP_STEPS, false)) }
    var permissionsMissing by remember { mutableStateOf(missingPermissions(context)) }
    var status by remember { mutableStateOf(readSyncStatus(context)) }

    val permLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestMultiplePermissions()
    ) {
        permissionsMissing = missingPermissions(context)
    }

    Column(
        modifier = modifier.verticalScroll(rememberScrollState()).padding(24.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text(text = "JY Time Companion", style = MaterialTheme.typography.headlineMedium)
        Text(
            text = "Sends your next calendar event, phone battery %, and weather to your Pebble Time 2 watchface every minute.",
            style = MaterialTheme.typography.bodyMedium
        )

        if (permissionsMissing.isNotEmpty()) {
            Text("Missing permissions: ${permissionsMissing.size}", style = MaterialTheme.typography.bodyMedium)
            Button(onClick = { permLauncher.launch(permissionsMissing.toTypedArray()) }) {
                Text("Grant permissions")
            }
        }

        Spacer(Modifier.size(8.dp))
        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            Text(if (enabled) "Service: running" else "Service: stopped",
                style = MaterialTheme.typography.titleMedium)
            Switch(
                checked = enabled,
                enabled = permissionsMissing.isEmpty(),
                onCheckedChange = { newVal ->
                    enabled = newVal
                    prefs.edit().putBoolean(Constants.PREF_ENABLED, newVal).apply()
                    if (newVal) WatchSyncService.start(context)
                    else WatchSyncService.stop(context)
                }
            )
        }

        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            Text("Top W800 step counter", style = MaterialTheme.typography.titleMedium)
            Switch(
                checked = topSteps,
                onCheckedChange = { newVal ->
                    topSteps = newVal
                    prefs.edit().putBoolean(Constants.PREF_TOP_STEPS, newVal).apply()
                    if (enabled) WatchSyncService.syncNow(context)
                }
            )
        }

        Spacer(Modifier.size(8.dp))
        Button(onClick = { status = readSyncStatus(context) }) {
            Text("Refresh status")
        }
        Text(
            text = if (status.lastSync > 0)
                "Last sync: " + DateFormat.getTimeInstance(DateFormat.MEDIUM).format(Date(status.lastSync))
            else "Last sync: never (start the service)",
            style = MaterialTheme.typography.bodySmall
        )
        Text("Next event: ${status.lastEvent ?: "—"}", style = MaterialTheme.typography.bodySmall)
        Text("Phone battery: ${status.lastBattery?.let { "$it%" } ?: "—"}", style = MaterialTheme.typography.bodySmall)
        Text(
            "Weather: " + (status.lastWmo?.let { "code $it" } ?: "—") + ", " +
                (status.lastTemp?.let { "${it}°F" } ?: "—") + ", rain " +
                (status.lastRain?.let { "$it%" } ?: "—"),
            style = MaterialTheme.typography.bodySmall
        )
    }
}
