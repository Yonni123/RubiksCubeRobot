#include "SequenceManager.h"
#include "Servo.h"
#include <ctype.h> 
#include <stdlib.h>
#include <Arduino.h>

SequenceManager seqManager;

// Convert character to ServoState
static bool parseServoState(char c, ServoState& state)
{
    switch (c)
    {
        case 'L': state = STATE_L; return true;
        case 'C': state = STATE_C; return true;
        case 'R': state = STATE_R; return true;
        case 'r': state = STATE_r; return true;
        case 'l': state = STATE_l; return true;
        default: return false;
    }
}

// Start a new sequence
// Returns:
//  0  = OK
// -1  = busy
// -2  = format error
int SequenceManager::startSequence(const char* moveString)
{
    if (!moveString || *moveString == '\0')
        return -2;

    // Cancel command
    if (moveString[0] == 'C' && moveString[1] == '\0')
    {
        busy = false;
        activeSequence[0] = '\0';
        notifyState();
        return 0;
    }

    if (busy)
        return -1;

    strncpy(activeSequence, moveString, sizeof(activeSequence) - 1);
    activeSequence[sizeof(activeSequence) - 1] = '\0';

    sequenceIndex = 0;
    nextMoveAt = millis();
    busy = true;
    notifyState();

    return 0;
}

// Called repeatedly from loop()
int SequenceManager::tick()
{
    if (!busy)
        return 0;

    unsigned long now = millis();
    if (now < nextMoveAt)
        return 0;

    return executeUntilDelay();
}

void SequenceManager::notifyState()
{
    if (busy)
        Serial.println("BUSY");
    else
        Serial.println("IDLE");
}

int SequenceManager::executeUntilDelay()
{
    // Execute until we hit a delay or end of sequence
    while (activeSequence[sequenceIndex] != '\0' &&
       !isdigit(activeSequence[sequenceIndex]))
    {
        Serial.print("Sequence char: ");
        Serial.println(activeSequence[sequenceIndex]);
        ServoType servoType;
        if (!parseServoType(activeSequence[sequenceIndex], servoType))
        {
            busy = false;
            activeSequence[0] = '\0';
            notifyState();
            return -3;  // servo type error
        }
        sequenceIndex++;

        ServoState state;
        if (!parseServoState(activeSequence[sequenceIndex], state))
        {
            busy = false;
            activeSequence[0] = '\0';
            notifyState();
            return -4;  // state error
        }
        sequenceIndex++;

        // Execute move immediately
        servos[servoType].setState(state);
        servos[servoType].sendPulse();
    }

    // ---- End of sequence ----
    if (activeSequence[sequenceIndex] == '\0')
    {
        busy = false;
        activeSequence[0] = '\0';
        notifyState();
        return 0;
    }

    // ---- Delay encountered ----
    if (isdigit(activeSequence[sequenceIndex]))
    {
        char* endPtr;
        unsigned long delay =
            strtoul(&activeSequence[sequenceIndex], &endPtr, 10);

        sequenceIndex = endPtr - activeSequence;
        nextMoveAt = millis() + delay;
        return 0;
    }

    return 1;   // Current move executed, more to schedule
}
