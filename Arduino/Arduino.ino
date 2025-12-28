#include "Calibrate.h"
#include "Servo.h"
#include "CubeController.h"

#define CALIBRATE false

void setup() {
#if CALIBRATE
  calibrateSetup();
#else
  Serial.begin(9600);
#endif
}

void loop() {
  sendPWMAll(servos, NUM_SERVOS);
#if CALIBRATE
    calibrateLoop();
#else
    cubeController.tick();
    if (!Serial.available())
        return;

    char c = Serial.read();

    if (c == 'f')
    {
        cubeController.executeMove(MOVE_F);
    }

    if (c == 'b')
    {
        cubeController.executeMove(MOVE_B);
    }

    if (c == 'r')
    {
        cubeController.executeMove(MOVE_R);
    }

    if (c == 'l')
    {
        cubeController.executeMove(MOVE_L);
    }
    
#endif
}
