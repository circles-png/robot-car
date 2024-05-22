// Task IV - Move the robot depending on the distance detected by the ultrasonic sensor

// Include the pins and other parameters at compile time
#include "include/defines.h"
#include "include/motor.h"

// Set up a motor network
MotorNetwork network(Motor(MOTOR_A_1, MOTOR_A_2, ENA), Motor(MOTOR_B_1, MOTOR_B_2, ENB));

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
    Serial.print("[--] <- ");
    // Print the distance to the serial monitor
    Serial.print(distanceCentimetres);
    Serial.print(" cm ");
    // Print some dashes
    for (int i = 0; i < distanceCentimetres / 10; i++)
    {
        Serial.print("-- ");
    }
    Serial.println("-> |");

    // If the distance is more than 10 cm, keep moving forward
    if (distanceCentimetres > 10)
    {
        network.forward();
    }
    else
    {
        // Otherwise, move backwards for 500 ms and turn left for 500 ms
        network.backward();
        delay(500);
        network.left();
        delay(500);
    }
}
