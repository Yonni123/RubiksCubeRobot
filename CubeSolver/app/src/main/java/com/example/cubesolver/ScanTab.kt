package com.example.cubesolver.tabs

import android.graphics.Bitmap
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.runtime.snapshots.SnapshotStateList
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.content.ContextCompat
import com.example.cubesolver.bluetooth.BluetoothHelper
import kotlinx.coroutines.launch
import java.nio.ByteBuffer
import kotlin.math.pow
import kotlin.math.sqrt
import kotlinx.coroutines.withContext
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlin.math.abs

/* ===================== STATE ===================== */

enum class ScanPhase {
    SCANNING,
    REVIEWING
}

/* ===================== COMPOSABLE ===================== */

@Composable
fun ScanTab(
    modifier: Modifier = Modifier,
    btHelper: BluetoothHelper,
    robotState: String,
    onCubeScanned: (String) -> Unit
) {
    var scanPhase by remember { mutableStateOf(ScanPhase.SCANNING) }
    var currentFace by remember { mutableStateOf(0) }
    var reviewFaceIndex by remember { mutableStateOf(0) }

    val scannedFaces = remember { mutableStateListOf<Bitmap?>().apply { repeat(6) { add(null) } } }
    val detectedFaces = remember {
        mutableStateListOf<SnapshotStateList<CubeColor>>()
    }
    var isProcessing by remember { mutableStateOf(false) }
    var scanningMessage by remember { mutableStateOf("Ready to start scanning") }
    var scanCompleted by remember { mutableStateOf(false) }
    var scanButtonPressed by remember { mutableStateOf(false) } // disables button immediately

    val coroutineScope = rememberCoroutineScope()
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    var camera by remember { mutableStateOf<Camera?>(null) }

    val imageCapture = remember {
        ImageCapture.Builder()
            .setCaptureMode(ImageCapture.CAPTURE_MODE_MINIMIZE_LATENCY)
            .build()
    }

    // Graying out UI and enabling/disabling based on Bluetooth
    val alpha = if (btHelper.isConnected) 1f else 0.4f
    val enabled = btHelper.isConnected

    // Observe robotState to re-enable button only when IDLE
    LaunchedEffect(robotState) {
        if (robotState == "IDLE") {
            scanButtonPressed = false
        }
    }

    Box(modifier = modifier.fillMaxSize()) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(16.dp)
                .alpha(alpha), // dims everything when disconnected
            horizontalAlignment = Alignment.CenterHorizontally
        ) {

            /* ---------- CAMERA + GRID ---------- */
            if (scanPhase == ScanPhase.SCANNING) {
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .aspectRatio(1f)
                        .border(2.dp, Color.Gray, RoundedCornerShape(8.dp))
                        .clip(RoundedCornerShape(8.dp))
                ) {
                    AndroidView(factory = { ctx ->
                        val previewView = PreviewView(ctx)
                        val providerFuture = ProcessCameraProvider.getInstance(ctx)
                        providerFuture.addListener({
                            val provider = providerFuture.get()
                            val preview = Preview.Builder().build().also { it.setSurfaceProvider(previewView.surfaceProvider) }
                            provider.unbindAll()
                            camera = provider.bindToLifecycle(
                                lifecycleOwner,
                                CameraSelector.DEFAULT_BACK_CAMERA,
                                preview,
                                imageCapture
                            )
                        }, ContextCompat.getMainExecutor(ctx))
                        previewView
                    })
                    CubeGridOverlay()
                }
            }

            /* ---------- CONFIRMATION GRID ---------- */
            if (scanPhase == ScanPhase.REVIEWING) {
                if (scanningMessage.isEmpty()) {
                    scanningMessage = "Confirm face ${reviewFaceIndex + 1}..."
                }

                FaceColorGrid(
                    colors = detectedFaces[reviewFaceIndex],
                    tileSize = 80.dp,
                    faceIndex = reviewFaceIndex
                )
            }

            Spacer(Modifier.height(24.dp))

            /* ---------- FEEDBACK TEXT ---------- */
            Text(
                text = when {
                    scanCompleted -> "Scanning complete! Go back to home screen."
                    isProcessing -> "Processing scanned images..."
                    scanningMessage.isNotEmpty() -> scanningMessage
                    else -> "Ready to start scanning"
                },
                style = MaterialTheme.typography.bodyMedium,
                color = if (scanCompleted) Color.Green else Color.White
            )

            Spacer(Modifier.height(16.dp))

            /* ---------- BUTTON ---------- */
            val scanButtonEnabled = robotState == "IDLE" && !isProcessing && !scanCompleted && enabled && !scanButtonPressed

            Button(
                enabled = scanButtonEnabled,
                onClick = {
                    scanButtonPressed = true // immediately disable

                    when (scanPhase) {
                        ScanPhase.SCANNING -> {
                            if (currentFace == 0) {
                                currentFace = 1
                                sendFaceCommand(btHelper, currentFace)
                                scanningMessage = "Scanning face 1..."
                            } else {
                                imageCapture.takePicture(
                                    ContextCompat.getMainExecutor(context),
                                    object : ImageCapture.OnImageCapturedCallback() {
                                        override fun onCaptureSuccess(image: ImageProxy) {
                                            val bitmap = image.toBitmap()
                                            image.close()
                                            scannedFaces[currentFace - 1] = bitmap
                                            scanningMessage = "Face $currentFace captured!"

                                            if (currentFace == 6) {
                                                isProcessing = true
                                                scanningMessage = "Processing all faces..."
                                                coroutineScope.launch {
                                                    detectedFaces.clear()
                                                    processCubeImages(scannedFaces.filterNotNull())
                                                        .forEach { face ->
                                                            detectedFaces.add(face.toMutableStateList())
                                                        }
                                                    scanPhase = ScanPhase.REVIEWING
                                                    // Recover from last face scan
                                                    reviewFaceIndex = 0
                                                    sendFaceCommand(btHelper, 7)
                                                    isProcessing = false
                                                    scanningMessage = "Confirm face ${reviewFaceIndex + 1}..."
                                                    // Button will auto re-enable via LaunchedEffect when robot IDLE
                                                }
                                            } else {
                                                currentFace++
                                                sendFaceCommand(btHelper, currentFace)
                                                scanningMessage = "Scanning face $currentFace..."
                                            }
                                        }
                                    }
                                )
                            }
                        }

                        ScanPhase.REVIEWING -> {
                            if (reviewFaceIndex < 5) {
                                reviewFaceIndex++
                                sendFaceCommand(btHelper, reviewFaceIndex + 1)
                                scanningMessage = "Confirm face ${reviewFaceIndex + 1}..."
                            } else {
                                scanCompleted = true
                                sendFaceCommand(btHelper, 8)
                                val cubeState = buildCubeStateFromColors(detectedFaces)
                                onCubeScanned(cubeState)
                                scanningMessage = ""
                            }
                        }
                    }
                }
            ) {
                Text(
                    when {
                        scanCompleted -> "Scanning Done"
                        scanPhase == ScanPhase.SCANNING -> if (currentFace == 0) "Start Scanning" else "Scan Face"
                        scanPhase == ScanPhase.REVIEWING -> "Confirm"
                        else -> "Start"
                    }
                )
            }
        }

        /* ---------- PROCESSING OVERLAY ---------- */
        if (isProcessing) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .background(Color.Black.copy(alpha = 0.5f)),
                contentAlignment = Alignment.Center
            ) {
                CircularProgressIndicator(color = Color.White)
            }
        }
    }
}

