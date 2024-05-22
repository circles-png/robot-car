// Task VI - Control the robot with binary serial input from a Rust client (../client).
// Use an IR receiver to provide another control method.

// Include the pins and other parameters at compile time
#include <IRremote.h>
#include "include/defines.h"
#include "include/motor.h"

// Create a network of motors
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

// Initialise an infrared receiver interface
IRrecv irrecv(1);

#ifdef IR_RECEIVER
enum IrCommand
{
    up,
    left,
    down,
    right
};
IrCommand lastCommand;
unsigned long lastCommandTime;
void handleCommand(IrCommand command)
{
    switch (command)
    {
    case IrCommand::up:
        network.forward();
        break;
    case IrCommand::left:
        network.left();
        break;
    case IrCommand::down:
        network.backward();
        break;
    case IrCommand::right:
        network.right();
        break;
    }
}
#endif

void loop()
{
#ifdef IR_RECEIVER
    decode_results results;
    if (irrecv.decode(&results))
    {
        switch (results.value)
        {
        case 0xDC23B04F: // up
            lastCommand = IrCommand::up;
            break;
        case 0xDC2308F7: // left
            lastCommand = IrCommand::left;
            break;
        case 0xDC23A857: // down
            lastCommand = IrCommand::down;
            break;
        case 0xDC2348B7: // right
            lastCommand = IrCommand::right;
            break;
        }
        if (results.value == 0xDC23B04F ||
            results.value == 0xDC2308F7 ||
            results.value == 0xDC23A857 ||
            results.value == 0xDC2348B7 ||
            results.value == 0xFFFFFFFF)
        {
            handleCommand(lastCommand);
            lastCommandTime = millis();
        }
        irrecv.resume();
    }
    if (millis() - lastCommandTime > 400)
    {
        network.stop();
    }
#else
    // Check if there is any data available on the serial port
    while (Cereal.available() > 0)
    {
        // Read the command from the serial port
        byte command = Cereal.read();
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
#endif
    analogWrite(ULTRASONIC_TRIGGER, 0);
    delayMicroseconds(2);

    analogWrite(ULTRASONIC_TRIGGER, 255);
    delayMicroseconds(10);
    analogWrite(ULTRASONIC_TRIGGER, 0);

    float duration = pulseIn(ULTRASONIC_ECHO, true);
    float distance = duration * 0.0343 / 2;
    int distanceCentimetres = distance;

    Cereal.write((RIGHT << 7) | (LEFT << 6) | (((distanceCentimetres >> 4) & 0b00011111) << 1));
    Cereal.write(((distanceCentimetres & 0b00001111) << 4) | 1);
}
