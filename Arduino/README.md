# Rubik's Cube Robot

An autonomous robotic system that can visually scan a scrambled Rubik's cube, compute the optimal solution, and solve it through precise servo-controlled manipulation.

## Project Overview

This project combines:
- **Hardware**: Arduino-controlled robotic arm with 8 servo motors
- **Software**: Android mobile app for camera-based cube detection and Bluetooth control
- **Algorithms**: Kociemba two-phase algorithm for optimal cube solving

The system uses computer vision to analyze all 6 faces of a Rubik's cube, calculates the minimal solving sequence, and then executes it robotically.

## Hardware Architecture

### Servo Configuration

The robot has **8 servo motors** organized as 4 pairs:

| Position | Spinner (rotate) | Slider (manipulate) | Arduino Pins |
|----------|------------------|-------------------|--------------|
| RIGHT    | RIGHT_SPINNER (pin 5) | RIGHT_SLIDER (pin 4) | 5, 4 |
| LEFT     | LEFT_SPINNER (pin 19) | LEFT_SLIDER (pin 17) | 19, 17 |
| FRONT    | FRONT_SPINNER (pin 15) | FRONT_SLIDER (pin 16) | 15, 16 |
| BACK     | BACK_SPINNER (pin 2) | BACK_SLIDER (pin 3) | 2, 3 |

### Servo States

Each servo can move to 5 distinct positions:
- **STATE_C**: Center (fully centered)
- **STATE_L**: Left (fully extended left)
- **STATE_R**: Right (fully extended right)
- **STATE_l**: Center-left (45° between C and L)
- **STATE_r**: Center-right (45° between C and R)

### Communication

- **Protocol**: Serial communication at 9600 baud
- **Bluetooth Module**: HC-06 (for wireless Android app control)
- **Power**: Standard servo power rails

## Project Structure

```
RubiksCubeRobot/
├── Arduino/
│   ├── Arduino.ino              # Main sketch entry point
│   ├── API.cpp/h                # Command processing from Bluetooth
│   ├── SequenceManager.cpp/h    # Moves execution engine
│   ├── MyServo.cpp/h            # Individual servo control
│   ├── Calibrate.cpp/h          # Servo calibration tool
│   ├── Types.h                  # Enum definitions (ServoState, ServoType, etc.)
│   ├── Config.h                 # Pin mappings and buffer sizes
│   ├── Calibrations.info        # Stored servo calibrations
│   └── Calibrations.txt         # Human-readable calibration reference
│
└── CubeSolver/
    └── app/src/main/java/com/example/cubesolver/
        ├── MainActivity.kt          # App entry point
        ├── BluetoothHelper.kt       # Bluetooth connection management
        ├── Bluetooth.kt             # Bluetooth UI components
        ├── HomeTab.kt               # Home screen (solve controls)
        ├── ScanTab.kt               # Camera scanning & color detection
        ├── ControlTab.kt            # Manual robot control (placeholder)
        └── ui/theme/                # Material Design theming
```

## Arduino Code Components

### 1. **API Layer** (`API.cpp/h`)

Parses and executes commands received from the Bluetooth module:

- `SEQ <string>` - Execute a predefined servo sequence
- `MOVE <delay> <orientation> <moves>` - Execute standard Rubik's notation moves (U, D, L, R, F, B)
- `STATUS [servo]` - Query robot state (IDLE/BUSY) or individual servo state
- `PING` - Connection test

Example:
```
SEQ rRBL250fr  → Move right spinner to R, back slider to L, wait 250ms, front spinner to R, relax
MOVE 210 0 UFRF  → Solve the cube with these moves at 210ms intervals
```

### 2. **SequenceManager** (`SequenceManager.cpp/h`)

Executes servo moves at precise timing:
- Parses move strings (either custom sequences or standard cube notation)
- Manages timing with `tick()` called in main loop
- Handles cube rotations to reach different faces
- Tracks busy/idle state