/* ===================== IMAGE PROCESSING ===================== */

fun processCubeImages(images: List<Bitmap>): List<List<CubeColor>> {
    val rawFaces: List<List<Color>> = images.map { faceBitmap ->
        val sampled = sampleFaceColorsRaw(faceBitmap)

        // Rotate the sampled tiles 45° clockwise to correct the orientation
        val rotateMap = listOf(6, 3, 0, 7, 4, 1, 8, 5, 2)
        sampled.mapIndexed { i, _ -> sampled[rotateMap[i]] }
    }
    val rawCenters: List<Color> = rawFaces.map { it[4] }

    // Step 1: assign each center a unique CubeColor
    val availableColors = CubeColor.values().toMutableList()
    val centerCubeColors = mutableListOf<CubeColor>()
    for (centerColor in rawCenters) {
        val closest = availableColors.minBy { distColor(centerColor, it.display) }
        centerCubeColors += closest
        availableColors.remove(closest)
    }

    // Step 2: prepare tile list (excluding centers)
    data class Tile(val faceIndex: Int, val tileIndex: Int, val color: Color)
    val tiles = mutableListOf<Tile>()
    for ((faceIndex, face) in rawFaces.withIndex()) {
        for ((tileIndex, tileColor) in face.withIndex()) {
            if (tileIndex != 4) tiles += Tile(faceIndex, tileIndex, tileColor)
        }
    }

    // Step 3: compute all distances between tiles and centers
    data class TileCenterDist(val tile: Tile, val centerIndex: Int, val distance: Float)
    val distances = mutableListOf<TileCenterDist>()
    for ((centerIndex, centerColor) in rawCenters.withIndex()) {
        for (tile in tiles) {
            val dist = distColor(tile.color, centerColor)
            distances += TileCenterDist(tile, centerIndex, dist)
        }
    }

    // Step 4: sort all tile-center pairs by distance (ascending)
    val sortedDistances = distances.sortedBy { it.distance }

    // Step 5: assign tiles iteratively to centers (max 8 per center)
    val assignedTiles = mutableMapOf<Pair<Int, Int>, CubeColor>() // (faceIndex, tileIndex) -> CubeColor
    val centerCounts = MutableList(6) { 1 } // center already counted

    for (pair in sortedDistances) {
        val key = pair.tile.faceIndex to pair.tile.tileIndex
        if (key in assignedTiles) continue
        val centerIdx = pair.centerIndex
        if (centerCounts[centerIdx] < 9) {
            assignedTiles[key] = centerCubeColors[centerIdx]
            centerCounts[centerIdx]++
        }
    }

    // Step 6: build final faces list
    val finalFaces = mutableListOf<List<CubeColor>>()
    for ((faceIndex, face) in rawFaces.withIndex()) {
        val newFace = mutableListOf<CubeColor>()
        for ((tileIndex, _) in face.withIndex()) {
            if (tileIndex == 4) {
                newFace += centerCubeColors[faceIndex] // center
            } else {
                newFace += assignedTiles[faceIndex to tileIndex]
                    ?: centerCubeColors[faceIndex] // fallback (should not happen)
            }
        }
        finalFaces += newFace
    }

    return finalFaces
}

