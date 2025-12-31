#include "Arduino.h"
#include "CubeController.h"
#define SERVO_MOVE_DELAY 210  // milliseconds between servo moves

CubeOrientation currentOrientation = ORIENT_NORMAL;

#define MAX_QUEUE_SIZE 48
struct ScheduledMove {
    int servoIndex;
    ServoState targetState;
    unsigned long executeAt;  // millis() timestamp
};
ScheduledMove moveQueue[MAX_QUEUE_SIZE];
unsigned int queueHead = 0;
unsigned int queueTail = 0;

void enqueueMove(ScheduledMove move)
{
    unsigned int nextTail = (queueTail + 1) % MAX_QUEUE_SIZE;
    if (nextTail == queueHead)
    {
        // Queue is full, cannot enqueue
        Serial.println("Move queue is full, cannot enqueue move.");
        return;
    }

    moveQueue[queueTail] = move;
    queueTail = nextTail;
}

void dequeueMove()
{
    if (queueHead == queueTail)
    {
        // Queue is empty
        Serial.println("Move queue is empty, cannot dequeue move.");
        return;
    }

    queueHead = (queueHead + 1) % MAX_QUEUE_SIZE;
}

void CubeController::openAllSliders()
{
    unsigned long now = millis();
    for (int i = 0; i < numServos; i++)
    {
        if (servos[i].getType() % 2 == 1) // Slider servo is odd
        {
            enqueueMove({i, STATE_R, now});
        }
    }
}

void CubeController::closeAllSliders()
{
    unsigned long now = millis();
    for (int i = 0; i < numServos; i++)
    {
        if (servos[i].getType() % 2 == 1) // Slider servo is odd
        {
            enqueueMove({i, STATE_C, now});
        }
    }
}

int CubeController::rotateCube(CubeOrientation newOrientation)
{
    if (newOrientation == currentOrientation)
        return 0; // No change

    unsigned long now = millis();
    enqueueMove({FRONT_SLIDER, STATE_L, now});  // Grip the front and back side
    enqueueMove({BACK_SLIDER, STATE_L, now});

    enqueueMove({RIGHT_SLIDER, STATE_R, now});  // Release the right and left side
    enqueueMove({LEFT_SLIDER, STATE_R, now});

    if (newOrientation == ORIENT_INVERT)
    {
        enqueueMove({FRONT_SPINNER, STATE_R, now + SERVO_MOVE_DELAY * 1});  // Rotate front and back to make the entire cube turn
        enqueueMove({BACK_SPINNER, STATE_L, now + SERVO_MOVE_DELAY * 1});
    }
    else if (newOrientation == ORIENT_NORMAL)
    {
        enqueueMove({FRONT_SPINNER, STATE_L, now + SERVO_MOVE_DELAY * 1});  // Rotate front and back to make the entire cube turn
        enqueueMove({BACK_SPINNER, STATE_R, now + SERVO_MOVE_DELAY * 1});
    }

    enqueueMove({RIGHT_SLIDER, STATE_C, now + SERVO_MOVE_DELAY * 2});   // Close right and left side sliders
    enqueueMove({LEFT_SLIDER, STATE_C, now + SERVO_MOVE_DELAY * 2});

    enqueueMove({FRONT_SLIDER, STATE_R, now + SERVO_MOVE_DELAY * 3});   // Release front and back side sliders
    enqueueMove({BACK_SLIDER, STATE_R, now + SERVO_MOVE_DELAY * 3});

    enqueueMove({FRONT_SPINNER, STATE_C, now + SERVO_MOVE_DELAY * 4});  // Center front and back again
    enqueueMove({BACK_SPINNER, STATE_C, now + SERVO_MOVE_DELAY * 4});
    enqueueMove({FRONT_SPINNER, STATE_C, now + SERVO_MOVE_DELAY * 4});  // Center twice to go to absolute center
    enqueueMove({BACK_SPINNER, STATE_C, now + SERVO_MOVE_DELAY * 4});

    enqueueMove({FRONT_SLIDER, STATE_C, now + SERVO_MOVE_DELAY * 5});   // Close front and back side sliders
    enqueueMove({BACK_SLIDER, STATE_C, now + SERVO_MOVE_DELAY * 5});

    currentOrientation = newOrientation;

    return SERVO_MOVE_DELAY * 6; // Total time taken for the rotation
}

