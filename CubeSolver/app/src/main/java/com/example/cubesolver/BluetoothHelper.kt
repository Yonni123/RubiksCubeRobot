package com.example.cubesolver.bluetooth

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.util.Log
import kotlinx.coroutines.*
import java.io.IOException
import java.util.*
import androidx.compose.runtime.*

class BluetoothHelper {

    companion object {
        private val TAG = "BluetoothHelper"
        val HC06_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") // Standard SPP UUID
    }

    private var socket: BluetoothSocket? = null
    private var job: Job? = null

    // --- Observable state for Compose ---
    var isConnected by mutableStateOf(false)
        private set

    var lastMessage by mutableStateOf("")
        private set

    // --- Connect ---
    fun connect(
        device: BluetoothDevice,
        onConnected: () -> Unit = {},
        onFailed: (Exception) -> Unit = {}
    ) {
        // Close old socket if still exists
        if (socket != null) {
            disconnect()
        }

        job = CoroutineScope(Dispatchers.IO).launch {
            try {
                socket = device.createRfcommSocketToServiceRecord(HC06_UUID)
                socket?.connect()
                isConnected = true
                startReading() // start reading incoming messages
                withContext(Dispatchers.Main) {
                    onConnected() // call the callback when connection succeeds
                }
            } catch (e: IOException) {
                Log.e(TAG, "Bluetooth connection failed", e)
                isConnected = false
                try { socket?.close() } catch (_: Exception) {}
                withContext(Dispatchers.Main) {
                    onFailed(e)
                }
            }
        }
    }

    // --- Disconnect ---
    fun disconnect() {
        job?.cancel() // stop any ongoing coroutine
        try {
            socket?.outputStream?.close()
            socket?.inputStream?.close()
            socket?.close()
        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            socket = null
            isConnected = false
        }
    }

    // --- Get paired devices ---
    fun getPairedDevices(): List<BluetoothDevice> {
        val adapter = BluetoothAdapter.getDefaultAdapter()
        return adapter?.bondedDevices?.toList() ?: emptyList()
    }

    // --- Send data ---
    fun send(data: String, onError: (Exception) -> Unit = {}) {
        try {
            if (socket?.isConnected == true) {
                val output = socket!!.outputStream
                output.write(data.toByteArray()) // send UTF-8 bytes
                output.flush()
            } else {
                onError(IOException("Not connected"))
            }
        } catch (e: Exception) {
            onError(e)
        }
    }

    // --- Start reading incoming messages ---
    private fun startReading() {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                val input = socket?.inputStream ?: return@launch
                val buffer = ByteArray(1024)
                var bytes: Int
                while (isConnected && socket?.isConnected == true) {
                    bytes = input.read(buffer)
                    if (bytes > 0) {
                        val message = String(buffer, 0, bytes)
                        withContext(Dispatchers.Main) {
                            lastMessage = message // Compose UI reacts automatically
                        }
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }
}
