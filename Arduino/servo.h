#pragma once
#include <Arduino.h>
#include "Config.h"
#include "Types.h"

// ToString for servo types
void servoTypeToString(ServoType type, char* buffer, size_t bufferSize);

class Servo
{
private:
    int pin{};
    ServoType type{};
    ServoCal cal{};
    ServoState state{STATE_C};
    int pulse{};    // Current pulse width in microseconds

public:

    // Constructor
    Servo(int pin, ServoType type, ServoCal calibration) :
        pin(pin),
        type(type),
        cal(calibration)
    {
        pulse = calibration.C_us;   // Start at center position
    }

    // State control
    void setState(ServoState next)
    {
        switch (next)
        {
        case STATE_L:
            pulse = cal.L_us; break;
        case STATE_R:
            pulse = cal.R_us; break;
        case STATE_C:
            if (type % 2 == 0) // Slider
            {
                pulse = cal.C_us;
                break;
            }  

            if (state == STATE_L)
                pulse = cal.C_us + cal.CD_us;
            else if (state == STATE_R)
                pulse = cal.C_us - cal.CD_us;
            else
                pulse = cal.C_us;

            break;
        }
        state = next;
        sendPulse();
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

    void adjustCalibration(int delta)
    {
        if (state == STATE_C)
        {
            cal.C_us += delta;
        }
        else if (state == STATE_L)
        {
            cal.L_us += delta;
        }
        else if (state == STATE_R)
        {
            cal.R_us += delta;
        }
        pulse += delta;
    }

    void adjustDeviation(int delta)
    {
        if (type % 2 == 1) // Spinner
        {
            cal.CD_us += delta;
            if (state == STATE_C)
            {
                pulse = cal.C_us + ((state == STATE_L) ? cal.CD_us : -cal.CD_us);
            }
        }
    }

    void sendPulse() const
    {
        digitalWrite(pin, HIGH);
        delayMicroseconds(pulse);
        digitalWrite(pin, LOW);
    }
};

#define NUM_SERVOS 8
// If you change these, it will read wrong calibration values
// Just don't change them
#define RIGHT_SPINNER_INDEX 0
#define RIGHT_SLIDER_INDEX 1
#define LEFT_SPINNER_INDEX 2
#define LEFT_SLIDER_INDEX 3
#define FRONT_SPINNER_INDEX 4
#define FRONT_SLIDER_INDEX 5
#define BACK_SPINNER_INDEX 6
#define BACK_SLIDER_INDEX 7

extern Servo servos[NUM_SERVOS];

void sendPWMAll(Servo servos[], int numServos);
