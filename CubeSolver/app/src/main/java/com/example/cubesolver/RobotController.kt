package com.example.cubesolver

import com.example.cubesolver.bluetooth.BluetoothHelper

var delay = 210  // delay between individual servo commands
private const val MIN_DELAY = 120    // fastest (100%)
private const val MAX_DELAY = 300   // slowest (10%)

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

class RobotController() {

    fun setSpeed(percent: Int) {
        // Clamp input to 10–100
        val p = percent.coerceIn(10, 100)

        // Normalize to 0.0–1.0
        val t = (p - 10) / 90.0f

        // Interpolate delay (inverse relationship)
        delay = (MAX_DELAY - t * (MAX_DELAY - MIN_DELAY)).toInt()
    }

    // ----------------- Cube orientation -----------------
    enum class Orientation {
        NORMAL,     // UP face is UP and inaccessible
        INVERTED    // RIGHT face is up and inaccessible (cube has flipped)
    }
    var orientation: Orientation = Orientation.NORMAL

    fun rotateCube(target: Orientation): String {
        if (orientation == target) {
            return ""   // Already in target orientation
        }

        // Build the command string to rotate the cube
        val command = buildString {
            append("FLBLRRLR${delay}")     // FRONT and BACK grab, RIGHT and LEFT release
            when (target) {
                Orientation.NORMAL -> {
                    append("fRfRbLbL${delay}")     // front spin right, back spin left, twice for true left and right
                }
                Orientation.INVERTED -> {
                    append("fLfLbRbR${delay}")     // front spin left, back spin right, twice for true left and right
                }
            }
            append("RLLL${delay}")         // RIGHT and LEFT grab
            append("FRBR${delay}")         // FRONT and BACK release
            append("fCfCbCbC${delay}")    // front and back spin to true center
            append("FCBC${delay}")         // FRONT and BACK sliders go back
            append("RCLC")                 // Relax RIGHT and LEFT sliders, they don't need to grab anymore
        }

        orientation = target    // Update current orientation

        return command
    }

    // ----------------- Face move functions -----------------
    // Each function returns a big command string that controls all servos for that face
    private fun U(): String {
        return buildString {
            append(rotateCube(Orientation.INVERTED))  // Returns empty if already INVERTED
            append("lR${delay}")    // Turn left spinner right, wait for it
            append("LR${delay}")    // Release Left slider
            append("lClC${delay}")  // Left spinner to center twice (for true center)
            append("LC${delay}")            // Left Slider Center
        }
    }

    private fun u(): String {
        return buildString {
            append(rotateCube(Orientation.INVERTED))  // Returns empty if already INVERTED
            append("lL${delay}")    // Turn left spinner left, wait for it
            append("LR${delay}")    // Release Left slider
            append("lClC${delay}")  // Left spinner to center twice (for true center)
            append("LC${delay}")            // Left Slider Center
        }
    }

    private fun D(): String {
        return buildString {
            append(rotateCube(Orientation.INVERTED))  // Returns empty if already INVERTED
            append("rR${delay}")    // Turn right spinner right, wait for it
            append("RR${delay}")    // Release Right slider
            append("rCrC${delay}")  // Right spinner to center twice (for true center)
            append("RC${delay}")            // Right Slider Center
        }
    }

    private fun d(): String {
        return buildString {
            append(rotateCube(Orientation.INVERTED))  // Returns empty if already INVERTED
            append("rL${delay}")    // Turn right spinner left, wait for it
            append("RR${delay}")    // Release Right slider
            append("rCrC${delay}")  // Right spinner to center twice (for true center)
            append("RC${delay}")            // Right Spinner Center
        }
    }

    private fun L(): String {
        return buildString {
            append(rotateCube(Orientation.NORMAL))  // Returns empty if already NORMAL
            append("lR${delay}")    // Turn left spinner right, wait for it
            append("LR${delay}")    // Release Left slider
            append("lClC${delay}")  // Left spinner to center twice (for true center)
            append("LC${delay}")            // Left Slider Center
        }
    }

    private fun l(): String {
        return buildString {
            append(rotateCube(Orientation.NORMAL))  // Returns empty if already NORMAL
            append("lL${delay}")    // Turn left spinner left, wait for it
            append("LR${delay}")    // Release Left slider
            append("lClC${delay}")  // Left spinner to center twice (for true center)
            append("LC${delay}")            // Left Slider Center
        }
    }

    private fun R(): String {
        return buildString {
            append(rotateCube(Orientation.NORMAL))  // Returns empty if already NORMAL
            append("rR${delay}")    // Turn right spinner right, wait for it
            append("RR${delay}")    // Release Right slider
            append("rCrC${delay}")  // Right spinner to center twice (for true center)
            append("RC${delay}")            // Right Slider Center
        }
    }

    private fun r(): String {
        return buildString {
            append(rotateCube(Orientation.NORMAL))  // Returns empty if already NORMAL
            append("rL${delay}")    // Turn right spinner left, wait for it
            append("RR${delay}")    // Release Right slider
            append("rCrC${delay}")  // Right spinner to center twice (for true center)
            append("RC${delay}")            // Right Spinner Center
        }
    }

    private fun F(): String {
        return buildString {
            append("fR${delay}")
            append("FR${delay}")
            append("fCrC${delay}")
            append("FC${delay}")
        }
    }

    private fun f(): String {
        return buildString {
            append("fL${delay}")
            append("FR${delay}")
            append("fCrC${delay}")
            append("FC${delay}")
        }
    }

    private fun B(): String {
        return buildString {
            append("bR${delay}")
            append("BR${delay}")
            append("bCrC${delay}")
            append("BC${delay}")
        }
    }

    private fun b(): String {
        return buildString {
            append("bL${delay}")
            append("BR${delay}")
            append("bCrC${delay}")
            append("BC${delay}")
        }
    }

    // Map single-character moves to functions
    private val moveMap = mapOf<Char, () -> String>(
        'U' to ::U,
        'u' to ::u,
        'D' to ::D,
        'd' to ::d,
        'L' to ::L,
        'l' to ::l,
        'R' to ::R,
        'r' to ::r,
        'F' to ::F,
        'f' to ::f,
        'B' to ::B,
        'b' to ::b
    )

    // ----------------- Execute full solution -----------------
    fun executeMoves(btHelper: BluetoothHelper, moves: String) {
        val parsed = parseCubeNotation(moves)

        // Build the full command string by calling each move's function
        val robotCommand = "MOVE " + parsed.map { move ->
            moveMap[move]?.invoke() ?: ""
        }.joinToString("")  // no separators, everything is one big string

        btHelper.send(robotCommand)
    }
}
