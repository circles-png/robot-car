// Define the pins
#define ENA 5
#define ENB 6
#define MOTOR_A_1 10
#define MOTOR_A_2 11
#define MOTOR_B_1 8
#define MOTOR_B_2 9
#define LEFT_LINE_FINDER 4
#define RIGHT_LINE_FINDER 7
#define ULTRASONIC_ECHO A1
#define ULTRASONIC_TRIGGER A0
// #define IR_RECEIVER 1

#define LEFT !digitalRead(LEFT_LINE_FINDER)
#define RIGHT !digitalRead(RIGHT_LINE_FINDER)

#define Cereal Serial
