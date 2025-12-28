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
    ORIENT_INVERT,    // Original UP face is now in the FRONT servo
    // More orientations can be added, but they would make things complicated
};

class CubeController {
public:
    CubeController(Servo* servos) : servos(servos), numServos(NUM_SERVOS)
    {
        // Empty body
    }

    void openAllSliders();
    void closeAllSliders();
    int rotateCube(CubeOrientation newOrientation);
    int executeMove(CubeMove move, unsigned long delayOffset = 0);
    int executeMoves(const CubeMove moves[], int count);
    void tick();

private:
    Servo* servos;
    int numServos;
};

extern CubeOrientation currentOrientation;
extern CubeController cubeController;
