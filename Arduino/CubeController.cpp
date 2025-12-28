#include "Arduino.h"
#include "CubeController.h"

#define MAX_QUEUE_SIZE 32
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
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
        break;
    case MOVE_B:
        spinnerIndex = FACE_B * 2 + 1;
        sliderIndex  = FACE_B * 2;
        enqueueMove({spinnerIndex, STATE_R, now});
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
        break;
    case MOVE_R:
        spinnerIndex = FACE_R * 2 + 1;
        sliderIndex  = FACE_R * 2;
        enqueueMove({spinnerIndex, STATE_R, now});
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
        break;
    case MOVE_L:
        spinnerIndex = FACE_L * 2 + 1;
        sliderIndex  = FACE_L * 2;
        enqueueMove({spinnerIndex, STATE_R, now});
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
        break;
    case MOVE_FP:
        spinnerIndex = FACE_F * 2 + 1;
        sliderIndex  = FACE_F * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
        break;
    case MOVE_BP:
        spinnerIndex = FACE_B * 2 + 1;
        sliderIndex  = FACE_B * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
        break;
    case MOVE_RP:
        spinnerIndex = FACE_R * 2 + 1;
        sliderIndex  = FACE_R * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
        break;
    case MOVE_LP:
        spinnerIndex = FACE_L * 2 + 1;
        sliderIndex  = FACE_L * 2;
        enqueueMove({spinnerIndex, STATE_L, now});
        enqueueMove({sliderIndex, STATE_R, now + 200});
        enqueueMove({spinnerIndex, STATE_C, now + 400});
        enqueueMove({sliderIndex, STATE_C, now + 600});
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
