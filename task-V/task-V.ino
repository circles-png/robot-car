#include "include/defines.h"
#include "include/motor.h"

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
    if (digitalRead(LEFT_LINE_FINDER))
    {
        network.leftMotor.backward();
    }
    else
    {
        network.leftMotor.forward();
    }
    if (digitalRead(RIGHT_LINE_FINDER))
    {
        network.rightMotor.backward();
    }
    else
    {
        network.rightMotor.forward();
    }
}
