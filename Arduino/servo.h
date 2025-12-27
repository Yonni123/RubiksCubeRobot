#pragma once

const int NUM_SERVOS = 8;

enum ServoState {
    STATE_C,
    STATE_L,
    STATE_R
};

enum ServoType {
    SPINNER,
    SLIDER
};

class Servo {
private:
    int pin{};
    ServoType type{};
    ServoState state{};
    int pulse{};

    // PWM calibration (microseconds)
    int L_us{};
    int R_us{};
    int CL_us{};
    int CR_us{};

public:
    // Constructor
    Servo(int pin, ServoType type, int L_us, int R_us, int CL_us, int CR_us):
      pin(pin), 
      type(type), 
      state(STATE_C), // default starting state
      pulse((CL_us+CR_us)/2), // initial PWM
      L_us(L_us), 
      R_us(R_us), 
      CL_us(CL_us), 
      CR_us(CR_us) {}

    // State control
    void setState(ServoState next){
      switch(next) {
        case STATE_L: pulse = L_us; break;
        case STATE_R: pulse = R_us; break;
        case STATE_C:
          if (state == STATE_L) pulse = CL_us;
          else if (state == STATE_R) pulse = CR_us;
          break;
      }
      state = next;
    }

    int pulseWidth() const {
        return pulse;
    }

    void sendPulse() const {
        digitalWrite(pin, HIGH);
        delayMicroseconds(pulse);
        digitalWrite(pin, LOW);
    }
};

extern Servo servos[NUM_SERVOS];

void sendPWMAll(Servo servos[], int numServos);
