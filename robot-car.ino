#include "motor.h"

#define ENA 6
#define ENB 5
#define MOTOR_A_1 13
#define MOTOR_A_2 12
#define MOTOR_B_1 11
#define MOTOR_B_2 10

Motor A(MOTOR_A_1, MOTOR_A_2, ENA);
Motor B(MOTOR_B_1, MOTOR_B_2, ENB);

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Ready");
}

void loop()
{
    A.forward();
    B.forward();
    A.setSpeed(255);
    B.setSpeed(255);
    delay(2000);

    A.backward();
    B.backward();
    A.setSpeed(255);
    B.setSpeed(255);
    delay(2000);

    A.stop();
    B.stop();
    delay(2000);
}
