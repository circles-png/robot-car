class Motor
{
public:
    Motor(int pin1, int pin2, int enable);
    int pin1;
    int pin2;
    int enable;
    void forward();
    void backward();
    void stop();
    void setSpeed(int speed);
    int getSpeed();

    Motor(int pin1, int pin2, int enable)
    {
        pin1 = pin1;
        pin2 = pin2;
        enable = enable;
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        pinMode(enable, OUTPUT);
        setSpeed(1023);
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
        speed = speed;
        analogWrite(enable, speed);
    }

    int getSpeed()
    {
        return speed;
    }

private:
    int speed = 0;
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

    MotorNetwork(Motor left, Motor right): left(left), right(right)
    {
        left = left;
        right = right;
        stop();
    }

    void forward()
    {
        left.forward();
        right.forward();
    }

    void backward()
    {
        left.backward();
        right.backward();
    }

    void left()
    {
        left.backward();
        right.forward();
    }

    void right()
    {
        left.forward();
        right.backward();
    }

    void stop()
    {
        left.stop();
        right.stop();
    }

    void setSpeed(int speed)
    {
        left.setSpeed(speed);
        right.setSpeed(speed);
    }

    int getSpeed()
    {
        return left.getSpeed();
    }
};