Key methods:
- `startSequence(moveString)` - Begin executing a custom servo sequence
- `startMoves(cubeNotation, delayMs)` - Execute standard Rubik's moves
- `tick()` - Progress execution (call from main loop)

### 3. **MyServo** (`MyServo.cpp/h`)

Individual servo control:
- Manages PWM pulse width (250-3000 microseconds)
- Tracks current state and calibration
- Power management (attach/detach to save power)
- Calibration adjustments during setup

### 4. **Calibration Tool** (`Calibrate.cpp/h`)

Interactive servo calibration:
- Stores calibrations in EEPROM
- Serial menu for fine-tuning each servo
- Commands: `0-7` (select), `c/l/r` (mode), `+/-` (adjust), `w` (write)

Set `#define CALIBRATE true` in Config.h to enter calibration mode.

## Android App Components

### 1. **MainActivity.kt**

App entry point with 3 main tabs:
- **Home**: Solve controls and robot status
- **Scan**: Camera-based cube detection
- **Control**: Manual servo control (WIP)

### 2. **BluetoothHelper.kt**

Core Bluetooth communication:
- Connects to HC-06 module
- Sends move sequences
- Listens for robot state changes (IDLE/BUSY)
- Coroutine-based for non-blocking I/O

### 3. **ScanTab.kt**

Camera scanning and color detection:
- Phase 1: Captures all 6 cube faces using AndroidX Camera
- Phase 2: Processes images to detect 9 color tiles per face
- Uses color averaging and distance-based quantization
- Converts detected colors to Kociemba cube state format

**Color Detection Algorithm**:
1. Sample 40% of each 3×3 tile area
2. Average RGB values for that tile
3. Find closest match to 6 standard colors (White, Yellow, Red, Orange, Blue, Green)
4. Assign colors greedily (each color used max 9 times = 1 face)

### 4. **HomeTab.kt**

Main control screen:
- Displays cube state and solving solution
- Speed slider (10-100%) for execution speed
- Open/Close buttons for gripper
- Solve button to execute the algorithm

Uses Kociemba's `Search.solution()` with:
- Max depth: 21 moves
- Timeout: 1000ms
- No separator in output

### 5. **Color Quantization** (`ScanTab.kt`)

```kotlin
enum class CubeColor {
    WHITE(Color.White),
    YELLOW(Color.Yellow),
    RED(Color.Red),
    ORANGE(Color(1f, 0.5f, 0f)),
    BLUE(Color.Blue),
    GREEN(Color.Green)
}
```

## Command Protocol

### Arduino Serial Format

**Outgoing** (Arduino → Phone):
```
IDLE         # Robot is ready for new commands
BUSY         # Robot is executing a sequence
SEQ ERR -X   # Sequence execution error code
OK / ERR     # Command acknowledgment
```

**Incoming** (Phone → Arduino via API):
```
SEQ rRBL250fr            # Execute servo sequence
MOVE 210 0 UFRF          # Execute cube moves with 210ms delays
STATUS                   # Get robot state
STATUS r                 # Get right spinner state
PING                     # Ping for connection test
```

### Move Notation

**Servo sequences** (custom):
- Servos: `R/r`, `L/l`, `F/f`, `B/b` (uppercase=slider, lowercase=spinner)
- States: `C`, `L`, `R`, `l`, `r`
- Numbers: Delay in milliseconds
- Example: `"rRBL250fr"` = right spinner→R, back slider→L, wait 250ms, front→r, back→r

