package com.example.cubesolver.bluetooth

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.util.Log
import java.io.IOException
import java.util.*
import kotlinx.coroutines.*

class BluetoothHelper {

    companion object {
        private val TAG = "BluetoothHelper"
        val HC06_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") // Standard SPP UUID
    }

    private var socket: BluetoothSocket? = null
    private var job: Job? = null

    var isConnected = false
        private set

    // Connect to a Bluetooth device
    fun connect(device: BluetoothDevice, onConnected: () -> Unit, onFailed: (Exception) -> Unit) {
        // Close old socket if still exists
        if (socket != null) {
            disconnect()
        }

        job = CoroutineScope(Dispatchers.IO).launch {
            try {
                socket = device.createRfcommSocketToServiceRecord(HC06_UUID)
                socket?.connect()
                isConnected = true
                withContext(Dispatchers.Main) { onConnected() }
            } catch (e: IOException) {
                Log.e(TAG, "Bluetooth connection failed", e)
                isConnected = false
                try { socket?.close() } catch (_: Exception) {}
                withContext(Dispatchers.Main) { onFailed(e) }
            }
        }
    }

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

    fun getPairedDevices(): List<BluetoothDevice> {
        val adapter = BluetoothAdapter.getDefaultAdapter()
        return adapter?.bondedDevices?.toList() ?: emptyList()
    }
}
