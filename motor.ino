
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

private:
    int speed = 0;

    Motor(int pin1, int pin2, int enable)
    {
        this->pin1 = pin1;
        this->pin2 = pin2;
        this->enable = enable;
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        pinMode(enable, OUTPUT);
        stop();
    }

    void forward()
    {
        digitalWrite(this->pin1, true);
        digitalWrite(this->pin2, false);
    }

    void backward()
    {
        digitalWrite(this->pin1, false);
        digitalWrite(this->pin2, true);
    }

    void stop()
    {
        digitalWrite(this->pin1, false);
        digitalWrite(this->pin2, false);
    }

    void setSpeed(int speed)
    {
        this->speed = speed;
        analogWrite(enable, speed);
    }

    int getSpeed()
    {
        return this->speed;
    }
};
