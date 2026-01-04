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

struct MoveToExecute
{
    ServoType servoType;
    ServoState state;
    bool alreadyExecuted = false;
};
MoveToExecute pendingMove;

// Called repeatedly from loop()
int SequenceManager::tick()
{
    if (!busy)
        return 0;

    unsigned long now = millis();
    if (now < nextMoveAt)
        return 0;

    if (!pendingMove.alreadyExecuted)
    {
        servos[pendingMove.servoType].setState(pendingMove.state);
        pendingMove.alreadyExecuted = true;
    }

    return scheduleNextMove();
}

void SequenceManager::notifyState()
{
    if (busy)
        Serial.println("BUSY");
    else
        Serial.println("IDLE");
}

// Parse and schedule the next move
int SequenceManager::scheduleNextMove()
{
    if (!activeSequence)
    {
        busy = false;
        notifyState();
        return 0;
    }

    // Skip spaces and separators
    while (activeSequence[sequenceIndex] == ' ' || activeSequence[sequenceIndex] == '_')
        sequenceIndex++;

    // End of sequence
    if (activeSequence[sequenceIndex] == '\0')
    {
        busy = false;
        activeSequence[0] = '\0';
        notifyState();
        return 0;
    }

    // ---- Parse servo type ----
    ServoType servoType;
    if (!parseServoType(activeSequence[sequenceIndex], servoType))
    {
        busy = false;
        activeSequence[0] = '\0';
        notifyState();
        return -3;  // servo type error
    }
    sequenceIndex++;

    // ---- Parse servo state ----
    ServoState state;
    if (!parseServoState(activeSequence[sequenceIndex], state))
    {
        busy = false;
        activeSequence[0] = '\0';
        notifyState();
        return -4;  // state error
    }
    sequenceIndex++;

    // ---- Parse delay ----
    char* endPtr;
    if (!isdigit(activeSequence[sequenceIndex]))
    {
        busy = false;
        activeSequence[0] = '\0';
        notifyState();
        return -5;  // delay error
    }

    unsigned long delayMs =
        strtoul(&activeSequence[sequenceIndex], &endPtr, 10);

    sequenceIndex = endPtr - activeSequence;

    // ---- Make the move pending ----
    pendingMove.servoType = servoType;
    pendingMove.state = state;
    pendingMove.alreadyExecuted = false;

    // ---- Schedule next move ----
    nextMoveAt = millis() + delayMs;

    return 0;
}
