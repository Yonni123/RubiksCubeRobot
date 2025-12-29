#include "SequenceManager.h"

CubeSequenceManager seqManager(cubeController);

// Helper to parse single move
static bool parseSingleMove(char face, char modifier, CubeMove& outMove)
{
    switch (face)
    {
    case 'U': outMove = MOVE_U; break;
    case 'D': outMove = MOVE_D; break;
    case 'L': outMove = MOVE_L; break;
    case 'R': outMove = MOVE_R; break;
    case 'F': outMove = MOVE_F; break;
    case 'B': outMove = MOVE_B; break;
    default: return false;
    }

    if (modifier == '\'')
    {
        switch (outMove)
        {
        case MOVE_U: outMove = MOVE_UP; break;
        case MOVE_D: outMove = MOVE_DP; break;
        case MOVE_L: outMove = MOVE_LP; break;
        case MOVE_R: outMove = MOVE_RP; break;
        case MOVE_F: outMove = MOVE_FP; break;
        case MOVE_B: outMove = MOVE_BP; break;
        }
    }

    return true;
}

// ------------------------------

CubeSequenceManager::CubeSequenceManager(CubeController& controller)
    : cube(controller) {}

// Start a new sequence (interrupts any current one)
void CubeSequenceManager::startSequence(const char* moveString)
{
    activeSequence = moveString;
    sequenceIndex = 0;
    nextMoveAt = millis();
    sequenceActive = true;
}

// Called repeatedly in loop()
void CubeSequenceManager::tick()
{
    cube.tick();
    if (!sequenceActive)
        return;

    unsigned long now = millis();
    if (now < nextMoveAt)
        return;

    scheduleNextMove();
}

// Schedule the next move from the string
void CubeSequenceManager::scheduleNextMove()
{
    char c = activeSequence[sequenceIndex];
    if (c == '\0')
    {
        sequenceActive = false; // Finished
        return;
    }

    char modifier = '\0';
    // Look ahead for ' or 2
    if (activeSequence[sequenceIndex + 1] == '\'' ||
        activeSequence[sequenceIndex + 1] == '2')
    {
        modifier = activeSequence[sequenceIndex + 1];
        sequenceIndex++;
    }

    CubeMove move;
    if (!parseSingleMove(c, modifier == '\'' ? '\'' : '\0', move))
    {
        sequenceActive = false; // invalid move
        return;
    }

    unsigned long delayOffset = 0;
    if (modifier == '2')
    {
        delayOffset += cube.executeMove(move);
        delayOffset += cube.executeMove(move, delayOffset);
    }
    else
    {
        delayOffset += cube.executeMove(move);
    }

    sequenceIndex++;
    nextMoveAt = millis() + delayOffset;
}
