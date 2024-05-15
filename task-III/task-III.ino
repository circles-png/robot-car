#include "include/defines.h"

void setup()
{
    // Setup the serial communication
    Serial.begin(115200);
    // Wait for the serial connection to be established
    while (!Serial)
        ;
}

void loop()
{
    char out[128];
    sprintf(out, "Left [%c]  ----  [%c] Right", digitalRead(LEFT_LINE_FINDER) ? ' ' : '!', digitalRead(RIGHT_LINE_FINDER) ? ' ' : '!');
    Serial.println(out);
    delay(50);
}
