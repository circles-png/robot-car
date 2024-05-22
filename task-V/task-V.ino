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
    network.setSpeed(120);
    // Move left or right depending on the status of the line finders
    if (LEFT && RIGHT)
    {
        network.forward();
    }
    else if (LEFT)
    {
        network.right();
    }
    else if (RIGHT)
    {
        network.left();
    }
    else
    {
        network.stop();
    }
}
