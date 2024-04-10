#![warn(clippy::pedantic, clippy::nursery)]
use std::{
    panic::{set_hook, take_hook},
    time::{Duration, Instant},
};

use inquire::Select;
use pancurses::{endwin, initscr, noecho, Input};
use serialport::{available_ports, new};

// Represents commands that can be sent to the robot
#[derive(Clone, Copy, PartialEq)]
enum Command {
    Forward,
    Backward,
    Left,
    Right,
    Stop,
    SetSpeed(u8),
}

impl Command {
    // Converts the command to a byte
    const fn to_byte(self) -> u8 {
        // 3 least significant bits are used to represent the command
        // For SetSpeed, the other bits are used to represent the speed
        match self {
            Self::Forward => 0b0000_0000,
            Self::Backward => 0b0000_0001,
            Self::Left => 0b0000_0010,
            Self::Right => 0b0000_0011,
            Self::Stop => 0b0000_0100,
            Self::SetSpeed(speed) => 0b0000_0101 | (speed & 0b1111_1000),
        }
    }
}

// Represents the controls from the user
enum Control {
    Forward,
    Backward,
    Left,
    Right,
    Faster,
    Slower,
    Quit,
}

fn main() {
    // The baud rate of the serial port
    const BAUD_RATE: u32 = 115_200;
    // Get the available ports
    let ports = available_ports().unwrap();
    // Prompt the user to select a port
    let name = Select::new(
        "Select a port",
        ports.iter().map(|p| p.port_name.clone()).collect(),
    )
    .prompt()
    .unwrap();
    // Initialize the curses window
    let window = initscr();
    // Turn on keypad mode
    window.keypad(true);
    // Turn off echoing
    noecho();
    // Don't wait for input
    window.nodelay(true);
    // The speed of the robot (PWM duty cycle)
    let mut speed: u8 = 0;
    // The time of the last input used for debouncing and stopping the robot
    let mut last_input_time = Instant::now();
    // Set the panic hook to end the curses window
    let handler = take_hook();
    set_hook(Box::new(move |info| {
        endwin();
        handler(info);
    }));
    // Open the serial port
    let mut port = new(name, BAUD_RATE)
        .timeout(Duration::from_millis(500))
        .open()
        .unwrap();
    // Main loop
    loop {
        // Get the input from the user
        let char = window.getch();
        // Match the input to a control
        let control = char.and_then(|char| match char {
            Input::Character('w') | Input::KeyUp => Some(Control::Forward),
            Input::Character('s') | Input::KeyDown => Some(Control::Backward),
            Input::Character('a') | Input::KeyLeft => Some(Control::Left),
            Input::Character('d') | Input::KeyRight => Some(Control::Right),
            Input::Character('i') => Some(Control::Faster),
            Input::Character('k') => Some(Control::Slower),
            Input::Character('q') => Some(Control::Quit),
            _ => None,
        });
        // Match the control to a command
        let command = match control {
            Some(Control::Forward) => Command::Forward,
            Some(Control::Backward) => Command::Backward,
            Some(Control::Left) => Command::Left,
            Some(Control::Right) => Command::Right,
            Some(Control::Faster) => {
                // Increase the speed by 10, saturating at 255 (u8::MAX)
                speed = speed.saturating_add(10);
                Command::SetSpeed(speed)
            }
            Some(Control::Slower) => {
                // Decrease the speed by 10, saturating at 0
                speed = speed.saturating_sub(10);
                Command::SetSpeed(speed)
            }
            Some(Control::Quit) => break,
            None => {
                // Stop the robot if no input is received for 500ms
                if last_input_time.elapsed() < Duration::from_millis(500) {
                    continue;
                }
                Command::Stop
            }
        };
        // If a control is received, update the last input time
        if control.is_some() {
            last_input_time = Instant::now();
        }
        // Clear the window and refresh
        window.clear();
        window.refresh();
        // Write the command to the serial port
        port.write_all(&[command.to_byte()]).unwrap();
    }
    // End the curses window when the loop is exited
    endwin();
}
