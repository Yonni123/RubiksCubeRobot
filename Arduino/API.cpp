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

    line.toUpperCase();

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
        Serial.println("    servo: servo number (0-" + String(NUM_SERVOS - 1) + ")");
        Serial.println("    if no servo specified, returns BUSY or IDLE depending on sequence status");
        Serial.println("MOVE <string>|C");
        Serial.println("    Example MOVE string: 2C0_4R500_2L1000");
        Serial.println("    (move servo 2 to C asap, servo 4 to R 500ms after asap, servo 2 to L 1000ms after asap)");
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

        int servo = tokens[1].toInt();

        if (servo < 0 || servo >= NUM_SERVOS)
        {
            Serial.println("ERR servo out_of_range");
            return;
        }

        char c = 'C';
        switch (servos[servo].getState())
        {
            case STATE_R: c = 'R'; break;
            case STATE_L: c = 'L'; break;
            case STATE_C: c = 'C'; break;
        }

        Serial.println(c);
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