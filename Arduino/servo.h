#pragma once

const int NUM_SERVOS = 8;

enum ServoState
{
    STATE_C,
    STATE_L,
    STATE_R
};

enum ServoType
{
    SPINNER,
    SLIDER
};

class Servo
{
private:
    int pin{};
    ServoType type{};
    ServoState state{};
    int pulse{};

public:
    // PWM calibration (microseconds)
    int L_us{};
    int R_us{};
    int C_us{};
    int CD_us{};    // Deviation, plus minus..

    // Constructor
    Servo(int pin, ServoType type, int L_us, int R_us, int C_us, int CD_us = 0) :
        pin(pin),
        type(type),
        state(STATE_C),     // default starting state
        pulse(C_us),        // initial PWM
        L_us(L_us),
        R_us(R_us),
        C_us(C_us),
        CD_us(CD_us)
    {
        // Empty body
    }

    // State control
    void setState(ServoState next)
    {
        switch (next)
        {
        case STATE_L:
            pulse = L_us; break;
        case STATE_R:
            pulse = R_us; break;
        case STATE_C:
            if (type == SLIDER)
            {
                pulse = C_us;
                break;
            }  

            if (state == STATE_L)
                pulse = C_us + CD_us;
            else if (state == STATE_R)
                pulse = C_us - CD_us;
            else
                pulse = C_us;

            break;
        }
        state = next;
    }

    int pulseWidth() const
    {
        return pulse;
    }

    int getPin() const
    {
        return pin;
    }

    int getType() const
    {
        return type;
    }

    void adjustCalibration(int delta)
    {
        if (state == STATE_C)
        {
            C_us += delta;
        }
        else if (state == STATE_L)
        {
            L_us += delta;
        }
        else if (state == STATE_R)
        {
            R_us += delta;
        }
        pulse += delta;
    }

    void adjustDeviation(int delta)
    {
        if (type == SPINNER)
        {
            CD_us += delta;
            if (state == STATE_C)
            {
                pulse = C_us + (pulse > -C_us ? CD_us : CD_us);
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

extern Servo servos[NUM_SERVOS];

void sendPWMAll(Servo servos[], int numServos);
