#include "Calibrate.h"
#include "Servo.h"
#include "SequenceManager.h"
#include "API.h"

void setup() {
    Serial.begin(9600);
    calibrateSetup();   // Initialize EEPROM where calibrations are stored
#if !CALIBRATE
    APISetup();        // Setup API if not in calibration mode
#endif
}

void loop() {
  sendPWMAll();
  int res = seqManager.tick();
  if (res < 0)
    {
        Serial.print("SEQ ERR ");
        Serial.println(res);
    }
#if CALIBRATE
    calibrateLoop();
#else
    APILoop();  // Listen for API commands
#endif
}
