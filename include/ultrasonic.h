#include "defines.h"
#include "motor.h"

int distanceCentimetres()
{
    // Set the ultrasonic trigger to low for 2 microseconds
    analogWrite(ULTRASONIC_TRIGGER, 0);
    delayMicroseconds(2);

    // Pulse the ultrasonic trigger high for 10 microseconds
    analogWrite(ULTRASONIC_TRIGGER, 255);
    delayMicroseconds(10);
    analogWrite(ULTRASONIC_TRIGGER, 0);

    // Time the duration until the echo pin goes high
    float duration = pulseIn(ULTRASONIC_ECHO, true);
    // Calculate the distance by multiplying the duration by a fraction of the speed of light
    float distance = duration * 0.0343 / 2;
    // Convert the distance to an integer
    int distanceCentimetres = distance;
    return distanceCentimetres;
}
