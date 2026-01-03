package com.example.cubesolver.bluetooth

import android.Manifest
import android.bluetooth.BluetoothDevice
import android.os.Build
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Dialog

@Composable
fun BluetoothConnectivity(
    bluetoothHelper: BluetoothHelper,
    onConnect: () -> Unit,
    onDisconnect: () -> Unit
) {
    var showBluetoothDialog by remember { mutableStateOf(false) }
    var devices by remember { mutableStateOf(listOf<BluetoothDevice>()) }
    var isConnecting by remember { mutableStateOf(false) }

    val permissionLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestPermission(),
        onResult = { granted ->
            if (granted) {
                devices = bluetoothHelper.getPairedDevices()
                showBluetoothDialog = true
            }
        }
    )

    // --- Bluetooth Card ---
    Card(modifier = Modifier.fillMaxWidth(), elevation = CardDefaults.cardElevation(4.dp)) {
        Row(
            modifier = Modifier
                .padding(16.dp)
                .fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text("Bluetooth:", style = MaterialTheme.typography.titleMedium)
            Text(
                text = if (bluetoothHelper.isConnected) "Connected" else if (isConnecting) "Connecting..." else "Disconnected",
                style = MaterialTheme.typography.titleMedium,
                color = if (bluetoothHelper.isConnected) Color(0xFF4CAF50) else Color.Red
            )
            Button(onClick = {
                if (bluetoothHelper.isConnected) {
                    bluetoothHelper.disconnect()
                    onDisconnect()
                } else {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                        permissionLauncher.launch(Manifest.permission.BLUETOOTH_CONNECT)
                    } else {
                        devices = bluetoothHelper.getPairedDevices()
                        showBluetoothDialog = true
                    }
                }
            }) {
                Text(if (bluetoothHelper.isConnected) "Disconnect" else "Connect")
            }
        }
    }

    // --- Device selection dialog ---
    if (showBluetoothDialog) {
        Dialog(onDismissRequest = { showBluetoothDialog = false }) {
            Card(
                modifier = Modifier.fillMaxWidth().padding(16.dp),
                shape = RoundedCornerShape(8.dp),
                elevation = CardDefaults.cardElevation(8.dp)
            ) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text("Select a device", style = MaterialTheme.typography.titleMedium)
                    Spacer(modifier = Modifier.height(12.dp))
                    LazyColumn(modifier = Modifier.heightIn(max = 300.dp)) {
                        items(devices) { device ->
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(vertical = 4.dp)
                                    .background(Color.LightGray, RoundedCornerShape(4.dp))
                                    .clickable {
                                        isConnecting = true
                                        showBluetoothDialog = false
                                        bluetoothHelper.connect(
                                            device,
                                            onConnected = {
                                                isConnecting = false
                                                onConnect()
                                            },
                                            onFailed = {
                                                isConnecting = false
                                                // Optional: show toast "Failed"
                                            }
                                        )
                                    }
                                    .padding(12.dp)
                            ) {
                                Text("${device.name} (${device.address})")
                            }
                        }
                    }
                }
            }
        }
    }
}
