#pragma once
#include "Arduino.h"
#include "CubeController.h"
#include "Config.h"

class CubeSequenceManager
{
public:
    CubeSequenceManager(CubeController& controller);

    // Start a new move string sequence (Singmaster notation)
    void startSequence(const char* moveString);

    // Call this in the main loop
    void tick();

    // Query if sequence is still running
    bool isActive() const { return sequenceActive; }

private:
    CubeController& cube;
    const char* activeSequence = nullptr;
    int sequenceIndex = 0;
    unsigned long nextMoveAt = 0;
    bool sequenceActive = false;

    void scheduleNextMove();
};

extern CubeSequenceManager seqManager;
