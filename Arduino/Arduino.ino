#include "Calibrate.h"
#include "Servo.h"
#include "SequenceManager.h"
#include "API.h"

unsigned long pwmHoldUntil = 0;
const unsigned long PWM_HOLD_MS = 1000;  // hold PWM for 2 seconds after idle

void setup() {
    Serial.begin(9600);
    calibrateSetup();   // Initialize EEPROM where calibrations are stored
#if !CALIBRATE
    APISetup();        // Setup API if not in calibration mode
    pwmHoldUntil = millis() + PWM_HOLD_MS;
#endif
}

void loop() {
    int res = seqManager.tick();
    if (res < 0)
    {
        Serial.print("SEQ ERR ");
        Serial.println(res);
    }

    // If busy, update the PWM hold timer
    if (seqManager.isBusy())
    {
        sendPWMAll();
        pwmHoldUntil = millis() + PWM_HOLD_MS;
    }
    else
    {
        // Sequence is idle: keep sending PWM until hold timer expires
        if (millis() < pwmHoldUntil)
        {
            sendPWMAll();
        }
    }

#if CALIBRATE
    calibrateLoop();
#else
    APILoop();  // Listen for API commands
#endif
}