private fun distColor(c1: Color, c2: Color): Float {
    return sqrt(
        (c1.red - c2.red).pow(2) +
                (c1.green - c2.green).pow(2) +
                (c1.blue - c2.blue).pow(2)
    )
}

fun sampleFaceColorsRaw(bitmap: Bitmap): List<Color> {
    val w = bitmap.width
    val h = bitmap.height
    val tileW = w / 3
    val tileH = h / 3
    val sampleScale = 0.4f

    val colors = mutableListOf<Color>()

    for (row in 0..2) {
        for (col in 0..2) {
            val sw = (tileW * sampleScale).toInt()
            val sh = (tileH * sampleScale).toInt()
            val x = col * tileW + (tileW - sw) / 2
            val y = row * tileH + (tileH - sh) / 2

            colors += averageColor(bitmap, x, y, sw, sh)
        }
    }
    return colors
}

fun averageColor(bmp: Bitmap, x: Int, y: Int, w: Int, h: Int): Color {
    var r = 0L
    var g = 0L
    var b = 0L
    var count = 0

    for (iy in y until (y + h)) {
        for (ix in x until (x + w)) {
            val p = bmp.getPixel(ix, iy)
            r += android.graphics.Color.red(p)
            g += android.graphics.Color.green(p)
            b += android.graphics.Color.blue(p)
            count++
        }
    }

    return Color(
        (r / count) / 255f,
        (g / count) / 255f,
        (b / count) / 255f
    )
}

