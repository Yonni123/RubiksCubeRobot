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

    // Buffer to hold the active sequence, look at start sequence function for format
    // Please fill this buffer before calling startSequence() and null-terminate it.
    char activeSequence[SEQUENCE_BUFFER_SIZE];

    // Buffer to hold individual move for MOVE command
    char moveBuf[MOVE_BUFFER_SIZE];

    // Move string should be "rRBL250fr"
    // meaning: right spinner to R, back slider to L, wait 250ms, front spinner to r
    // Moves are a pair of two chars, first refers to the servo, second to the state. numbers refer to delays in milliseconds.
    // The servo numbers are defined in the ServoType enum in Types.h and in this function, referenced like this:
    //     "F" "B" "L" "R" : Front, Back, Left, Right Sliders
    //     "f" "b" "l" "r" : Front, Back, Left, Right Spinners
    // States are defined in the ServoState enum in Types.h:
    //     "C" : Center
    //     "L" : Left
    //     "R" : Right
    //     "r" : Center-right (between C and R)
    //     "l" : Center-left (between C and L)
    // A single move to a spinner will make it go further to compenstate for gripper gap between cube and gripper.
    // Sometimes we don't want that, so just call the move again, to make it go to the true state.
    // Example: "rC" will move right spinner to center, then go a little further to make the side itself centered.
    // But if we want the gripper itself to be centered, we call "rCrC".
    int startSequence(const char* moveString);

    // Execute moves on the cube.
    // Each character in the string is a move.
    // "U" : Up face clockwise
    // "u" : Up face counter-clockwise
    // And so on for other faces: D, L, R, F, B (lowercase for counter-clockwise)
    // Don't input U' or U2 or anything similar, not even spaces, just parse your moves before calling this function.
    // The delay is how long to wait for servos to reach their position before executing the next move.
    int startMoves(const char* moveString, int delayMs);

    // Call this in the main loop
    int tick();

    // Query if sequence is still running
    bool isBusy() const { return busy != 0; }

    unsigned long idleTimeMs;  // Time since last sequence completed

private:
    int busy = 0;   // 0 = idle, 1 = busy with SEQ, 2 = busy with MOVE
    void notifyState();

    // Sequence handling stuff
    int sequenceIndex = 0;
    unsigned long nextMoveAt = 0;
    int executeUntilDelay();
    int handleSequence();
    
    // MOVE handling stuff
    int movesDelayMs;   // This is for MOVE command only
    char seqBuf[128]; // Used by MOVE command only to build individual moves
    int moveIndex = 0;
    CubeOrientation orientation = ORIENT_NORMAL;    // To keep track of which side is up
    int handleMoves();
    void populateActiveSequenceMove(char moveChar);
    void rotateCube(CubeOrientation newOrientation);
};

extern SequenceManager seqManager;
