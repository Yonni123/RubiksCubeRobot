#include "Calibrate.h"
#include "CubeController.h"
#include "SequenceManager.h"

void setup() {
    Serial.begin(9600);
    calibrateSetup();   // Initialize EEPROM where calibrations are stored
}

void loop() {
  sendPWMAll(servos, NUM_SERVOS);
#if CALIBRATE
    calibrateLoop();
#else
    seqManager.tick();
    if (!Serial.available())
        return;

    char c = Serial.read();

    Serial.print("Received command: ");
    Serial.println(c);

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

    if (c == 'u')
    {
        cubeController.executeMove(MOVE_U);
    }

    if (c == 'd')
    {
        cubeController.executeMove(MOVE_D);
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

    if (c == 'y')
    {
        seqManager.startSequence("LUB'U'RL'BR'FB'DRD'F'");
    }

#endif
}
