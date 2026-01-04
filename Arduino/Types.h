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

// MOVE rr0_FL0_BL0_RR0_LR0_fL250_bR0_FC0_BC0                           SCAN RIGHT SIDE
// MOVE rr0_FL0_BL0_RR0_LR0_fC250_bC0_FC250_BC0_FC0_BC0_RC0_LC0_rC250   RECOVER
// MOVE lr0_FL0_BL0_RR0_LR0_fR250_bL0_FC0_BC0                           SCAN RIGHT SIDE
// MOVE lr0_FL0_BL0_RR0_LR0_fC250_bC0_FC250_BC0_FC0_BC0_RC0_LC0_lC250   RECOVER

// MOVE br0_RL0_LL0_BR0_FR0_rL250_lR0_RC0_LC0                           SCAN BACK SIDE
// MOVE br0_RL0_LL0_BR0_FR0_rC250_lC0_RC250_LC0_RC0_LC0_BC0_FC0_bC250   RECOVER
// MOVE fr0_RL0_LL0_BR0_FR0_rR250_lL0_RC0_LC0                           SCAN FRONT SIDE
// MOVE fr0_RL0_LL0_BR0_FR0_rC250_lC0_RC250_LC0_RC0_LC0_BC0_FC0_fC250   RECOVER

// MOVE 5L0_BL0_RR0_LR0_fL250_bR0_ ROTATE CUBE

