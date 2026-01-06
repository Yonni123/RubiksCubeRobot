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
        busy = 0;
        activeSequence[0] = '\0';
        notifyState();
        return 0;
    }

    if (busy)
        return -1;

    strncpy(activeSequence, moveString, sizeof(activeSequence) - 1);
    activeSequence[sizeof(activeSequence) - 1] = '\0';  // TODO: use strcpy?

    sequenceIndex = 0;
    nextMoveAt = millis();
    busy = 1;   // Busy with SEQ
    notifyState();

    return 0;
}

int SequenceManager::startMoves(const char* moveString, int delayMs)
{
    if (!moveString || *moveString == '\0')
        return -2;

    if (busy)
        return -1;

    movesDelayMs = delayMs;

    strncpy(moveBuf, moveString, sizeof(moveBuf) - 1);
    activeSequence[sizeof(moveBuf) - 1] = '\0'; // TODO: use strcpy?

    sequenceIndex = 0;
    nextMoveAt = millis();
    busy = 2;   // Busy with MOVE
    notifyState();

    return 0;
}

// Called repeatedly from loop()
int SequenceManager::tick()
{
    if (!isBusy())
        return 0;

    unsigned long now = millis();
    if (now < nextMoveAt)
        return 0;

    return executeUntilDelay();
}

void SequenceManager::notifyState()
{
    if (isBusy())
        Serial.println("BUSY");
    else
        Serial.println("IDLE");
}

int SequenceManager::executeUntilDelay()
{
    if (busy == 1)
        return handleSequence();
    else if (busy == 2)
        return handleMoves();

    return -10;  // Should not happen
}

