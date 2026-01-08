#pragma once
#include <Arduino.h>
#include "Config.h"
#include "Types.h"
#include <Servo.h>

// ToString for servo types
void servoTypeToString(ServoType type, char* buffer, size_t bufferSize);

// Convert character to ServoType (uppercase is slider, lowercase is spinner)
bool parseServoType(char c, ServoType& type);

class MyServo
{
private:
    int pin{};
    Servo thisServo{};
    ServoType type{};
    ServoCal cal{};
    ServoState state{STATE_C};
    int pulse{};    // Current pulse width in microseconds
    bool attached{false};

public:

    // Constructor
    MyServo(int pin, ServoType type, ServoCal calibration);

    // State control
    void setState(ServoState next);

    ServoState getState() const
    {
        return state;
    }

    int pulseWidth() const
    {
        return pulse;
    }

    int getPin() const
    {
        return pin;
    }

    ServoType getType() const
    {
        return type;
    }

    ServoCal getCalibration() const
    {
        return cal;
    }

    void adjustCalibration(int delta);

    void adjustDeviation(int delta);

    void attach();

    void detach();
};

#define NUM_SERVOS 8

extern MyServo servos[NUM_SERVOS];

void attachAllServos();

void detachAllServos();
