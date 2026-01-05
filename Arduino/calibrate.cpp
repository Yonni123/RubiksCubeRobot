#include <Arduino.h>
#include <EEPROM.h>
#include "Servo.h"
#include "Calibrate.h"
#include "Config.h"

#define EEPROM_MAGIC 0xCAFE
#define EEPROM_MAGIC_ADDR 100
#define EEPROM_CALIBRATION_START_ADDR 102 // After magic number

// Select one servo and calibrate one at a time
static int selectedServo = 0;
static ServoState mode = STATE_C;
static const int step = 5;

void initEEPROM()
{
    uint16_t magic;
    EEPROM.get(EEPROM_MAGIC_ADDR, magic);

    if (magic != EEPROM_MAGIC)
    {
        for (int i = 0; i < NUM_SERVOS; i++)
        {
            ServoCal initCal = {1000, 1500, 2000};
            int servoPin = servos[i].getPin();
            EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(ServoCal), initCal);
        }

        EEPROM.put(EEPROM_MAGIC_ADDR, EEPROM_MAGIC);
    }
}

ServoCal readCalibration(ServoType type)
{
    ServoCal cal;
    // Type is just an enum from 0 to NUM_SERVOS-1, so we can use it directly as index
    EEPROM.get(EEPROM_CALIBRATION_START_ADDR + type * sizeof(ServoCal), cal);
    return cal;
}

void writeCalibration(ServoType type, ServoCal cal)
{
    Serial.print("Writing calibration for servo type: ");
    char buffer[20];
    servoTypeToString(type, buffer, sizeof(buffer));
    Serial.print(buffer);
    Serial.print("\n");
    EEPROM.put(EEPROM_CALIBRATION_START_ADDR + type * sizeof(ServoCal), cal);
}

void writeAllCalibrations()
{
    Serial.println("Writing all calibrations to EEPROM...");
    for (int i = 0; i < NUM_SERVOS; i++)
    {
        ServoCal cal = servos[i].getCalibration();
        writeCalibration(i, cal);
    }
    Serial.println("Done writing calibrations, please set #define CALIBRATE false and re-upload the sketch.");
}

// Print status of selected servo
void printStatus()
{
    Serial.print("Selected Servo: ");
    Serial.print(selectedServo);
    Serial.print(" (");
    char buffer[20];
    servoTypeToString(servos[selectedServo].getType(), buffer, sizeof(buffer));
    Serial.print(buffer);
    Serial.print(")");
    Serial.print(" | Mode: ");
    ServoCal cal = servos[selectedServo].getCalibration();
    switch (mode)
    {
    case STATE_C:
        Serial.print("CENTER");
        Serial.print(" | Center us: ");
        Serial.print(cal.C_us);
        Serial.print(" | Deviation: ");
        Serial.print(cal.CD_us);
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

void printCalibrations()
{
    char buffer[20];

    Serial.println();
    Serial.println(F("Idx Pin Type     L(us) R(us) C(us) Dev(us)"));
    Serial.println(F("--- --- -------- ----- ----- ----- -------"));

    for (int i = 0; i < NUM_SERVOS; i++)
    {
        ServoCal cal = servos[i].getCalibration();

        Serial.print(i);
        Serial.print(F("   "));
        Serial.print(servos[i].getPin());
        Serial.print(F("   "));

        servoTypeToString(servos[i].getType(), buffer, sizeof(buffer));
        Serial.print(buffer);
        Serial.print(F(" "));

        Serial.print(cal.L_us);
        Serial.print(F("  "));
        Serial.print(cal.R_us);
        Serial.print(F("  "));
        Serial.print(cal.C_us);
        Serial.print(F("  "));
        Serial.println(cal.CD_us);
    }

    Serial.println();
}


void printHelp()
{
    Serial.println("Servo calibration tool commands:");
    Serial.println("0-7 : select servo");
    Serial.println("c   : set state CENTER to calibrate center");
    Serial.println("l   : set state LEFT to calibrate left");
    Serial.println("r   : set state RIGHT to calibrate right");
    Serial.println("+/- : increase/decrease step size");
    Serial.println("*// : increase/decrease step size * 10");
    Serial.println("</> : increase/decrease center deviation (spinners only)");
    Serial.println("p   : Print current calibration values");
    Serial.println("w   : Write calibration to EEPROM");
    Serial.println();
}

void calibrateSetup()
{
    initEEPROM();

#if CALIBRATE // Only setup if in calibration mode, otherwise this is not your concern
    for (int i = 0; i < NUM_SERVOS; i++)
    {
        pinMode(servos[i].getPin(), OUTPUT);
        servos[i].setState(STATE_C);
    }
    printStatus();
#endif
}

void calibrateLoop()
{
    sendPWMAll();   // Since sequence manager is always idle, we need to keep sending PWM during calibration

    if (!Serial.available())
        return;

    char c = Serial.read();

    if (c == 'h')
    {
        printHelp();
    }
    else if (c >= '0' && c <= '7')
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
        printCalibrations();
    }
    else if (c == 'w')
    {
        writeAllCalibrations();
    }
}
