#include "include/defines.h"
#include "include/motor.h"

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
    sprintf(out, "Left (1 if covered else 0):%i Right (1 if covered else 0):%i", !digitalRead(LEFT_LINE_FINDER), !digitalRead(RIGHT_LINE_FINDER));
    Serial.println(out);
}
