package com.example.cubesolver.tabs

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.sp

@Composable
fun ScanTab(
    modifier: Modifier = Modifier,
    isBluetoothConnected: Boolean,
    onCubeScanned: (String) -> Unit
) {
    var flashEnabled by remember { mutableStateOf(false) }
    var scanning by remember { mutableStateOf(false) }
    var currentFace by remember { mutableStateOf(1) } // 1..6 cube faces

    Column(
        modifier = modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {

        // --- Camera Preview Placeholder ---
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .aspectRatio(1f) // makes it square
                .border(2.dp, Color.Gray, RoundedCornerShape(8.dp))
                .background(Color.Black.copy(alpha = 0.2f)), // placeholder background
            contentAlignment = Alignment.Center
        ) {
            Text("Camera Preview", color = Color.White)

            // --- 3x3 Grid Overlay ---
            Canvas(modifier = Modifier.matchParentSize()) {
                val width = size.width
                val height = size.height
                val thirdW = width / 3
                val thirdH = height / 3
                val lineColor = Color.White.copy(alpha = 0.6f)
                val strokeWidth = 2.dp.toPx()

                // vertical lines
                drawLine(lineColor, Offset(thirdW, 0f), Offset(thirdW, height), strokeWidth)
                drawLine(lineColor, Offset(thirdW * 2, 0f), Offset(thirdW * 2, height), strokeWidth)

                // horizontal lines
                drawLine(lineColor, Offset(0f, thirdH), Offset(width, thirdH), strokeWidth)
                drawLine(lineColor, Offset(0f, thirdH * 2), Offset(width, thirdH * 2), strokeWidth)
            }
        }

        // --- Info Text ---
        Text(
            text = "Face $currentFace of 6: Position cube in frame",
            style = MaterialTheme.typography.bodyLarge,
            color = Color.Gray
        )

        // --- Flash Toggle Button ---
        Button(onClick = { flashEnabled = !flashEnabled }) {
            Text(if (flashEnabled) "Flash ON" else "Flash OFF")
        }

        // --- Scanning Controls ---
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            Button(
                onClick = { currentFace = (currentFace % 6) + 1 },
                enabled = scanning,
                modifier = Modifier.weight(1f)
            ) { Text("Next Face") }

            Button(
                onClick = { scanning = false },
                enabled = scanning,
                modifier = Modifier.weight(1f)
            ) { Text("Cancel") }
        }
    }
}
