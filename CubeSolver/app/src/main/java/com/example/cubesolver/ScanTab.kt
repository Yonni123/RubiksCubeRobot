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
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import com.example.cubesolver.bluetooth.BluetoothHelper

import android.Manifest
import android.content.pm.PackageManager
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.ui.draw.clip
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.content.ContextCompat

@Composable
fun ScanTab(
    modifier: Modifier = Modifier,
    btHelper: BluetoothHelper,
    onCubeScanned: (String) -> Unit
) {
    val context = LocalContext.current

    var hasCameraPermission by remember {
        mutableStateOf(
            ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.CAMERA
            ) == PackageManager.PERMISSION_GRANTED
        )
    }

    val permissionLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestPermission()
    ) { granted ->
        hasCameraPermission = granted
    }

    // Ask permission when ScanTab first appears
    LaunchedEffect(Unit) {
        if (!hasCameraPermission) {
            permissionLauncher.launch(Manifest.permission.CAMERA)
        }
    }

    var flashEnabled by remember { mutableStateOf(false) }
    var scanning by remember { mutableStateOf(false) }
    var currentFace by remember { mutableStateOf(1) } // 1..6 cube faces

    val alpha = if (btHelper.isConnected) 1f else 0.4f
    val enabled = btHelper.isConnected

    Box(
        modifier = modifier
            .fillMaxSize()
            .padding(16.dp)
    ) {
        Column(
            verticalArrangement = Arrangement.spacedBy(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            modifier = Modifier.alpha(alpha) // dims everything when not connected
        ) {

            // --- Camera Preview Placeholder ---
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .aspectRatio(1f)
                    .border(2.dp, Color.Gray, RoundedCornerShape(8.dp))
                    .clip(RoundedCornerShape(8.dp)) // clip camera to the square
                    .background(Color.Black),
                contentAlignment = Alignment.Center
            ) {

                if (hasCameraPermission && btHelper.isConnected) {
                    CameraPreview(
                        modifier = Modifier.matchParentSize(),
                        flashEnabled = flashEnabled
                    )
                } else {
                    Box(
                        modifier = Modifier
                            .matchParentSize()
                            .background(Color.Black.copy(alpha = 0.2f)),
                        contentAlignment = Alignment.Center
                    ) {
                        Text(
                            text = if (!hasCameraPermission)
                                "Camera permission required"
                            else
                                "Connect robot to start scanning",
                            color = Color.White
                        )
                    }
                }

                // 🔹 3x3 grid overlay (unchanged)
                Canvas(modifier = Modifier.matchParentSize()) {
                    val w = size.width
                    val h = size.height

                    val thirdW = w / 3f
                    val thirdH = h / 3f

                    val stroke = 2.dp.toPx()
                    val gridColor = Color.White.copy(alpha = 0.6f)
                    val sampleColor = Color.Yellow.copy(alpha = 0.8f)

                    // --- Draw grid lines ---
                    for (i in 1..2) {
                        drawLine(
                            gridColor,
                            Offset(thirdW * i, 0f),
                            Offset(thirdW * i, h),
                            stroke
                        )
                        drawLine(
                            gridColor,
                            Offset(0f, thirdH * i),
                            Offset(w, thirdH * i),
                            stroke
                        )
                    }

                    // --- Draw sampling squares ---
                    val sampleScale = 0.4f // 40% of tile size
                    val sampleW = thirdW * sampleScale
                    val sampleH = thirdH * sampleScale

                    for (row in 0..2) {
                        for (col in 0..2) {
                            val tileLeft = col * thirdW
                            val tileTop = row * thirdH

                            val sampleLeft =
                                tileLeft + (thirdW - sampleW) / 2f
                            val sampleTop =
                                tileTop + (thirdH - sampleH) / 2f

                            drawRect(
                                color = sampleColor,
                                topLeft = Offset(sampleLeft, sampleTop),
                                size = androidx.compose.ui.geometry.Size(sampleW, sampleH),
                                style = androidx.compose.ui.graphics.drawscope.Stroke(
                                    width = 2.dp.toPx()
                                )
                            )
                        }
                    }
                }
            }

            // --- Info Text ---
            Text(
                text = "Face $currentFace of 6: Position cube in frame",
                style = MaterialTheme.typography.bodyLarge,
                color = Color.Gray
            )

            // --- Flash Toggle Button ---
            Button(
                onClick = { flashEnabled = !flashEnabled },
                enabled = enabled
            ) {
                Text(if (flashEnabled) "Flash ON" else "Flash OFF")
            }

            // --- Scanning Controls ---
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                Button(
                    onClick = { scanning = false },
                    enabled = scanning && enabled,
                    modifier = Modifier.weight(1f)
                ) { Text("Cancel") }

                Button(
                    onClick = { currentFace = (currentFace % 6) + 1 },
                    enabled = scanning && enabled,
                    modifier = Modifier.weight(1f)
                ) { Text("Next Face") }
            }
        }
    }
}

@Composable
fun CameraPreview(
    modifier: Modifier = Modifier,
    flashEnabled: Boolean
) {
    val context = LocalContext.current
    val lifecycleOwner = androidx.compose.ui.platform.LocalLifecycleOwner.current

    var camera by remember { mutableStateOf<androidx.camera.core.Camera?>(null) }

    AndroidView(
        modifier = modifier,
        factory = { ctx ->
            val previewView = androidx.camera.view.PreviewView(ctx).apply {
                scaleType = androidx.camera.view.PreviewView.ScaleType.FILL_CENTER
            }

            val cameraProviderFuture =
                androidx.camera.lifecycle.ProcessCameraProvider.getInstance(ctx)

            cameraProviderFuture.addListener({
                val cameraProvider = cameraProviderFuture.get()

                // 🔑 FORCE SQUARE OUTPUT
                val resolutionSelector =
                    androidx.camera.core.resolutionselector.ResolutionSelector.Builder()
                        .setAspectRatioStrategy(
                            androidx.camera.core.resolutionselector.AspectRatioStrategy(
                                androidx.camera.core.AspectRatio.RATIO_4_3,
                                androidx.camera.core.resolutionselector.AspectRatioStrategy.FALLBACK_RULE_AUTO
                            )
                        )
                        .build()

                val preview = androidx.camera.core.Preview.Builder()
                    .setResolutionSelector(resolutionSelector)
                    .build()
                    .also {
                        it.setSurfaceProvider(previewView.surfaceProvider)
                    }

                val cameraSelector =
                    androidx.camera.core.CameraSelector.DEFAULT_BACK_CAMERA

                cameraProvider.unbindAll()

                camera = cameraProvider.bindToLifecycle(
                    lifecycleOwner,
                    cameraSelector,
                    preview
                )

            }, ContextCompat.getMainExecutor(ctx))

            previewView
        }
    )

    LaunchedEffect(flashEnabled, camera) {
        camera?.cameraControl?.enableTorch(flashEnabled)
    }

    // Force 2× zoom, this is better for aspect ratio during scanning
    LaunchedEffect(camera) {
        camera?.cameraControl?.setZoomRatio(2.0f)
    }

    DisposableEffect(camera) {
        onDispose {
            camera?.cameraControl?.enableTorch(false)
        }
    }
}
