// Include the motor library
#include "motor.h"

// Define the pins for the motors
#define ENA 5
#define ENB 6
#define MOTOR_A_1 10
#define MOTOR_A_2 11
#define MOTOR_B_1 8
#define MOTOR_B_2 9
#define LEFT_LINE_FINDER A4
#define RIGHT_LINE_FINDER A5
#define ULTRASONIC_ECHO A0
#define ULTRASONIC_TRIGGER A1

// Create a network of motors
MotorNetwork network(Motor(MOTOR_A_1, MOTOR_A_2, ENA), Motor(MOTOR_B_1, MOTOR_B_2, ENB));

void setup()
{
    // Setup the serial communication
    Serial.begin(115200);
    // Wait for the serial connection to be established
    while (!Serial)
        ;
}

void loop()
{
    // Check if there is any data available on the serial port
    while (Serial.available() > 0)
    {
        // Read the command from the serial port
        byte command = Serial.read();
        // Switch on the command
        switch (command & 0b00000111)
        {
        case 0b000:
            // Move forward
            network.forward();
            break;
        case 0b001:
            // Move backward
            network.backward();
            break;
        case 0b010:
            // Turn left
            network.left();
            break;
        case 0b011:
            // Turn right
            network.right();
            break;
        case 0b100:
            // Stop the motors
            network.stop();
            break;
        case 0b101:
            // The 5 most significant bits of the command represent the speed
            network.setSpeed(command & 0b11111000);
            break;
        }
    }

#ifdef ULTRASONIC
    digitalWrite(ULTRASONIC_TRIGGER, false);
    delayMicroseconds(2);

    digitalWrite(ULTRASONIC_TRIGGER, true);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIGGER, false);

    long duration = pulseIn(ULTRASONIC_ECHO, true);
    int distanceCentimetres = duration * 0.034 / 2;

    Serial.write(((analogRead(LEFT_LINE_FINDER) < 512) << 7) | ((analogRead(RIGHT_LINE_FINDER) < 512) << 6) | (((distanceCentimetres >> 4) & 0b00011111) << 1));
    Serial.write(((distanceCentimetres & 0b00001111) << 4) | 1);
#else
    Serial.write(((analogRead(LEFT_LINE_FINDER) < 512) << 7) | ((analogRead(RIGHT_LINE_FINDER) < 512) << 6) | (((0 >> 4) & 0b00011111) << 1));
    Serial.write(((0 & 0b00001111) << 4) | 1);
#endif
}
