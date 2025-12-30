#include <Arduino.h>
#include "Servo.h"
#include "CubeController.h"
#include "Config.h"
#include "Calibrate.h"


Servo servos[NUM_SERVOS] =
{
    [RIGHT_SPINNER_INDEX] = Servo(RIGHT_SPINNER_PIN, RIGHT_SPINNER, readCalibration(RIGHT_SPINNER_INDEX)),
    [RIGHT_SLIDER_INDEX]  = Servo(RIGHT_SLIDER_PIN,  RIGHT_SLIDER,  readCalibration(RIGHT_SLIDER_INDEX)),
    [LEFT_SPINNER_INDEX]  = Servo(LEFT_SPINNER_PIN,  LEFT_SPINNER,  readCalibration(LEFT_SPINNER_INDEX)),
    [LEFT_SLIDER_INDEX]   = Servo(LEFT_SLIDER_PIN,   LEFT_SLIDER,   readCalibration(LEFT_SLIDER_INDEX)),
    [FRONT_SPINNER_INDEX] = Servo(FRONT_SPINNER_PIN, FRONT_SPINNER, readCalibration(FRONT_SPINNER_INDEX)),
    [FRONT_SLIDER_INDEX]  = Servo(FRONT_SLIDER_PIN,  FRONT_SLIDER,  readCalibration(FRONT_SLIDER_INDEX)),
    [BACK_SPINNER_INDEX]  = Servo(BACK_SPINNER_PIN,  BACK_SPINNER,  readCalibration(BACK_SPINNER_INDEX)),
    [BACK_SLIDER_INDEX]   = Servo(BACK_SLIDER_PIN,   BACK_SLIDER,   readCalibration(BACK_SLIDER_INDEX))
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

void servoTypeToString(ServoType type, char* buffer, size_t bufferSize)
{
    switch (type)
    {
    case FRONT_SLIDER:
        strncpy(buffer, "FRONT_SLIDER", bufferSize); break;
    case FRONT_SPINNER:
        strncpy(buffer, "FRONT_SPINNER", bufferSize); break;
    case RIGHT_SLIDER:
        strncpy(buffer, "RIGHT_SLIDER", bufferSize); break;
    case RIGHT_SPINNER:
        strncpy(buffer, "RIGHT_SPINNER", bufferSize); break;
    case BACK_SLIDER:
        strncpy(buffer, "BACK_SLIDER", bufferSize); break;
    case BACK_SPINNER:
        strncpy(buffer, "BACK_SPINNER", bufferSize); break;
    case LEFT_SLIDER:
        strncpy(buffer, "LEFT_SLIDER", bufferSize); break;
    case LEFT_SPINNER:
        strncpy(buffer, "LEFT_SPINNER", bufferSize); break;
    default:
        strncpy(buffer, "UNKNOWN", bufferSize); break;
    }
}
