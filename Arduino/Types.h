#pragma once

enum ServoState
{
    STATE_C,
    STATE_L,
    STATE_R
};

enum ServoType
{   // If a servo type is odd, it's a spinner; if even, it's a slider
    FRONT_SLIDER,
    FRONT_SPINNER,
    RIGHT_SLIDER,
    RIGHT_SPINNER,
    BACK_SLIDER,
    BACK_SPINNER,
    LEFT_SLIDER,
    LEFT_SPINNER
};

struct ServoCal {
    unsigned int L_us;
    unsigned int R_us;
    unsigned int C_us;
    unsigned int CD_us; // Deviation for center position for spinners
};