**Cube moves** (standard Rubik's notation):
- `U/u` - Up face (clockwise/counter-clockwise)
- `D/d` - Down face
- `L/l` - Left face
- `R/r` - Right face
- `F/f` - Front face
- `B/b` - Back face

## Workflow: Solving a Cube

1. **User starts app** → Connects to Arduino via Bluetooth (HC-06)
2. **Scan Phase**:
   - Opens camera tab
   - Robot rotates to expose each face
   - User taps "Scan Face" for each of 6 faces
   - App captures and processes images
3. **Processing**:
   - Converts color detection to Kociemba format
   - Sends to two-phase solver (embedded `.jar`)
   - Returns optimal move sequence
4. **Solving**:
   - User sets speed (slow = 300ms delays, fast = 120ms delays)
   - Taps "Solve"
   - App sends `MOVE` command with sequence
   - Arduino executes servo movements
   - Phone listens for IDLE/BUSY status updates
5. **Done**: Cube is solved, robot returns to rest position

## Configuration

### Arduino Config (`Config.h`)

```cpp
#define CALIBRATE false                    // Set true to calibrate servos
#define SEQUENCE_BUFFER_SIZE 256           // Max sequence length
#define MOVE_BUFFER_SIZE 64                // Max moves for solving (God's number is 20)
#define RIGHT_SLIDER_PIN 4                 // Servo pin mappings...
// ... etc
```

### Servo Calibrations (`Calibrations.info`)

Stores microsecond values for each servo's positions:
- `L_us`: Left position pulse width
- `R_us`: Right position pulse width
- `C_us`: Center position pulse width
- `CD_us`: Center deviation (spinners only, for gap compensation)

Example:
```
RIGHT_SPINNER: L=495us, R=2580us, C=1570us, Dev=70us
```

## Building & Flashing

### Arduino
1. Open `Arduino/Arduino.ino` in Arduino IDE
2. Select board (e.g., Arduino Mega 2560)
3. Set `CALIBRATE false` in `Config.h`
4. Upload sketch

### Android App
1. Open `CubeSolver/` in Android Studio
2. Build with Gradle
3. Deploy to Android 7+ device (API 24+)
4. Pair Bluetooth module first (Settings → Bluetooth)

## Troubleshooting

### Servo Jitter
- Increase power supply capacity (servos draw significant current)
- Check for loose servo connections

### Calibration Issues
- Set `CALIBRATE true`, flash, and use serial monitor to tune each servo
- Remember to set `CALIBRATE false` before solving

### Bluetooth Connection Fails
- Pair HC-06 module first (default PIN: 1234)
- Check baud rate is 9600
- Ensure Arduino is powered

### Colors Detected Incorrectly
- Ensure good lighting during scan
- Hold cube still for 1 second per face
- Manually adjust detected colors in review phase (tap tiles to cycle)

## Hardware Calibration Process

1. Connect Arduino to computer via USB
2. Set `#define CALIBRATE true` in `Config.h`
3. Flash the sketch
4. Open Serial Monitor (9600 baud)
5. Type commands:
   - `0-7`: Select servo (0=RIGHT_SPINNER, 1=RIGHT_SLIDER, etc.)
   - `c/l/r`: Select state to calibrate
   - `+/-`: Adjust by ±5µs
   - `*//`: Adjust by ±50µs
   - `</>`：Adjust center deviation (spinners only)
   - `p`: Print all calibrations
   - `w`: Write to EEPROM and done
6. Set `CALIBRATE false` and re-flash

## Dependencies

### Arduino
- Standard Arduino libraries (Servo.h, EEPROM.h)

### Android
- AndroidX (Core, Lifecycle, Compose, Camera)
- Material Design 3
- Kociemba two-phase solver (`twophase.jar` - must be in `app/libs/`)

## Technical Notes

- **Cube Representation**: Kociemba format (54 characters, 6 faces × 9 stickers each)
- **Optimal Solution**: Guaranteed ≤21 moves (God's number)
- **Servo Power**: Detaches servos after 3 seconds idle to save power
- **Timing**: Moves are synced to servo speed with configurable delays
- **Camera**: Uses CameraX for reliable image capture across Android versions

## Future Improvements

- Implement full manual control in ControlTab
- Add move visualization/preview
- Optimize color detection with ML model
- Support scramble input from photos
- Web interface for remote solving

---

**Created**: January 2026  
**Author**: Yonan  
**License**: Private/Research
