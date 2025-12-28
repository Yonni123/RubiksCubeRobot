#include <Arduino.h>
#include "Servo.h"
#include "CubeController.h"


Servo servos[NUM_SERVOS] = {    // If below values are wrong, use calibrate.cpp to calibrate
    Servo(6, SLIDER, FACE_R, 2450, 1250, 2340, 0),
    Servo(7, SPINNER, FACE_R, 520, 2550, 1550, 45),
    Servo(4, SLIDER, FACE_L, 600, 1900, 800, 0),
    Servo(5, SPINNER, FACE_L, 500, 2500, 1500, 80),
    Servo(8, SLIDER, FACE_F, 500, 1700, 650, 0),
    Servo(9, SPINNER, FACE_F, 400, 2410, 1400, 55),
    Servo(2, SLIDER, FACE_B, 600, 1800, 800, 0),
    Servo(3, SPINNER, FACE_B, 350, 2410, 1405, 60)
};

// Call this function in the main loop to send PWM signals to all servos to keep them in position
void sendPWMAll(Servo servos[], int numServos)
{
    unsigned long frameStart = micros();

    for (int i = 0; i < numServos; i++)
    {
        servos[i].sendPulse();
    }

    // Finish 20 ms servo frame
    unsigned long frameTime = micros() - frameStart;
    if (frameTime < 20000)
    {
        delayMicroseconds(20000 - frameTime);
    }
}
