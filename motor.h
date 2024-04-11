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
        setSpeed(255);
    }

    void forward()
    {
        //   +----+
        // ^ |    | ^
        // | |    | |
        //   |    |
        //   +----+
        analogWrite(leftMotor.enable, this->speed);
        analogWrite(rightMotor.enable, this->speed);
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
        analogWrite(leftMotor.enable, this->speed);
        analogWrite(rightMotor.enable, this->speed);
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
        analogWrite(leftMotor.enable, this->turnSpeed);
        analogWrite(rightMotor.enable, this->turnSpeed);
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
        analogWrite(leftMotor.enable, this->turnSpeed);
        analogWrite(rightMotor.enable, this->turnSpeed);
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
        this->speed = speed;
        this->turnSpeed = speed * 0.5;
        analogWrite(leftMotor.enable, this->speed);
        analogWrite(rightMotor.enable, this->speed);
    }

    int getSpeed()
    {
        return speed;
    }

private:
    Motor leftMotor;
    Motor rightMotor;
    int speed;
    int turnSpeed;
};
