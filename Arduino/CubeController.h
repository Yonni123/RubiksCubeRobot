#pragma once
#include "Servo.h"

enum CubeMove {
    MOVE_R,
    MOVE_RP,
    MOVE_L,
    MOVE_LP,
    MOVE_F,
    MOVE_FP,
    MOVE_B,
    MOVE_BP,
    MOVE_U,
    MOVE_UP,
    MOVE_D,
    MOVE_DP
};

enum CubeOrientation {
    ORIENT_NORMAL,   // Original orientation
    ORIENT_UP_FRONT,    // Original UP face is now in the FRONT servo
    ORIENT_UP_RIGHT,    // Original UP face is now in the RIGHT servo
    ORIENT_UP_BACK,     // Original UP face is now in the BACK servo
    ORIENT_UP_LEFT      // Original UP face is now in the LEFT servo
    // Please do not make UP face DOWN, it is not necessary and only complicates things
};

class CubeController {
public:
    CubeController(Servo* servos) : servos(servos), numServos(NUM_SERVOS)
    {
        // Empty body
    }

    void executeMove(CubeMove move);
    void tick();

private:
    Servo* servos;
    int numServos;
};

extern CubeOrientation currentOrientation;
extern CubeController cubeController;
