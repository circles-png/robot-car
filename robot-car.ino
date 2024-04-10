#include "motor.h"

#define ENA 6
#define ENB 5
#define MOTOR_A_1 11
#define MOTOR_A_2 10
#define MOTOR_B_1 9
#define MOTOR_B_2 8

MotorNetwork network(Motor(MOTOR_A_1, MOTOR_A_2, ENA), Motor(MOTOR_B_1, MOTOR_B_2, ENB));

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("Ready");
}

void loop()
{
    Serial.println("Moving forward");
    network.forward();
    delay(2000);

    Serial.println("Moving backward");
    network.backward();
    delay(2000);

    Serial.println("Stopping");
    network.stop();
    delay(2000);
}