int CubeController::executeMove(CubeMove move, unsigned long delayOffset = 0)
{
    // We can only move front, back, left and right directly.
    // To move up and down, we need to rotate the cube first.
    // If cube is inverted, left and right moves become up and down moves.
    unsigned long now = millis();
    now += delayOffset;
    int rotDel = 0;
    ServoState targetState; 
    int spinnerIndex;
    int sliderIndex;
    switch (move)
    {
    case MOVE_F:
        spinnerIndex = FRONT_SPINNER;
        sliderIndex  = FRONT_SLIDER;
        targetState = STATE_R;
        break;
    case MOVE_B:
        spinnerIndex = BACK_SPINNER;
        sliderIndex  = BACK_SLIDER;
        targetState = STATE_R;
        break;
    case MOVE_R:
        if (currentOrientation == ORIENT_INVERT)
        {
            rotDel = rotateCube(ORIENT_NORMAL);
        }
        now += rotDel;
        spinnerIndex = RIGHT_SPINNER;
        sliderIndex  = RIGHT_SLIDER;
        targetState = STATE_R;
        break;
    case MOVE_L:
        if (currentOrientation == ORIENT_INVERT)
        {
            rotDel = rotateCube(ORIENT_NORMAL);
        }
        now += rotDel;
        spinnerIndex = LEFT_SPINNER;
        sliderIndex  = LEFT_SLIDER;
        targetState = STATE_R;
        break;
    case MOVE_U:
        if (currentOrientation == ORIENT_NORMAL)
        {
            rotDel = rotateCube(ORIENT_INVERT);
        }
        now += rotDel;
        spinnerIndex = RIGHT_SPINNER;
        sliderIndex  = RIGHT_SLIDER;
        targetState = STATE_R;
        break;
    case MOVE_D:
        if (currentOrientation == ORIENT_NORMAL)
        {
            rotDel = rotateCube(ORIENT_INVERT);
        }
        now += rotDel;
        spinnerIndex = LEFT_SPINNER;
        sliderIndex  = LEFT_SLIDER;
        targetState = STATE_R;
        break;
    case MOVE_FP:
        spinnerIndex = FRONT_SPINNER;
        sliderIndex  = FRONT_SLIDER;
        targetState = STATE_L;
        break;
    case MOVE_BP:
        spinnerIndex = BACK_SPINNER;
        sliderIndex  = BACK_SLIDER;
        targetState = STATE_L;
        break;
    case MOVE_RP:
        if (currentOrientation == ORIENT_INVERT)
        {
            rotDel = rotateCube(ORIENT_NORMAL);
        }
        now += rotDel;
        spinnerIndex = RIGHT_SPINNER;
        sliderIndex  = RIGHT_SLIDER;
        targetState = STATE_L;
        break;
    case MOVE_LP:
        if (currentOrientation == ORIENT_INVERT)
        {
            rotDel = rotateCube(ORIENT_NORMAL);
        }
        now += rotDel;
        spinnerIndex = LEFT_SPINNER;
        sliderIndex  = LEFT_SLIDER;
        targetState = STATE_L;
        break;
    case MOVE_UP:
        if (currentOrientation == ORIENT_NORMAL)
        {
            rotDel = rotateCube(ORIENT_INVERT);
        }
        now += rotDel;
        spinnerIndex = RIGHT_SPINNER;
        sliderIndex  = RIGHT_SLIDER;
        targetState = STATE_L;
        break;
    case MOVE_DP:
        if (currentOrientation == ORIENT_NORMAL)
        {
            rotDel = rotateCube(ORIENT_INVERT);
        }
        now += rotDel;
        spinnerIndex = LEFT_SPINNER;
        sliderIndex  = LEFT_SLIDER;
        targetState = STATE_L;
        break;
    default:
        return 0; // Invalid move
        break;
    }

    enqueueMove({spinnerIndex, targetState, now});  // Spin into target state
    enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});    // Release slider to allow rotation
    enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});   // Center spinner after rotation
    enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});   // Center again to ensure absolute center
    enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});    // Close slider to grip again

    return SERVO_MOVE_DELAY * 4 + rotDel; // Total time taken for the move
}

void CubeController::tick()
{
    unsigned long currentMillis = millis();

    while (queueHead != queueTail)
    {
        ScheduledMove& nextMove = moveQueue[queueHead];
        if (currentMillis >= nextMove.executeAt)
        {
            servos[nextMove.servoIndex].setState(nextMove.targetState);
            dequeueMove();
        }
        else
        {
            break; // Next move is not ready yet
        }
    }
}

CubeController cubeController(servos);
