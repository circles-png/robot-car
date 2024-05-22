// Task III - Display the status of the line finders

// Include the pins and other parameters at compile time
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
    sprintf(out, "Left [%c]  ----  [%c] Right", LEFT ? '!' : ' ', RIGHT ? '!' : ' ');
    // Print out to the serial monitor
    Serial.println(out);
    // Wait for 50 milliseconds
    delay(50);
}
