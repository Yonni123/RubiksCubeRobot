#pragma once
#include "Config.h"
#include "Types.h"


struct SequenceMove
{
    ServoType servo;    // Which servo to move
    ServoState state;   // Target state (R, C, L)
    unsigned long timeMs; // When to perform the move   (relative to program start (millis()))
};

class SequenceManager
{
public:
    SequenceManager() = default;

    // Move string should be "2C0 4R500 2L1000"
    // meaning: move servo 2 to C at time 0ms (now),
    // move servo 4 to R at time 500ms (from now),
    // move servo 2 to L at time 1000ms (from now)
    // The servo numbers are defined in the ServoType enum in Types.h
    // Retuns 0 on success, -1 if busy
    int startSequence(const char* moveString);

    // Call this in the main loop
    int tick();

    // Query if sequence is still running
    bool isBusy() const { return busy; }

private:
    char activeSequence[64];
    int sequenceIndex = 0;
    unsigned long nextMoveAt = 0;
    bool busy = false;

    int scheduleNextMove();
};

extern SequenceManager seqManager;
