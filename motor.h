class Motor
{
public:
    Motor(int pin1, int pin2, int enable)
    {
        this->pin1 = pin1;
        this->pin2 = pin2;
        this->enable = enable;
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        pinMode(enable, OUTPUT);
        setSpeed(255);
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
        this->speed = speed;
        analogWrite(enable, speed);
    }

    int getSpeed()
    {
        return speed;
    }

private:
    int speed;
    int pin1;
    int pin2;
    int enable;
};

class MotorNetwork
{
public:
    MotorNetwork(Motor left, Motor right) : leftMotor(left), rightMotor(right)
    {
        leftMotor = left;
        rightMotor = right;
        stop();
    }

    void forward()
    {
        leftMotor.forward();
        rightMotor.forward();
    }

    void backward()
    {
        leftMotor.backward();
        rightMotor.backward();
    }

    void left()
    {
        leftMotor.backward();
        rightMotor.forward();
    }
    void right()
    {
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
