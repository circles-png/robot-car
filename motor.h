// This class represents a motor
class Motor
{
public:
    // Constructor for the Motor class
    Motor(
        int pin1,  // Pin 1 of the motor
        int pin2,  // Pin 2 of the motor
        int enable // Enable pin. Must be a PWM pin (3, 5, 6, 9, 10, or 11 on the Arduino Uno)
    );
    int pin1; // Connection on the left terminal block of the motor
    int pin2; // Connection on the right terminal block of the motor
    int enable;
    void forward();
    void backward();
    void stop();
    void setSpeed(int speed);
    int getSpeed();

private:
    int speed = 0; // Sets inital speed to zero
};
class MotorNetwork
{
public:
    MotorNetwork(Motor left, Motor right);
    Motor left;
    Motor right;
    void forward();
    void backward();
    void left();
    void right();
    void stop();
    void setSpeed(int speed);
    int getSpeed();
};
