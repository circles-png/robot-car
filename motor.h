// This class represents a motor
class Motor
{
public:
    Motor(int pin1, int pin2, int enable); // Declares the pin and enable integers
    int pin1;                              // Connection on the left terminal block of the motor
    int pin2;                              // Connection on the right terminal block of the motor
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
