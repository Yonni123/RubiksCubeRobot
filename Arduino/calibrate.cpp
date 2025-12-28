#include <Arduino.h>
#include "Servo.h"
#include "Calibrate.h"

// Select one servo and calibrate one at a time
static int selectedServo = 0;
static ServoState mode = STATE_C;
static const int step = 5;

// Print status of selected servo
void printStatus()
{
    Serial.print("Selected Servo: ");
    Serial.print(selectedServo);
    Serial.print(" (");
    Serial.print(servos[selectedServo].getType() == SLIDER ? "SLIDER" : "SPINNER");
    Serial.print(")");
    Serial.print(" | Mode: ");
    switch (mode)
    {
    case STATE_C:
        Serial.print("CENTER");
        Serial.print(" | Center us: ");
        Serial.print(servos[selectedServo].C_us);
        Serial.print(" | Deviation: ");
        Serial.print(servos[selectedServo].CD_us);
        Serial.println(" us");
        Serial.print(" | Pulse: ");
        Serial.print(servos[selectedServo].pulseWidth());
        break;
    case STATE_L:
        Serial.print("LEFT");
        Serial.print(" | Pulse: ");
        Serial.print(servos[selectedServo].pulseWidth());
        Serial.println(" us");
        break;
    case STATE_R:
        Serial.print("RIGHT");
        Serial.print(" | Pulse: ");
        Serial.print(servos[selectedServo].pulseWidth());
        Serial.println(" us");
        break;
    }
}

// Print results of calibration for all servos
void printCalibration()
{
    Serial.println("Current Calibration Values:");
    Serial.println("Servo servos[NUM_SERVOS] = {");
    for (int i = 0; i < NUM_SERVOS; i++)
    {
        Serial.print("    Servo(");
        Serial.print(servos[i].getPin());
        Serial.print(", ");
        Serial.print(servos[i].getType() == SLIDER ? "SLIDER" : "SPINNER");
        Serial.print(", ");
        Serial.print("FACE_");;
        switch (servos[i].face)
        {
        case FACE_R:
            Serial.print("R, ");
            break;
        case FACE_L:
            Serial.print("L, ");
            break;
        case FACE_F:
            Serial.print("F, ");
            break;
        case FACE_B:
            Serial.print("B, ");
            break;
        }
        Serial.print(servos[i].L_us);
        Serial.print(", ");
        Serial.print(servos[i].R_us);
        Serial.print(", ");
        Serial.print(servos[i].C_us);
        Serial.print(", ");
        Serial.print(servos[i].CD_us);
        if (i < NUM_SERVOS - 1)
            Serial.println("),");
        else
            Serial.println(")");
    }
    Serial.println("};");
}

void calibrateSetup()
{
    Serial.begin(9600);

    Serial.println("Servo calibration tool");
    Serial.println("0-7 : select servo");
    Serial.println("c   : set state CENTER to calibrate center");
    Serial.println("l   : set state LEFT to calibrate left");
    Serial.println("r   : set state RIGHT to calibrate right");
    Serial.println("+/- : increase/decrease step size");
    Serial.println("*// : increase/decrease step size * 10");
    Serial.println("</> : increase/decrease center deviation (spinners only)");
    Serial.println("p   : print calibration");
    Serial.println();

    for (int i = 0; i < NUM_SERVOS; i++)
    {
        pinMode(servos[i].getPin(), OUTPUT);
        servos[i].setState(STATE_C);
    }

    printStatus();
}

void calibrateLoop()
{
    if (!Serial.available())
        return;

    char c = Serial.read();

    if (c >= '0' && c <= '7')
    {
        selectedServo = c - '0';
        printStatus();
    }
    else if (c == 'c')
    {
        mode = STATE_C;
        servos[selectedServo].setState(STATE_C);
        printStatus();
    }
    else if (c == 'l')
    {
        mode = STATE_L;
        servos[selectedServo].setState(STATE_L);
        printStatus();
    }
    else if (c == 'r')
    {
        mode = STATE_R;
        servos[selectedServo].setState(STATE_R);
        printStatus();
    }
    else if (c == '+')
    {
        servos[selectedServo].adjustCalibration(step);
        printStatus();
    }
    else if (c == '-')
    {
        servos[selectedServo].adjustCalibration(-step);
        printStatus();
    }
    else if (c == '*')
    {
        servos[selectedServo].adjustCalibration(step * 10);
        printStatus();
    }
    else if (c == '/')
    {
        servos[selectedServo].adjustCalibration(-step * 10);
        printStatus();
    }
    else if (c == '>')
    {
        servos[selectedServo].adjustDeviation(step);
        printStatus();
    }
    else if (c == '<')
    {
        servos[selectedServo].adjustDeviation(-step);
        printStatus();
    }
    else if (c == 'p')
    {
        printCalibration();
    }
}