/* ===================== COLOR QUANTIZATION ===================== */

enum class CubeColor(val display: Color) {
    WHITE(Color.White),
    YELLOW(Color.Yellow),
    RED(Color.Red),
    ORANGE(Color(1f, 0.5f, 0f)),
    BLUE(Color.Blue),
    GREEN(Color.Green)
}

fun CubeColor.next(): CubeColor {
    val all = CubeColor.values()
    return all[(this.ordinal + 1) % all.size]
}
/* ===================== UI HELPERS ===================== */

@Composable
fun FaceColorGrid(
    colors: MutableList<CubeColor>,
    tileSize: Dp,
    faceIndex: Int // 0-based, Face1 = 0
) {
    // Define labels for each face in scan order
    val faceLabels = listOf(
        listOf("R1","R2","R3","R4","R5","R6","R7","R8","R9"), // Face1
        listOf("L9","L8","L7","L6","L5","L4","L3","L2","L1"), // Face2
        listOf("B3","B6","B9","B2","B5","B8","B1","B4","B7"), // Face3
        listOf("F7","F4","F1","F8","F5","F2","F9","F6","F3"), // Face4
        listOf("D3","D6","D9","D2","D5","D8","D1","D4","D7"), // Face5
        listOf("U7","U4","U1","U8","U5","U2","U9","U6","U3")  // Face6
    )

    Column {
        for (row in 0..2) {
            Row {
                for (col in 0..2) {
                    val index = row * 3 + col
                    Box(
                        contentAlignment = Alignment.Center,
                        modifier = Modifier
                            .size(tileSize)
                            .background(colors[index].display)
                            .border(2.dp, Color.Black)
                            .clickable {
                                // allow user to cycle the color
                                colors[index] = colors[index].next()
                            }
                    ) {
                        Text(
                            text = faceLabels[faceIndex][index],
                            style = MaterialTheme.typography.labelSmall,
                            color = Color.Black
                        )
                    }
                }
            }
        }
    }
}

@Composable
fun CubeGridOverlay() {
    Canvas(Modifier.fillMaxSize()) {
        val w = size.width
        val h = size.height
        val thirdW = w / 3f
        val thirdH = h / 3f
        val stroke = 2.dp.toPx()

        val gridColor = Color.White.copy(alpha = 0.6f)
        val sampleColor = Color.Yellow.copy(alpha = 0.8f)
        val sampleScale = 0.4f

        for (i in 1..2) {
            drawLine(gridColor, Offset(thirdW * i, 0f), Offset(thirdW * i, h), stroke)
            drawLine(gridColor, Offset(0f, thirdH * i), Offset(w, thirdH * i), stroke)
        }

        val sw = thirdW * sampleScale
        val sh = thirdH * sampleScale

        for (row in 0..2) {
            for (col in 0..2) {
                val x = col * thirdW + (thirdW - sw) / 2
                val y = row * thirdH + (thirdH - sh) / 2
                drawRect(
                    sampleColor,
                    Offset(x, y),
                    Size(sw, sh),
                    style = androidx.compose.ui.graphics.drawscope.Stroke(2.dp.toPx())
                )
            }
        }
    }
}

/* ===================== ROBOT ===================== */

