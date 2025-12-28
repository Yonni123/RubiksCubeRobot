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

    Serial.print("Received command: ");
    Serial.println(c);

    if (c == 'f')
    {
        cubeController.executeMove(MOVE_FP);
    }

    if (c == 'b')
    {
        cubeController.executeMove(MOVE_BP);
    }

    if (c == 'r')
    {
        cubeController.executeMove(MOVE_RP);
    }

    if (c == 'l')
    {
        cubeController.executeMove(MOVE_LP);
    }

    if (c == 't')
    {
        cubeController.rotateCube(ORIENT_INVERT);
    }

    if (c == 'n')
    {
        cubeController.rotateCube(ORIENT_NORMAL);
    }

    if (c == 'o')
    {
        cubeController.openAllSliders();
    }

    if (c == 'c')
    {
        cubeController.closeAllSliders();
    }

#endif
}
