package com.example.cubesolver.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.unit.dp
import androidx.compose.ui.graphics.Color
import com.example.cubesolver.bluetooth.BluetoothHelper

@Composable
fun HomeTab(
    modifier: Modifier = Modifier,
    btHelper: BluetoothHelper,
    robotState: String,
    cubeState: String?
) {
    val solution = cubeState?.let { "R U R' U'" } // dummy solution
    var solveSpeed by remember { mutableStateOf(0.5f) } // 0..1, default 50%

    // Determine alpha and enable state based on Bluetooth connection
    val alpha = if (btHelper.isConnected) 1f else 0.4f
    val enabled = btHelper.isConnected

    Box(modifier = modifier.fillMaxSize().padding(16.dp)) {
        Column(
            verticalArrangement = Arrangement.spacedBy(24.dp),
            modifier = Modifier.alpha(alpha) // dims all content
        ) {

            // --- Robot State Card ---
            Card(modifier = Modifier.fillMaxWidth(), elevation = CardDefaults.cardElevation(4.dp)) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Column {
                            Text("Robot State", style = MaterialTheme.typography.titleMedium)
                            Spacer(modifier = Modifier.height(4.dp))
                            Text(
                                robotState,
                                color = if (robotState == "BUSY") Color.Red else Color(0xFF4CAF50),
                                style = MaterialTheme.typography.bodyLarge
                            )
                        }
                        Button(
                            onClick = { /* abort robot sequence */ },
                            enabled = robotState == "BUSY" && enabled // also check Bluetooth
                        ) {
                            Text("Abort")
                        }
                    }
                }
            }

            // --- Cube State Card ---
            Card(modifier = Modifier.fillMaxWidth(), elevation = CardDefaults.cardElevation(4.dp)) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text("Cube State", style = MaterialTheme.typography.titleMedium)
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(cubeState ?: "UNKNOWN", style = MaterialTheme.typography.bodyLarge)
                }
            }

            // --- Solution Card ---
            Card(modifier = Modifier.fillMaxWidth(), elevation = CardDefaults.cardElevation(4.dp)) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text("Solution", style = MaterialTheme.typography.titleMedium)
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(solution ?: "UNKNOWN", style = MaterialTheme.typography.bodyLarge)
                    Spacer(modifier = Modifier.height(12.dp))

                    // --- Solve Speed Slider ---
                    Column {
                        Text("Solve Speed: ${(solveSpeed * 100).toInt()}%", style = MaterialTheme.typography.bodyMedium)
                        Slider(
                            value = solveSpeed,
                            onValueChange = { solveSpeed = it },
                            valueRange = 0.1f..1f,
                            steps = 10,
                            enabled = enabled
                        )
                    }
                    Spacer(modifier = Modifier.height(12.dp))

                    Button(
                        onClick = { /* solve button clicked */ },
                        enabled = solution != null && enabled,
                        modifier = Modifier.align(Alignment.End)
                    ) {
                        Text("Solve")
                    }
                }
            }

            // --- Open / Close Buttons at bottom ---
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(16.dp)
            ) {
                Button(
                    onClick = {
                        // Send "OPEN" command to robot
                        btHelper.send("MOVE RRLRFRBR") { e ->
                            // Optional: handle error
                            println("Failed to send OPEN: ${e.message}")
                        }
                    },
                    enabled = robotState == "IDLE" && btHelper.isConnected,
                    modifier = Modifier.weight(1f)
                ) {
                    Text("Open")
                }

                Button(
                    onClick = {
                        // Send "Close" command to robot
                        btHelper.send("MOVE RCLCFCBC") { e ->
                            // Optional: handle error
                            println("Failed to send CLOSE: ${e.message}")
                        }
                    },
                    enabled = robotState == "IDLE" && btHelper.isConnected,
                    modifier = Modifier.weight(1f)
                ) {
                    Text("Close")
                }
            }
        }
    }
}
