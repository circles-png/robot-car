#include "motor.h"

#define ENA 6
#define ENB 5
#define MOTOR_A_1 11
#define MOTOR_A_2 10
#define MOTOR_B_1 9
#define MOTOR_B_2 8

#define TINY_IO

MotorNetwork network(Motor(MOTOR_A_1, MOTOR_A_2, ENA), Motor(MOTOR_B_1, MOTOR_B_2, ENB));

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;
}

void loop()
{
    while (Serial.available() > 0)
    {
#ifdef TINY_IO
        byte command = Serial.read();
        switch (command & 0b00000111)
        {
        case 0b000:
            network.forward();
            break;
        case 0b001:
            network.backward();
            break;
        case 0b010:
            network.left();
            break;
        case 0b011:
            network.right();
            break;
        case 0b100:
            network.stop();
            break;
        case 0b101:
            network.setSpeed(command & 0b11111000);
            break;
        }
#else
        String command = Serial.readString();
        Serial.println(command);
        if (command == "forward")
        {
            network.forward();
        }
        else if (command == "backward")
        {
            network.backward();
        }
        else if (command == "left")
        {
            network.left();
        }
        else if (command == "right")
        {
            network.right();
        }
        else if (command == "stop")
        {
            network.stop();
        }
        else if (command.startsWith("speed"))
        {
            int speed = command.substring(6).toInt();
            network.setSpeed(speed);
        }
        else
        {
            Serial.println("Invalid command");
        }
#endif
    }
}
