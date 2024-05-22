// Task IV - Move the robot depending on the distance detected by the ultrasonic sensor

// Include the pins and other parameters at compile time
#include "include/defines.h"
#include "include/motor.h"
#include "include/ultrasonic.h"

// Set up a motor network
MotorNetwork network(Motor(MOTOR_A_1, MOTOR_A_2, ENA), Motor(MOTOR_B_1, MOTOR_B_2, ENB));

void setup()
{
    // Setup the serial communication
    Cereal.begin(115200);
    pinMode(ULTRASONIC_TRIGGER, OUTPUT);
    // Wait for the serial connection to be established
    while (!Cereal)
        ;
}

void loop()
{
    // Read the ultrasonic sensor and store the distance
    int distance = distanceCentimetres();

    // If the distance is more than the max range, ignore it
    if (distance > 400)
    {
        return;
    }

    int speed = distance < 30
                    ? distance * 255 / 30
                    : 255;
    network.setSpeed(speed);

    // Print a model of the car to the serial monitor
    Cereal.print("[");
    Cereal.print(speed);
    Cereal.print("] <- ");
    // Print the distance to the serial monitor
    Cereal.print(distance);
    Cereal.print(" cm ");
    // Print some dashes
    for (int i = 0; i < distance / 10; i++)
    {
        Cereal.print("-- ");
    }
    Cereal.println("-> |");

    delay(300);
}
