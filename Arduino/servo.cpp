#include "Servo.h"

// Calibrations: L_us, R_us, CL_us, CR_us
Servo servos[NUM_SERVOS] = {
    Servo(2, SLIDER, 1000,2000,1500,1600),
    Servo(3, SPINNER, 1000,2000,1500,1600),
    Servo(4, SLIDER, 1000,2000,1500,1600),
    Servo(5, SPINNER, 1000,2000,1500,1600),
    Servo(6, SLIDER, 1000,2000,1500,1600),
    Servo(7, SPINNER, 1000,2000,1500,1600),
    Servo(8, SLIDER, 1000,2000,1500,1600),
    Servo(9, SPINNER, 1000,2000,1500,1600)
};

void sendPWMAll(Servo servos[], int numServos) {
    unsigned long frameStart = micros();

    for (int i = 0; i < numServos; i++) {
        servos[i].sendPulse();
    }

    // Finish 20 ms servo frame
    unsigned long frameTime = micros() - frameStart;
    if (frameTime < 20000) {
        delayMicroseconds(20000 - frameTime);
    }
}
