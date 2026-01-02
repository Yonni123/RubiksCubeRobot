package com.example.cubesolver.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun ControlTab(modifier: Modifier = Modifier) {
    Box(
        modifier = modifier
            .fillMaxSize()
            .padding(16.dp),
        contentAlignment = Alignment.Center
    ) {
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .height(250.dp),
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(
                containerColor = Brush.horizontalGradient(
                    colors = listOf(Color(0xFFB3E5FC), Color(0xFF81D4FA))
                ).asComposeColor()
            ),
            elevation = CardDefaults.cardElevation(defaultElevation = 12.dp)
        ) {
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(24.dp),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                // Circle icon placeholder
                Box(
                    modifier = Modifier
                        .size(60.dp)
                        .background(Color.White.copy(alpha = 0.3f), shape = CircleShape)
                )
                Spacer(modifier = Modifier.height(16.dp))
                Text(
                    "Control tab is not implemented",
                    style = MaterialTheme.typography.titleMedium.copy(fontSize = 20.sp),
                    color = Color.Red
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    "This is a placeholder for future robot controls",
                    style = MaterialTheme.typography.bodyMedium,
                    color = Color.Red.copy(alpha = 0.9f)
                )
            }
        }
    }
}

// Helper to convert Brush to Compose Color for Card container
private fun Brush.asComposeColor() = Color.Transparent
