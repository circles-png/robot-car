#![warn(clippy::pedantic, clippy::nursery)]
use std::{
    panic::{set_hook, take_hook},
    time::{Duration, Instant},
};

use inquire::Select;
use pancurses::{endwin, initscr, noecho, Input};
use serialport::{available_ports, new};

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
    const fn to_byte(self) -> u8 {
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
    const BAUD_RATE: u32 = 115_200;
    let ports = available_ports().unwrap();
    let name = Select::new(
        "Select a port",
        ports.iter().map(|p| p.port_name.clone()).collect(),
    )
    .prompt()
    .unwrap();

    let window = initscr();
    window.keypad(true);
    noecho();
    window.nodelay(true);
    let mut speed: u8 = 0;
    let mut last_input_time = Instant::now();
    let handler = take_hook();
    set_hook(Box::new(move |info| {
        endwin();
        handler(info);
    }));
    let mut port = new(name, BAUD_RATE)
        .timeout(Duration::from_millis(500))
        .open()
        .unwrap();
    loop {
        let char = window.getch();
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
        let command = match control {
            Some(Control::Forward) => Command::Forward,
            Some(Control::Backward) => Command::Backward,
            Some(Control::Left) => Command::Left,
            Some(Control::Right) => Command::Right,
            Some(Control::Faster) => {
                speed = speed.saturating_add(10);
                Command::SetSpeed(speed)
            }
            Some(Control::Slower) => {
                speed = speed.saturating_sub(10);
                Command::SetSpeed(speed)
            }
            Some(Control::Quit) => break,
            None => {
                if last_input_time.elapsed() < Duration::from_millis(500) {
                    continue;
                }
                Command::Stop
            }
        };
        if control.is_some() {
            last_input_time = Instant::now();
        }
        window.clear();
        window.refresh();
        port.write_all(&[command.to_byte()]).unwrap();
    }
    endwin();
}
