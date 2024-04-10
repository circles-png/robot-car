// Represents a motor in a network
class Motor
{
public:
    Motor(int pin1, int pin2, int enable)
    {
        // Save the pins
        this->pin1 = pin1;
        this->pin2 = pin2;
        this->enable = enable;
        // Set the pins as outputs
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        pinMode(enable, OUTPUT);
        // Initialise the speed to 0
        setSpeed(0);
        // Stop the motor
        stop();
    }

    void forward()
    {
        digitalWrite(pin1, true);
        digitalWrite(pin2, false);
    }

    void backward()
    {
        digitalWrite(pin1, false);
        digitalWrite(pin2, true);
    }

    void stop()
    {
        digitalWrite(pin1, false);
        digitalWrite(pin2, false);
    }

    void setSpeed(int speed)
    {
        // Save the speed
        this->speed = speed;
        // Set the speed using PWM
        analogWrite(enable, speed);
    }

    int getSpeed()
    {
        // Return the speed
        return speed;
    }

private:
    // The speed of the motor
    int speed;
    // Pins for the motor
    int pin1;
    int pin2;
    int enable;
};

// Represents a network of motors
class MotorNetwork
{
public:
    MotorNetwork(Motor left, Motor right) : leftMotor(left), rightMotor(right)
    {
        leftMotor = left;
        rightMotor = right;
    }

    void forward()
    {
        //   +----+
        // ^ |    | ^
        // | |    | |
        //   |    |
        //   +----+
        leftMotor.forward();
        rightMotor.forward();
    }

    void backward()
    {
        //   +----+
        //   |    |
        // | |    | |
        // v |    | v
        //   +----+
        leftMotor.backward();
        rightMotor.backward();
    }

    void left()
    {
        //   +----+
        //   |    | ^
        // | |    | |
        // v |    |
        //   +----+
        leftMotor.backward();
        rightMotor.forward();
    }
    void right()
    {
        //   +----+
        // ^ |    |
        // | |    | |
        //   |    | v
        //   +----+
        leftMotor.forward();
        rightMotor.backward();
    }

    void stop()
    {
        leftMotor.stop();
        rightMotor.stop();
    }

    void setSpeed(int speed)
    {
        leftMotor.setSpeed(speed);
        rightMotor.setSpeed(speed);
    }

    int getSpeed()
    {
        return leftMotor.getSpeed();
    }

private:
    Motor leftMotor;
    Motor rightMotor;
};
