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

// MOVE FL0_BL0_RR0_LR0_fR250_fR0_bL0_bL0_ RC250_LC0_FR250_BR0_fC250_fC0_bC0_bC0_FC250_BC0
// MOVE FL0_BL0_RR0_LR0_fL250_fL0_bR0_bR0_ RC250_LC0_FR250_BR0_fC250_fC0_bC0_bC0_FC250_BC0
// MOVE rR0_RR250_rC250_rC0_RC250
// MOVE RC0_LC0_FC0_BC0

// MOVE FLBLRRLR250fRfRbLbL250RCLC250FRBR250fCfCbCbC250FCBC
