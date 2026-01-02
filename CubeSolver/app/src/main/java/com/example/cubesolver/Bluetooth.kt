package com.example.cubesolver.bluetooth

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.*
import androidx.compose.ui.window.Dialog


@Composable
fun BluetoothConnectivity(    isConnected: Boolean,
                              onConnect: () -> Unit,
                              onDisconnect: () -> Unit) {

    var showBluetoothDialog by remember { mutableStateOf(false) }

    // --- Connectivity Card ---
    Card(
        modifier = Modifier.fillMaxWidth(),
        elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
    ) {
        Row(
            modifier = Modifier
                .padding(16.dp)
                .fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "Bluetooth:",
                style = MaterialTheme.typography.titleMedium
            )
            Text(
                text = if (isConnected) "Connected" else "Disconnected",
                style = MaterialTheme.typography.titleMedium,
                color = if (isConnected) Color(0xFF4CAF50) else Color.Red // green if connected, red if not
            )
            Button(
                onClick = {
                    if (isConnected) {
                        onDisconnect()
                    } else {
                        showBluetoothDialog = true
                    }
                }
            ) {
                Text(if (isConnected) "Disconnect" else "Connect")
            }
        }
    }

    // --- Bluetooth Device Dialog ---
    if (showBluetoothDialog) {
        Dialog(onDismissRequest = { showBluetoothDialog = false }) {
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                shape = RoundedCornerShape(8.dp),
                elevation = CardDefaults.cardElevation(defaultElevation = 8.dp)
            ) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text("Select a device", style = MaterialTheme.typography.titleMedium)
                    Spacer(modifier = Modifier.height(12.dp))

                    // Dummy list of Bluetooth devices
                    val devices = listOf("HC-06", "OtherDevice1", "OtherDevice2", "Device3", "Device4", "HC-06", "OtherDevice1", "OtherDevice2", "Device3", "Device4", "HC-06", "OtherDevice1", "OtherDevice2", "Device3", "Device4", "HC-06", "OtherDevice1", "OtherDevice2", "Device3", "Device4")

                    // Make it scrollable if too many devices
                    LazyColumn(modifier = Modifier.heightIn(max = 300.dp)) {
                        items(devices) { device ->
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(vertical = 4.dp)
                                    .background(Color.LightGray, shape = RoundedCornerShape(4.dp))
                                    .clickable {
                                        onConnect()
                                        showBluetoothDialog = false
                                    }
                                    .padding(12.dp)
                            ) {
                                Text(device)
                            }
                        }
                    }
                }
            }
        }
    }
}