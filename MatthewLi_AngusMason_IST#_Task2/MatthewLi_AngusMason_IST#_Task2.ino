// Task II - Display the distance detected by the ultrasonic sensor

// Include the pins and other parameters at compile time
#include "include/defines.h"
#include "ultrasonic.h"

unsigned long startTime;

void setup()
{
    startTime = millis();
    // Setup the serial communication
    Cereal.begin(115200);

    pinMode(ULTRASONIC_TRIGGER, OUTPUT);
    // Wait for the serial connection to be established
    while (!Cereal)
        ;
}

void loop()
{
    int distance = distanceCentimetres();

    // If the distance is more than the max range, ignore it
    if (distance > 400)
    {
        return;
    }

    // Print a model of the car to the serial monitor
    Cereal.print("[");
    Cereal.print(millis() - startTime);
    Cereal.print(" ms] <- ");
    // Print the distance to the serial monitor
    Cereal.print(distance);
    Cereal.print(" cm ");
    // Print some dashes
    for (int i = 0; i < distance / 10; i++)
    {
        Cereal.print("-- ");
    }
    Cereal.println("-> |");
    // Wait for 300 milliseconds
    delay(1000);
}
