// Define the pins for the infrared sensors
#include "../defines.h"
#include "../motor.h"

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
    // Print the line finder results
    Serial.println((!digitalRead(RIGHT_LINE_FINDER) << 7) | (!digitalRead(LEFT_LINE_FINDER) << 6));
}
