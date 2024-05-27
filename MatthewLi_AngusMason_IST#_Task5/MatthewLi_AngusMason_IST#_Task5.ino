// Task V - Control the robot depending on the status of the line finders

// Include the pins and other parameters at compile time
#include "include/defines.h"
#include "include/motor.h"

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
    network.setSpeed(120);
    // Move left or right depending on the status of the line finders
    if (LEFT && RIGHT)
    {
        network.forward();
    }
    else if (LEFT)
    {
        network.left();
    }
    else if (RIGHT)
    {
        network.right();
    }
    else
    {
        network.stop();
    }
    delay(200);
}
