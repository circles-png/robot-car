// This class represents a motor
class Motor
{
public:
    Motor(int pin1, int pin2, int enable); // Declares the pin and enable integers
    int pin1; // Connection on the left terminal block of the motor
    int pin2; // Connection on the right terminal block of the motor
    int enable; // Turns the motor on or off, used for speed control
    void forward(); // Moves the motor forward
    void backward(); // Moves the motor backward
    void stop(); // Stops motor
    void setSpeed(int speed);
    int getSpeed();

private:
    int speed = 0; // Sets inital speed to zero
};
