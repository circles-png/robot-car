// Task II - Display the distance detected by the ultrasonic sensor

// Include the pins and other parameters at compile time
#include "include/defines.h"

unsigned long startTime;

void setup()
{
    startTime = millis();
    // Setup the serial communication
    Serial.begin(115200);
  
   pinMode(ULTRASONIC_TRIGGER, OUTPUT);
    // Wait for the serial connection to be established
    while (!Serial)
        ;
}

void loop()
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

    // If the distance is more than the max range, ignore it
    if (distanceCentimetres > 400)
    {
        return;
    }

    // Print a model of the car to the serial monitor
    Serial.print("[");
    Serial.print(millis() - startTime);
    Serial.print(" ms] <- ");
    // Print the distance to the serial monitor
    Serial.print(distanceCentimetres);
    Serial.print(" cm ");
    // Print some dashes
    for (int i = 0; i < distanceCentimetres / 10; i++)
    {
        Serial.print("-- ");
    }
    Serial.println("-> |");
    // Wait for 500 milliseconds
    delay(500);
}