private fun sendFaceCommand(btHelper: BluetoothHelper, face: Int) {
    var delay = 250
    var cmd = "MOVE "

    if (face == 1) {
        cmd += "rrFLBLRRLR${delay}fLbRFCBClr0"  // Scan RIGHT side
    } else if (face == 2) {
        cmd += "rrlCFLBL${delay}RRLR${delay}fCfCbCbC${delay}FCBCFCBCRCLC${delay}rC"  // RECOVER
        cmd += "lr${delay}FLBLRRLR${delay}fRbLFCBCrr0" // Scan LEFT side
    } else if (face == 3) {
        cmd += "lrrCFLBLRRLR${delay}fCfCbCbC${delay}FCBCFCBCRCLC${delay}lC"  // RECOVER
        cmd += "brRLLLBRFR${delay}rLlRRCLCfr0"  // Scan BACK side
    } else if (face == 4) {
        cmd += "brfCRLLLBRFR${delay}rCrClClC${delay}RCLCRCLCBCFC${delay}bC"  // Recover
        cmd += "frRLLLBRFR${delay}rRlLRCLCbr0" // Scan FRONT side
    } else if (face == 5) {
        cmd += "frbCfrbCRLLLBRFR${delay}rCrClClC${delay}RCLCRCLCBCFC${delay}fC"  // Recover
        cmd += "${delay}FLBLRRLR${delay}fLfLbRbR${delay}RLLL${delay}FRBR${delay}fCbCfCbC${delay}FCBC${delay}RCLC" // Rotate cube
        cmd += "rrFLBLRRLR${delay}fLbRFCBClr0" // Scan DOWN side
    } else if (face == 6) {
        cmd += "rrlCFLBLRRLR${delay}fCfCbCbC${delay}FCBCFCBCRCLC${delay}rC"  // Recover
        cmd += "lrFLBLRRLR${delay}fRbLFCBCrr0" // Scan UP side
    } else if (face == 7 || face == 8) { // Start or End of review phase
        cmd += "lrrCFLBLRRLR${delay}fCfCbCbC${delay}FCBCFCBCRCLC${delay}lC"  // Recover from face 6
        cmd += "${delay}FLBLRRLR${delay}fRfRbLbL${delay}RLLL${delay}FRBR${delay}fCbCfCbC${delay}FCBC${delay}RCLC" // Flip the cube back
        if (face == 7) {    // If start review, setup first face
            cmd += "rrFLBLRRLR${delay}fLbRFCBClr0"  // Show face RIGHT for review
        }
    }
    else {
        return
    }

    btHelper.send(cmd)
}

/* ===================== UTIL ===================== */

fun buildCubeStateFromColors(faces: List<List<CubeColor>>): String {
    val kociembaOrder = listOf(5,0,3,4,1,2) // U,R,F,D,L,B

    // Step 1: map centers to letters
    val centerColors = listOf(
        faces[5][4], // U
        faces[0][4], // R
        faces[3][4], // F
        faces[4][4], // D
        faces[1][4], // L
        faces[2][4]  // B
    )
    val kociembaLetters = listOf('U','R','F','D','L','B')
    val colorToFaceLetter = centerColors.zip(kociembaLetters).toMap()

    // Step 2: sticker maps (camera scan → Kociemba order)
    val stickerMaps = mapOf(
        0 to listOf(0,1,2,3,4,5,6,7,8), // R
        1 to listOf(8,7,6,5,4,3,2,1,0), // L
        2 to listOf(6,3,0,7,4,1,8,5,2), // B
        3 to listOf(2,5,8,1,4,7,0,3,6), // F
        4 to listOf(6,3,0,7,4,1,8,5,2), // D
        5 to listOf(2,5,8,1,4,7,0,3,6)  // U
    )

    val cubeString = StringBuilder(54)
    for (faceIndex in kociembaOrder) {
        val face = faces[faceIndex]
        val map = stickerMaps[faceIndex]!!
        for (i in map) {
            val color = face[i]
            cubeString.append(colorToFaceLetter[color])
        }
    }

    return cubeString.toString()
}
