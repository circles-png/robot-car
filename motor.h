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
};
