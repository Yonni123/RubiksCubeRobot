#include <Arduino.h>
#include <string.h>
#include "Servo.h"
#include "SequenceManager.h"
#include "Config.h"
#include "API.h"
#include "Types.h"

#define MAX_TOKENS  5

static char rxBuf[SEQUENCE_BUFFER_SIZE];    // This is extremely wasteful for the SEQ command, but oh well fuck it
static char* tokens[MAX_TOKENS];

void APISetup()
{
    Serial.println("Listening for API commands, type 'HELP' for list of commands.");
    Serial.setTimeout(2000); // safer for long lines
}

static int tokenize(char* line, char** tokens, int maxTokens)
{
    int count = 0;

    while (*line && count < maxTokens)
    {
        // Skip leading spaces
        while (*line == ' ')
            ++line;

        if (*line == '\0')
            break;

        tokens[count++] = line;

        // Advance to next space
        while (*line && *line != ' ')
            ++line;

        // Null-terminate token
        if (*line)
        {
            *line = '\0';
            ++line;
        }
    }

    return count;
}

void APILoop()
{
    if (!Serial.available())
        return;

    int len = Serial.readBytesUntil('\n', rxBuf, SEQUENCE_BUFFER_SIZE - 1);
    rxBuf[len] = '\0';

    // Trim trailing CR (Windows serial)
    if (len > 0 && rxBuf[len - 1] == '\r')
        rxBuf[len - 1] = '\0';

    if (rxBuf[0] == '\0')
        return;

    int tokenCount = tokenize(rxBuf, tokens, MAX_TOKENS);
    if (tokenCount == 0)
        return;

    const char* cmd = tokens[0];

    // --- HELP ---
    if (strcmp(cmd, "HELP") == 0)
    {
        Serial.println("Commands:");
        Serial.println("HELP");
        Serial.println("PING");
        Serial.println("STATUS [servo]");
        Serial.println("SEQ <string>|C");
        return;
    }

    // --- SEQ ---
    if (strcmp(cmd, "SEQ") == 0)
    {
        if (tokenCount != 2)
        {
            Serial.println("ERR args");
            return;
        }

        int res = seqManager.startSequence(tokens[1]);
        if (res == 0) Serial.println("OK");
        else if (res == -1) Serial.println("ERR busy");
        else if (res == -2) Serial.println("ERR format");
        else Serial.println("ERR");

        return;
    }

    // --- MOVE ---
    if (strcmp(cmd, "MOVE") == 0)
    {
        if (tokenCount != 3)
        {
            Serial.println("ERR args");
            return;
        }

        int delay = atoi(tokens[1]);
        if (delay < 0)
        {
            Serial.println("ERR delay");
            return;
        }

        int res = seqManager.startMoves(tokens[2], delay);
        if (res == 0) Serial.println("OK");
        else if (res == -1) Serial.println("ERR busy");
        else if (res == -2) Serial.println("ERR format");
        else Serial.println("ERR");

        return;
    }

    // --- STATUS ---
    if (strcmp(cmd, "STATUS") == 0)
    {
        if (tokenCount == 1)
        {
            Serial.println(seqManager.isBusy() ? "BUSY" : "IDLE");
            return;
        }

        if (tokenCount != 2)
        {
            Serial.println("ERR args");
            return;
        }

        ServoType servo;
        if (!parseServoType(tokens[1][0], servo))
        {
            Serial.println("ERR servo_type");
            return;
        }

        switch (servos[servo].getState())
        {
            case STATE_R: Serial.println('R'); break;
            case STATE_L: Serial.println('L'); break;
            case STATE_C: Serial.println('C'); break;
            case STATE_r: Serial.println('r'); break;
            case STATE_l: Serial.println('l'); break;
            default: Serial.println("ERR state"); break;
        }
        return;
    }

    // --- PING ---
    if (strcmp(cmd, "PING") == 0)
    {
        Serial.println("PONG");
        return;
    }

    // --- UNKNOWN ---
    Serial.print("ERR cmd: ");
    Serial.println(cmd);
}
