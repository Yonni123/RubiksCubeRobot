#include "Arduino.h"
#include "CubeController.h"
#define SERVO_MOVE_DELAY 200  // milliseconds between servo moves

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
    int now = millis();
    for (int i = 0; i < numServos; i++)
    {
        if (servos[i].getType() == SLIDER)
        {
            enqueueMove({i, STATE_R, now});
        }
    }
}

void CubeController::closeAllSliders()
{
    int now = millis();
    for (int i = 0; i < numServos; i++)
    {
        if (servos[i].getType() == SLIDER)
        {
            enqueueMove({i, STATE_C, now});
        }
    }
}

void CubeController::rotateCube(CubeOrientation newOrientation)
{
    if (newOrientation == currentOrientation)
        return; // No change

    int spinnerFIndex = FACE_F * 2 + 1;
    int sliderFIndex  = FACE_F * 2;
    int spinnerBIndex = FACE_B * 2 + 1;
    int sliderBIndex  = FACE_B * 2;
    int spinnerRIndex = FACE_R * 2 + 1;
    int sliderRIndex  = FACE_R * 2;
    int spinnerLIndex = FACE_L * 2 + 1;
    int sliderLIndex  = FACE_L * 2;

    int now = millis();
    enqueueMove({sliderFIndex, STATE_L, now});  // Grip the front and back side
    enqueueMove({sliderBIndex, STATE_L, now});

    enqueueMove({sliderRIndex, STATE_R, now});  // Release the right and left side
    enqueueMove({sliderLIndex, STATE_R, now});

    if (newOrientation == ORIENT_INVERT)
    {
        enqueueMove({spinnerFIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});  // Rotate front and back to make the entire cube turn
        enqueueMove({spinnerBIndex, STATE_L, now + SERVO_MOVE_DELAY* 1});
    }
    else if (newOrientation == ORIENT_NORMAL)
    {
        enqueueMove({spinnerFIndex, STATE_L, now + SERVO_MOVE_DELAY * 1});  // Rotate front and back to make the entire cube turn
        enqueueMove({spinnerBIndex, STATE_R, now + SERVO_MOVE_DELAY* 1});
    }

    enqueueMove({sliderRIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});   // Close right and left side sliders
    enqueueMove({sliderLIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});

    enqueueMove({sliderFIndex, STATE_R, now + SERVO_MOVE_DELAY * 3});   // Release front and back side sliders
    enqueueMove({sliderBIndex, STATE_R, now + SERVO_MOVE_DELAY * 3});

    enqueueMove({spinnerFIndex, STATE_C, now + SERVO_MOVE_DELAY * 4});  // Center front and back again
    enqueueMove({spinnerBIndex, STATE_C, now + SERVO_MOVE_DELAY * 4});

    enqueueMove({sliderFIndex, STATE_C, now + SERVO_MOVE_DELAY * 5});   // Close front and back side sliders
    enqueueMove({sliderBIndex, STATE_C, now + SERVO_MOVE_DELAY * 5});

    currentOrientation = newOrientation;
}

void CubeController::executeMove(CubeMove move)
{
    unsigned long now = millis();
    int spinnerIndex;
    int sliderIndex;
    switch (move)
    {
    case MOVE_F:
        spinnerIndex = FACE_F * 2 + 1;
        sliderIndex  = FACE_F * 2;
        enqueueMove({spinnerIndex, STATE_R, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    case MOVE_B:
        spinnerIndex = FACE_B * 2 + 1;
        sliderIndex  = FACE_B * 2;
        enqueueMove({spinnerIndex, STATE_R, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    case MOVE_R:
        spinnerIndex = FACE_R * 2 + 1;
        sliderIndex  = FACE_R * 2;
        enqueueMove({spinnerIndex, STATE_R, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    case MOVE_L:
        spinnerIndex = FACE_L * 2 + 1;
        sliderIndex  = FACE_L * 2;
        enqueueMove({spinnerIndex, STATE_R, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    case MOVE_FP:
        spinnerIndex = FACE_F * 2 + 1;
        sliderIndex  = FACE_F * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    case MOVE_BP:
        spinnerIndex = FACE_B * 2 + 1;
        sliderIndex  = FACE_B * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    case MOVE_RP:
        spinnerIndex = FACE_R * 2 + 1;
        sliderIndex  = FACE_R * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    case MOVE_LP:
        spinnerIndex = FACE_L * 2 + 1;
        sliderIndex  = FACE_L * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + SERVO_MOVE_DELAY * 1});
        enqueueMove({spinnerIndex, STATE_C, now + SERVO_MOVE_DELAY * 2});
        enqueueMove({sliderIndex, STATE_C, now + SERVO_MOVE_DELAY * 3});
        break;
    default:
        // Other moves not implemented yet
        break;
    }
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
