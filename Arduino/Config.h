// Set these to the Arduino pin numbers connected to each servo
// If you change these, it is recommended to recalibrate the servos
#define RIGHT_SLIDER_PIN 4
#define RIGHT_SPINNER_PIN 5
#define LEFT_SLIDER_PIN 17
#define LEFT_SPINNER_PIN 19
#define FRONT_SLIDER_PIN 16
#define FRONT_SPINNER_PIN 15
#define BACK_SLIDER_PIN 3
#define BACK_SPINNER_PIN 2

// If true, run the servo calibration tool instead of normal operation
// Set true only when you want to recalibrate servos
// After running in calibration mode, open serial monitor and type "h" for help
// It will let you control individual servos to calibrate them perfectly
#define CALIBRATE false

// Size of the buffer to hold active sequences in SequenceManager (SEQ command)
// A move (like U) can be a sequence like "rR220RR220rCrC220RC220rR2".
// Solving the cube, expect 24 moves to be safe, this quickly blows up to over 1kB ish.
// Please try not to exceed this size or increase it if your RAM allows it.
#define SEQUENCE_BUFFER_SIZE 256

// Size of the buffer to hold individual moves in MOVE command
// God's number is 20, but just to be safe we allow a few extra moves
#define MOVE_BUFFER_SIZE 32


// Everything below is for the acceleration control of servos
// Acceleration ramps are important to reduce mechanical stress on servos and improve reliability
#define ACCELERATION 15          // µs per update²
#define MAX_VELOCITY 60         // µs per update
#define UPDATE_PERIOD_MS 5     // 100 Hz - servo update frequency
