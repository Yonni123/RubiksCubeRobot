#include <Arduino.h>
#include "servo.h"

// Calibrations: L_us, R_us, C_us

Servo servos[NUM_SERVOS] = {
    Servo(2, SLIDER, 600, 1800, 800, 0),
    Servo(3, SPINNER, 350, 2410, 1405, 60),
    Servo(4, SLIDER, 600, 1900, 800, 0),
    Servo(5, SPINNER, 500, 2500, 1500, 80),
    Servo(6, SLIDER, 2450, 1250, 2340, 0),
    Servo(7, SPINNER, 520, 2550, 1550, 45),
    Servo(8, SLIDER, 500, 1700, 650, 0),
    Servo(9, SPINNER, 400, 2410, 1400, 55)
};

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
