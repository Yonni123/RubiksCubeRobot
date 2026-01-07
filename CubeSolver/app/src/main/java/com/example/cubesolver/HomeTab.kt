package com.example.cubesolver.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.unit.dp
import androidx.compose.ui.graphics.Color
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.cubesolver.bluetooth.BluetoothHelper
import org.kociemba.twophase.Search
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

fun solveCube(cubeState: String): String {
    return Search.solution(
        cubeState,
        21,      // max depth
        1000,    // timeout (ms)
        false    // use separator
    )
}

class SolverViewModel : ViewModel() {
    var solverReady by mutableStateOf(false)
        private set

    init {
        warmUpSolver()
    }

    private fun warmUpSolver() {
        viewModelScope.launch(Dispatchers.Default) {
            Search.solution(
                "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB",
                0,
                1000,
                false
            )
            solverReady = true
        }
    }
}

var delay = 210  // delay between individual servo commands
private const val MIN_DELAY = 120    // fastest (100%)
private const val MAX_DELAY = 300   // slowest (10%)
fun setSpeed(percent: Int) {
    // Clamp input to 10–100
    val p = percent.coerceIn(10, 100)

    // Normalize to 0.0–1.0
    val t = (p - 10) / 90.0f

    // Interpolate delay (inverse relationship)
    delay = (MAX_DELAY - t * (MAX_DELAY - MIN_DELAY)).toInt()
}


@Composable
fun HomeTab(
    modifier: Modifier = Modifier,
    btHelper: BluetoothHelper,
    robotState: String,
    cubeState: String?,
    solverViewModel: SolverViewModel = androidx.lifecycle.viewmodel.compose.viewModel()
) {
    var solution by remember { mutableStateOf<String?>(null) }
    var solveSpeed by remember { mutableStateOf(0.5f) } // 0..1, default 50%

    val solverReady = solverViewModel.solverReady

    if (!solverReady) {
        // --- Open / Close Buttons at bottom ---
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(16.dp)
        ) {
            Button(
                onClick = {
                    // Send "OPEN" command to robot
                    btHelper.send("SEQ RRLRFRBR") { e ->
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
                    btHelper.send("SEQ RCLCFCBC") { e ->
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

        Box(
            modifier = Modifier.fillMaxSize(),
            contentAlignment = Alignment.Center
        ) {
            Card(
                elevation = CardDefaults.cardElevation(8.dp),
                modifier = Modifier.padding(16.dp)
            ) {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.spacedBy(12.dp),
                    modifier = Modifier.padding(24.dp)
                ) {
                    CircularProgressIndicator()
                    Text(
                        "Initializing solver…",
                        style = MaterialTheme.typography.titleMedium
                    )
                }
            }
        }
        return
    }


    LaunchedEffect(cubeState) {
        if (cubeState == null) {
            solution = null
        } else {
            solution = "Solving..."
            solution = try {
                withContext(Dispatchers.Default) {
                    solveCube(cubeState)
                }
            } catch (e: Exception) {
                "ERROR"
            }
        }
    }

    LaunchedEffect(solveSpeed) {
        setSpeed((solveSpeed * 100).toInt())
    }

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
                            onClick = { btHelper.send("SEQ C") },  // Cancel
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
                        onClick = {
                            solution?.let { sol ->
                                val parsed = parseCubeNotation(sol)  // convert solution
                                btHelper.send("MOVE $delay $parsed")        // send the parsed moves
                            }
                        },
                        //enabled = solution != null && enabled && !solution.toString().startsWith("Error"),
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
                        btHelper.send("SEQ RRLRFRBR") { e ->
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
                        btHelper.send("SEQ RCLCFCBC") { e ->
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

fun parseCubeNotation(solution: String): String {
    val moves = StringBuilder()
    val rawMoves = solution.split(" ")

    for (move in rawMoves) {
        if (move.isEmpty()) continue

        val face = move[0]
        val modifier = if (move.length > 1) move.substring(1) else ""

        when (modifier) {
            ""  -> moves.append(face)
            "'" -> moves.append(face.lowercase())
            "2" -> moves.append(face).append(face)
            else -> println("Unknown modifier: $move")
        }
    }

    return moves.toString()
}
