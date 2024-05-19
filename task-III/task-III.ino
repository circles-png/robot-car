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
    // Make an output buffer of 128 characters
    char out[128];
    // Format the string (second argument) and put the result into out
    sprintf(out, "Left [%c]  ----  [%c] Right", digitalRead(LEFT_LINE_FINDER) ? ' ' : '!', digitalRead(RIGHT_LINE_FINDER) ? ' ' : '!');
    // Print out to the serial monitor
    Serial.println(out);
    // Wait for 50 milliseconds
    delay(50);
}