int SequenceManager::handleSequence()
{
    // Execute until we hit a delay or end of sequence
    while (activeSequence[sequenceIndex] != '\0' &&
       !isdigit(activeSequence[sequenceIndex]))
    {
        ServoType servoType;
        if (!parseServoType(activeSequence[sequenceIndex], servoType))
        {
            busy = 0;
            activeSequence[0] = '\0';
            notifyState();
            return -3;  // servo type error
        }
        sequenceIndex++;

        ServoState state;
        if (!parseServoState(activeSequence[sequenceIndex], state))
        {
            busy = 0;
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
        if (busy == 2)
            // SEQ is being used by MOVE, don't clear yet
            return 2; // This is an indicator for the MOVE handler

        busy = 0;
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

// ==================================================================
// EVERYTHING BELOW THIS POINT IS FOR MOVE COMMAND HANDLING
// Realistically, they should be in another file, but I'm too lazy
// ==================================================================

int SequenceManager::handleMoves()
{
    int seqResult = handleSequence();
    if (seqResult < 0)
    {
        busy = 0;   // Error in sequence
        activeSequence[0] = '\0';
        moveBuf[0] = '\0';
        notifyState();
        return seqResult;
    }
    if (seqResult != 2)
    {
        return 0;   // Sequence handler is running normally
    }

    // ---- Sequence finished, give it next move ----
    char moveChar = moveBuf[moveIndex];
    if (moveChar == '\0')
    {
        busy = 0;   // All moves done
        activeSequence[0] = '\0';
        moveBuf[0] = '\0';
        moveIndex = 0;
        sequenceIndex = 0;
        notifyState();
        return 0;
    }

    // Populate activeSequence with next move and start a new sequence
    activeSequence[0] = '\0';
    populateActiveSequenceMove(moveChar);
    sequenceIndex = 0;
    nextMoveAt = millis();
    moveIndex++;
    return 1;   // Next move scheduled
}

void SequenceManager::rotateCube(CubeOrientation newOrientation)
{
    if (orientation == newOrientation)
        return;

    char *p = activeSequence;

    p += strlen(p);
    sprintf(p, "FLBLRRLR%d", movesDelayMs);  // FRONT and BACK grab, RIGHT and LEFT release

    p += strlen(p);
    if (newOrientation == ORIENT_NORMAL)
        sprintf(p, "fRfRbLbL%d", movesDelayMs);     // front spin right, back spin left, twice for true left and right
    else
        sprintf(p, "fLfLbRbR%d", movesDelayMs);     // front spin left, back spin right, twice for true left and right

    p += strlen(p);
    sprintf(p, "RLLL%d", movesDelayMs);         // RIGHT and LEFT grab

    p += strlen(p);
    sprintf(p, "FRBR%d", movesDelayMs);         // FRONT and BACK release
   
    p += strlen(p);
    sprintf(p, "fCfCbCbC%d", movesDelayMs);    // front and back spin to true center

    p += strlen(p);
    sprintf(p, "FCBC%d", movesDelayMs);         // FRONT and BACK sliders go back

    p += strlen(p);
    sprintf(p, "RCLC");         // Relax RIGHT and LEFT sliders, they don't need to grab anymore
    
    orientation = newOrientation;
}

// This function could have been done in a better way, but oh well, again, fuck it
void SequenceManager::populateActiveSequenceMove(char moveChar)
{
    char *p = activeSequence;

    switch (moveChar)
    {
    case 'U':
        rotateCube(ORIENT_INVERT);  // Up face must be on right or left to access it

        p += strlen(p);
        sprintf(p, "lR%d", movesDelayMs);  // Rotate left spinner to R

        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs);  // Move left slider to Release

        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);  // Center left spinner twice for true center (no gap compensation)

        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs);  // Center left slider again
        break;
                                           // The rest follow the same pattern, just with different sides..
    case 'u':
        rotateCube(ORIENT_INVERT);

        p += strlen(p);
        sprintf(p, "lL%d", movesDelayMs); 

        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs); 

        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);

        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs); 
        break;

    case 'D':
        rotateCube(ORIENT_INVERT);

        p += strlen(p);
        sprintf(p, "rR%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);

        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'd':
        rotateCube(ORIENT_INVERT);

        p += strlen(p);
        sprintf(p, "rL%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);

        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'L':
        rotateCube(ORIENT_NORMAL);  // Put left and right faces back in their normal position

        p += strlen(p);
        sprintf(p, "lR%d", movesDelayMs);  // Rotate left spinner to R

        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs);  // Move left slider to Release

        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);  // Center left spinner twice for true center (no gap compensation)

        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs);  // Center left slider again
        break;

    case 'l':
        rotateCube(ORIENT_NORMAL);  // Put left and right faces back in their normal position

        p += strlen(p);
        sprintf(p, "lL%d", movesDelayMs);  // Rotate left spinner to R

        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs);  // Move left slider to Release

        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);  // Center left spinner twice for true center (no gap compensation)

        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs);  // Center left slider again
        break;

    case 'R':
        rotateCube(ORIENT_NORMAL);

        p += strlen(p);
        sprintf(p, "rR%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);

        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'r':
        rotateCube(ORIENT_NORMAL);

        p += strlen(p);
        sprintf(p, "rL%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  

        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);

        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'F':
        // F and B are always accessible, no need to rotate cube
        p += strlen(p);
        sprintf(p, "fR%d", movesDelayMs);  // Rotate front spinner to R

        p += strlen(p);
        sprintf(p, "FR%d", movesDelayMs);  // Move front slider to Release

        p += strlen(p);
        sprintf(p, "fCfC%d", movesDelayMs);  // Center front spinner twice for true center (no gap compensation)

        p += strlen(p);
        sprintf(p, "FC%d", movesDelayMs);  // Center front slider again
        break;

    case 'f':
        // F and B are always accessible, no need to rotate cube
        p += strlen(p);
        sprintf(p, "fL%d", movesDelayMs);  // Rotate front spinner to R

        p += strlen(p);
        sprintf(p, "FR%d", movesDelayMs);  // Move front slider to Release

        p += strlen(p);
        sprintf(p, "fCfC%d", movesDelayMs);  // Center front spinner twice for true center (no gap compensation)

        p += strlen(p);
        sprintf(p, "FC%d", movesDelayMs);  // Center front slider again
        break;

    case 'B':
        // F and B are always accessible, no need to rotate cube
        p += strlen(p);
        sprintf(p, "bR%d", movesDelayMs);  // Rotate front spinner to R

        p += strlen(p);
        sprintf(p, "BR%d", movesDelayMs);  // Move front slider to Release

        p += strlen(p);
        sprintf(p, "bCbC%d", movesDelayMs);  // Center front spinner twice for true center (no gap compensation)

        p += strlen(p);
        sprintf(p, "BC%d", movesDelayMs);  // Center front slider again
        break;

    case 'b':
        // F and B are always accessible, no need to rotate cube
        p += strlen(p);
        sprintf(p, "bL%d", movesDelayMs);  // Rotate front spinner to R

        p += strlen(p);
        sprintf(p, "BR%d", movesDelayMs);  // Move front slider to Release

        p += strlen(p);
        sprintf(p, "bCbC%d", movesDelayMs);  // Center front spinner twice for true center (no gap compensation)

        p += strlen(p);
        sprintf(p, "BC%d", movesDelayMs);  // Center front slider again
        break;
    
    default:
        break;
    }
}
