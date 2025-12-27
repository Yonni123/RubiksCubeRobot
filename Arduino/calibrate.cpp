#include <Arduino.h>
#include "servo.h"
#include "calibrate.h"

static int selectedServo = 0;
static ServoState mode = STATE_C;
static const int step = 5;

void printStatus();
void printCalibration();

void calibrateSetup() {
    Serial.begin(9600);

    Serial.println("Servo calibration tool");
    Serial.println("0-7 : select servo");
    Serial.println("c   : CENTER");
    Serial.println("l   : LEFT");
    Serial.println("r   : RIGHT");
    Serial.println("+/- : adjust value");
    Serial.println("p   : print calibration");
    Serial.println();

    for (int i = 0; i < NUM_SERVOS; i++) {
        pinMode(servos[i].getPin(), OUTPUT);
        servos[i].setState(STATE_C);
    }

    printStatus();
}

void calibrateLoop() {
    sendPWMAll(servos, NUM_SERVOS);

    if (!Serial.available()) return;

    char c = Serial.read();

    if (c >= '0' && c <= '7') {
        selectedServo = c - '0';
        printStatus();
    }
    else if (c == 'c') {
        mode = STATE_C;
        servos[selectedServo].setState(STATE_C);
        printStatus();
    }
    else if (c == 'l') {
        mode = STATE_L;
        servos[selectedServo].setState(STATE_L);
        printStatus();
    }
    else if (c == 'r') {
        mode = STATE_R;
        servos[selectedServo].setState(STATE_R);
        printStatus();
    }
    else if (c == '+') {
        servos[selectedServo].adjustCalibration(mode, step);
        printStatus();
    }
    else if (c == '-') {
        servos[selectedServo].adjustCalibration(mode, -step);
        printStatus();
    }
    else if (c == 'p') {
        printCalibration();
    }
}
