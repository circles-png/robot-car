#define ENA 6
#define ENB 5
#define MOTOR_A_1 13
#define MOTOR_A_2 12
#define MOTOR_B_1 11
#define MOTOR_B_2 10

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(MOTOR_A_1, OUTPUT);
    pinMode(MOTOR_A_2, OUTPUT);
    pinMode(MOTOR_B_1, OUTPUT);
    pinMode(MOTOR_B_2, OUTPUT);
    digitalWrite(ENA, HIGH);
    digitalWrite(ENB, HIGH);
    digitalWrite(MOTOR_A_1, true);
    digitalWrite(MOTOR_A_2, false);
    digitalWrite(MOTOR_B_1, true);
    digitalWrite(MOTOR_B_2, false);
}

void loop() {
    forward();
}

void motorAForward() {
    digitalWrite(MOTOR_A_1, true);
    digitalWrite(MOTOR_A_2, false);
}

void motorABackward() {
    digitalWrite(MOTOR_A_1, false);
    digitalWrite(MOTOR_A_2, true);
}

void motorBForward() {
    digitalWrite(MOTOR_B_1, true);
    digitalWrite(MOTOR_B_2, false);
}

void motorBBackward() {
    digitalWrite(MOTOR_B_1, false);
    digitalWrite(MOTOR_B_2, true);
}

void motorAStop() {
    digitalWrite(MOTOR_A_1, false);
    digitalWrite(MOTOR_A_2, false);
}

void motorBStop() {
    digitalWrite(MOTOR_B_1, false);
    digitalWrite(MOTOR_B_2, false);
}

void motorASetSpeed(int speed) {
    analogWrite(ENA, speed);
}

void motorBSetSpeed(int speed) {
    analogWrite(ENB, speed);
}

void stop() {
    motorAStop();
    motorBStop();
}

void forward() {
    motorAForward();
    motorBForward();
}

void backward() {
    motorABackward();
    motorBBackward();
}

void left() {
    motorAForward();
    motorBBackward();
}

void right() {
    motorABackward();
    motorBForward();
}
