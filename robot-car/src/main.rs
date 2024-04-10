#![warn(clippy::pedantic, clippy::nursery)]
#![no_std]
#![no_main]

pub mod motor;

use arduino_hal::{
    default_serial, pins,
    simple_pwm::{IntoPwmPin, Prescaler, Timer0Pwm},
    Peripherals,
};
use motor::{Motor, Network};
use panic_halt as _;

#[arduino_hal::entry]
fn main() -> ! {
    // Get the peripherals and pins
    let dp = Peripherals::take().unwrap();
    let pins = pins!(dp);

    // Create a timer for PWM
    let timer = Timer0Pwm::new(dp.TC0, Prescaler::Prescale64);

    // Create a network of motors
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

    // Create a serial port
    let mut serial = default_serial!(dp, pins, 115_200);

    loop {
        // Read a byte from the serial port
        let command = serial.read_byte();
        // Match the command
        match command & 0b0000_0111 {
            0b000 => network.forward(),
            0b001 => network.backward(),
            0b010 => network.left(),
            0b011 => network.right(),
            0b100 => network.stop(),
            0b101 => network.set_speed(command & 0b1111_1000),
            _ => {}
        }
    }
}
