use arduino_hal::{
    port::{
        mode::{Output, PwmOutput},
        Pin,
    },
    simple_pwm::{PwmPinOps, Timer0Pwm},
};

pub struct Motor<EnablePin>
where
    EnablePin: PwmPinOps<Timer0Pwm>,
{
    pin1: Pin<Output>,
    pin2: Pin<Output>,
    enable: Pin<PwmOutput<Timer0Pwm>, EnablePin>,
    speed: u8,
}

impl<EnablePin> Motor<EnablePin>
where
    EnablePin: PwmPinOps<Timer0Pwm>,
{
    #[must_use]
    pub fn new(
        pin1: Pin<Output>,
        pin2: Pin<Output>,
        mut enable: Pin<PwmOutput<Timer0Pwm>, EnablePin>,
    ) -> Self {
        enable.enable();
        let mut motor = Self {
            pin1,
            pin2,
            enable,
            speed: 0,
        };
        motor.stop();
        motor
    }

    pub fn forward(&mut self) {
        self.pin1.set_low();
        self.pin2.set_high();
    }

    pub fn backward(&mut self) {
        self.pin1.set_high();
        self.pin2.set_low();
    }

    pub fn stop(&mut self) {
        self.pin1.set_low();
        self.pin2.set_low();
    }

    pub const fn get_speed(&self) -> u8 {
        self.speed
    }

    pub fn set_speed(&mut self, speed: u8) {
        self.speed = speed;
        self.enable.set_duty(speed);
    }
}

pub struct Network<LeftEnable, RightEnable>
where
    LeftEnable: PwmPinOps<Timer0Pwm>,
    RightEnable: PwmPinOps<Timer0Pwm>,
{
    left: Motor<LeftEnable>,
    right: Motor<RightEnable>,
}

impl<LeftEnable, RightEnable> Network<LeftEnable, RightEnable>
where
    LeftEnable: PwmPinOps<Timer0Pwm>,
    RightEnable: PwmPinOps<Timer0Pwm>,
{
    pub const fn new(left: Motor<LeftEnable>, right: Motor<RightEnable>) -> Self {
        Self { left, right }
    }

    pub fn forward(&mut self) {
        self.left.forward();
        self.right.forward();
    }

    pub fn backward(&mut self) {
        self.left.backward();
        self.right.backward();
    }

    pub fn left(&mut self) {
        self.left.backward();
        self.right.forward();
    }

    pub fn right(&mut self) {
        self.left.forward();
        self.right.backward();
    }

    pub fn stop(&mut self) {
        self.left.stop();
        self.right.stop();
    }

    pub fn set_speed(&mut self, speed: u8) {
        self.left.set_speed(speed);
        self.right.set_speed(speed);
    }
}
