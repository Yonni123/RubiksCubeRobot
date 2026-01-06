#pragma once

enum ServoState
{
    STATE_C,
    STATE_L,
    STATE_R,
    STATE_r,    // 45 degree between CENTER and RIGHT
    STATE_l   // 45 degree between CENTER and LEFT
};

enum ServoType
{   // If a servo type is odd, it's a slider; if even, it's a spinner
    RIGHT_SPINNER,
    RIGHT_SLIDER,
    LEFT_SPINNER,
    LEFT_SLIDER,
    FRONT_SPINNER,
    FRONT_SLIDER,
    BACK_SPINNER,
    BACK_SLIDER
};

struct ServoCal {
    unsigned int L_us;
    unsigned int R_us;
    unsigned int C_us;
    unsigned int CD_us; // Deviation for center position for spinners
};
