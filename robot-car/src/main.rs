#![warn(clippy::pedantic, clippy::nursery)]
#![no_std]
#![no_main]

pub mod motor;

use arduino_hal::{default_serial, simple_pwm::{IntoPwmPin, Prescaler, Timer0Pwm}};
use motor::{Motor, Network};
use panic_halt as _;
use ufmt::uwriteln;

#[arduino_hal::entry]
fn main() -> ! {
    let dp = arduino_hal::Peripherals::take().unwrap();
    let pins = arduino_hal::pins!(dp);

    let timer = Timer0Pwm::new(dp.TC0, Prescaler::Prescale64);

    let mut network = Network::new(
        Motor::new(
            pins.d11.into_output().downgrade(),
            pins.d10.into_output().downgrade(),
            pins.d6.into_output().into_pwm(&timer),
        ),
        Motor::new(
            pins.d9.into_output().downgrade(),
            pins.d8.into_output().downgrade(),
            pins.d5.into_output().into_pwm(&timer),
        ),
    );

    let mut serial = default_serial!(dp, pins, 115_200);

    loop {
        uwriteln!(serial, "Forward").unwrap();
        network.forward();
        arduino_hal::delay_ms(2000);

        uwriteln!(serial, "Backward").unwrap();
        network.backward();
        arduino_hal::delay_ms(2000);

        uwriteln!(serial, "Stop").unwrap();
        network.stop();
    }
}
