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
