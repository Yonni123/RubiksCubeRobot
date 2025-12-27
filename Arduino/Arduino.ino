#include "calibrate.h"
#include "Servo.h"

#define CALIBRATE true

void setup() {
#if CALIBRATE
  calibrateSetup();
#else

#endif
}

void loop() {
  sendPWMAll(servos, NUM_SERVOS);
#if CALIBRATE
    calibrateLoop();
#else

#endif
}
