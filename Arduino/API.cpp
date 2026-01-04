#include <Arduino.h>
#include "Servo.h"
#include "SequenceManager.h"
#include "Config.h"
#include "API.h"
#include "Types.h"

void APISetup()
{
    Serial.println("Listening for API commands, type 'HELP' for list of commands.");
}

void APILoop()
{
    if (!Serial.available())
        return;

    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() == 0)
        return;

    //line.toUpperCase();

    // ---- TOKENIZE ----
    const int MAX_TOKENS = 5;
    String tokens[MAX_TOKENS];
    int tokenCount = 0;

    int start = 0;
    while (start < line.length() && tokenCount < MAX_TOKENS)
    {
        int space = line.indexOf(' ', start);
        if (space == -1)
            space = line.length();

        tokens[tokenCount++] = line.substring(start, space);
        start = space + 1;
        while (start < line.length() && line[start] == ' ')
            start++;
    }

    String cmd = tokens[0];

    // --- HELP ---
    if (cmd == "HELP")
    {
        Serial.println("Commands:");
        Serial.println("HELP");
        Serial.println("PING");
        Serial.println("STATUS [servo]");
        Serial.println("    servo: servo type character (R, L, F, B for sliders; r, l, f, b for spinners)");
        Serial.println("    if no servo specified, returns BUSY or IDLE depending on sequence status");
        Serial.println("MOVE <string>|C");
        Serial.println("    Example MOVE string: rC0_lR500_fL1000");
        Serial.println("    (move servo RIGHT_SPINNER to CENTER at time 0ms, LEFT_SLIDER to RIGHT AFTER waiting 500ms, FRONT_SLIDER to LEFT AFTER waiting 1000ms)");
        Serial.println("    'C' alone cancels current sequence (if busy)");
        return;
    }

    // --- MOVE SERVO ---
    if (cmd == "MOVE")
    {
        if (tokenCount != 2)
        {
            Serial.println("ERR args");
            return;
        }

        int res = seqManager.startSequence(tokens[1].c_str());
        if (res == 0)
        {
            Serial.println("OK");
        }
        else if (res == -1)
        {
            Serial.println("ERR busy");
        }
        else if (res == -2)
        {
            Serial.println("ERR format");
        }
        else
        {
            Serial.println("ERR");
        }
        return;
    }

    // --- SERVO STATUS ---
    if (cmd == "STATUS")
    {
        if (tokenCount == 1)
        {
            if (seqManager.isBusy())
            {
                Serial.println("BUSY");
            }
            else
            {
                Serial.println("IDLE");
            }
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
        }

        return;
    }

    // --- PING ---
    if (cmd == "PING")
    {
        Serial.println("PONG");
        return;
    }

    // --- UNKNOWN ---
    Serial.print("ERR cmd: ");
    Serial.println(cmd);
}