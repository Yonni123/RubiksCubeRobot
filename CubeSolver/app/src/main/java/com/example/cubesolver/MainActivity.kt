package com.example.cubesolver

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ControlCamera
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.CameraAlt
import androidx.compose.material3.Icon
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.adaptive.navigationsuite.NavigationSuiteScaffold
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.tooling.preview.PreviewScreenSizes
import com.example.cubesolver.ui.theme.CubeSolverTheme
import androidx.compose.ui.unit.dp

import com.example.cubesolver.tabs.HomeTab
import com.example.cubesolver.tabs.ScanTab
import com.example.cubesolver.tabs.ControlTab
import com.example.cubesolver.bluetooth.BluetoothConnectivity
import com.example.cubesolver.bluetooth.BluetoothHelper

class MainActivity : ComponentActivity() {
    // Shared Bluetooth helper for the whole app
    val bluetoothHelper by lazy { BluetoothHelper() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            CubeSolverTheme {
                CubeSolverApp(bluetoothHelper)
            }
        }
    }
}

@PreviewScreenSizes
@Composable
fun CubeSolverApp(btHelper: BluetoothHelper) {
    var currentDestination by rememberSaveable { mutableStateOf(AppDestinations.HOME) }

    // --- Shared app state ---
    var isConnected by remember { mutableStateOf(false) }
    var cubeState by remember { mutableStateOf<String?>(null) } // null = UNKNOWN
    var robotState by remember { mutableStateOf("IDLE") } // IDLE / BUSY

    NavigationSuiteScaffold(
        navigationSuiteItems = {
            AppDestinations.entries.forEach {
                item(
                    icon = { Icon(it.icon, contentDescription = it.label) },
                    label = { Text(it.label) },
                    selected = it == currentDestination,
                    onClick = { currentDestination = it }
                )
            }
        }
    ) {
        Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(innerPadding),
                verticalArrangement = Arrangement.spacedBy(16.dp)
            ) {

                // 🔹 Bluetooth UI
                BluetoothConnectivity(
                    bluetoothHelper = btHelper,  // your MainActivity instance
                    onConnect = { /* update any other state if needed */ },
                    onDisconnect = { /* update state */ }
                )

                // 🔹 Tabs
                when (currentDestination) {
                    AppDestinations.HOME -> {
                        HomeTab(
                            modifier = Modifier,
                            btHelper = btHelper,
                            robotState = robotState,
                            cubeState = cubeState
                        )
                    }

                    AppDestinations.SCAN -> {
                        ScanTab(
                            btHelper = btHelper,
                            onCubeScanned = { scannedCube ->
                                cubeState = scannedCube
                            }
                        )
                    }

                    AppDestinations.CONTROL -> {
                        ControlTab()
                    }
                }
            }
        }
    }
}

enum class AppDestinations(
    val label: String,
    val icon: ImageVector,
) {
    HOME("Home", Icons.Default.Home),
    SCAN("Scan", Icons.Default.CameraAlt),
    CONTROL("Control", Icons.Default.ControlCamera),
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    CubeSolverTheme {
        Greeting("Android")
    }
}