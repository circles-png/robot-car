// Define the pins for the ultrasonic sensor
#include "defines.h"

void setup()
{
    // Setup the serial communication
    Serial.begin(115200);
    pinMode(ULTRASONIC_TRIGGER, OUTPUT);
    // Wait for the serial connection to be established
    while (!Serial)
        ;
}

void loop()
{
    analogWrite(ULTRASONIC_TRIGGER, 0);
    delayMicroseconds(2);

    analogWrite(ULTRASONIC_TRIGGER, 255);
    delayMicroseconds(10);
    analogWrite(ULTRASONIC_TRIGGER, 0);

    float duration = pulseIn(ULTRASONIC_ECHO, true);
    float distance = duration * 0.0343 / 2;
    int distanceCentimetres = distance;

    Serial.write(((distanceCentimetres & 0b00001111) << 4) | 1);
}
