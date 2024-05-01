#![warn(clippy::pedantic, clippy::nursery)]
#![allow(
    clippy::cast_possible_truncation,
    clippy::cast_sign_loss,
    clippy::cast_precision_loss,
    clippy::too_many_lines,
    clippy::cast_possible_wrap
)]
use std::{
    panic::{set_hook, take_hook},
    sync::{Arc, Mutex},
    thread::spawn,
    time::{Duration, Instant},
};

use inquire::Select;
use pancurses::{
    endwin, init_pair, initscr, noecho, start_color, use_default_colors, Input, Window,
};
use serialport::{available_ports, new, SerialPort};

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
#[derive(Clone, Copy, PartialEq)]
enum Control {
    Forward,
    Backward,
    Left,
    Right,
    SetSpeed(u8),
    Quit,
}

// The baud rate of the serial port
const BAUD_RATE: u32 = 115_200;
const INITIAL_SPEED: u8 = u8::MAX;

fn main() {
    let name = get_port_name();
    // The time of the last input used for debouncing and stopping the robot
    let mut last_input_time = Instant::now();
    let mut speed = INITIAL_SPEED;
    clean_up_on_panic();
    // Open the serial port
    let mut port = new(name, BAUD_RATE)
        .timeout(Duration::from_millis(500))
        .open()
        .unwrap();
    let screen = init_window();
    let data = Arc::new(Mutex::new((false, false, 0)));
    {
        let mut port = port.try_clone().unwrap();
        let data = Arc::clone(&data);
        spawn(move || loop {
            let received = recv_data(&mut port);
            *data.lock().unwrap() = received;
        });
    }
    // Main loop
    loop {
        let (left, right, distance_cm) = { *data.lock().unwrap() };
        let char = screen.getch();
        // Match the input to a control
        let control = char.and_then(|char| match char {
            Input::Character('w') | Input::KeyUp => Some(Control::Forward),
            Input::Character('s') | Input::KeyDown => Some(Control::Backward),
            Input::Character('a') | Input::KeyLeft => Some(Control::Left),
            Input::Character('d') | Input::KeyRight => Some(Control::Right),
            Input::Character(char @ '0'..='9') => {
                let keys = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '0'];
                let key = keys.iter().position(|&k| k == char).unwrap();
                speed = ((key as f32) / 9. * 255.) as u8;
                Some(Control::SetSpeed(speed))
            }
            Input::Character('q') => Some(Control::Quit),
            _ => None,
        });
        // Match the control to a command
        let command = match control {
            Some(Control::Forward) => Command::Forward,
            Some(Control::Backward) => Command::Backward,
            Some(Control::Left) => Command::Left,
            Some(Control::Right) => Command::Right,
            Some(Control::SetSpeed(new_speed)) => Command::SetSpeed(new_speed),
            Some(Control::Quit) => break,
            None => {
                // Stop the robot if no input is received for 500ms
                if last_input_time.elapsed() < Duration::from_millis(500) {
                    continue;
                }
                Command::Stop
            }
        };
        let keys_window = screen.subwin(7, 15, 1, 2).unwrap();
        draw_keys(&keys_window, command);
        let speed_window = screen.subwin(7, 255 / 3 + 3, 1, 19).unwrap();
        draw_speed(&speed_window, speed);
        let car_window = screen.subwin(22, 28, 8, 2).unwrap();
        draw_car(&car_window, command, left, right, distance_cm);
        // If a control is received, update the last input time
        if control.is_some() {
            last_input_time = Instant::now();
        }
        // Write the command to the serial port
        port.write_all(&[command.to_byte()]).unwrap();
        screen.refresh();
    }
    // End the curses window when the loop is exited
    endwin();
}

fn draw_speed(window: &Window, speed: u8) {
    window.clear();
    window.border('|', '|', '-', '-', '+', '+', '+', '+');
    window.mvaddstr(3, 1, "-".repeat(255 / 3 + 1));
    window.color_set(2);
    for line in 1..=5 {
        window.mvaddch(line, i32::from(speed) / 3 + 1, '|');
    }
    window.color_set(0);
}

fn draw_keys(window: &Window, command: Command) {
    window.border('|', '|', '-', '-', '+', '+', '+', '+');
    let draw_key = |window: &Window, x, y| {
        window.mvaddstr(y, x, "+---+");
        window.mvaddstr(y + 1, x, "|   |");
        window.mvaddstr(y + 2, x, "+---+");
    };
    let reset_keys = || {
        window.mvaddstr(1, 5, "+---+");
        window.mvaddstr(2, 5, "|   |");
        window.mvaddstr(3, 1, "+---+---+---+");
        window.mvaddstr(4, 1, "|   |   |   |");
        window.mvaddstr(5, 1, "+---+---+---+");
    };
    match command {
        Command::Forward => {
            reset_keys();
            window.color_set(1);
            draw_key(window, 5, 1);
            window.color_set(0);
        }
        Command::Backward => {
            reset_keys();
            window.color_set(1);
            draw_key(window, 5, 3);
            window.color_set(0);
        }
        Command::Left => {
            reset_keys();
            window.color_set(1);
            draw_key(window, 1, 3);
            window.color_set(0);
        }
        Command::Right => {
            reset_keys();
            window.color_set(1);
            draw_key(window, 9, 3);
            window.color_set(0);
        }
        Command::Stop => {
            reset_keys();
        }
        Command::SetSpeed(_) => {}
    }
}

fn draw_car(window: &Window, command: Command, left: bool, right: bool, distance_cm: u16) {
    let reset = || {
        window.clear();
        window.border('|', '|', '-', '-', '+', '+', '+', '+');
        for (index, line) in [
            r"   [------------]",
            r"         ||",
            r"         ||",
            r"         ||",
            r"         ||",
            r"         ||",
            r"         ||",
            r"        _  _",
            r" ___---|^^^^|---___",
            r"|      |____|      |",
            r"|--+            +--|",
            r"/--| ---------- |--\",
            r"|  | | ------ | |  |",
            r"|  | | |~~~ - | |  |",
            r"\--| | |____< | |--/",
            r"   | | ______ | |",
            r"   | | |||||| | |",
            r"   | | ------ | |",
            r"   | __________ |",
            r"        \__/",
        ]
        .iter()
        .enumerate()
        {
            window.mvaddstr(index as i32 + 1, 4, line);
        }
    };
    reset();
    match command {
        Command::Forward => {
            window.mvaddch(12, 2, '^');
            window.mvaddch(12, 25, '^');
            for line in 13..=15 {
                window.mvaddch(line, 2, '|');
                window.mvaddch(line, 25, '|');
            }
        }
        Command::Backward => {
            window.mvaddch(16, 25, 'v');
            window.mvaddch(16, 2, 'v');
            for line in 13..=15 {
                window.mvaddch(line, 2, '|');
                window.mvaddch(line, 25, '|');
            }
        }
        Command::Left => {
            window.mvaddch(12, 25, '^');
            window.mvaddch(16, 2, 'v');
            for line in 13..=15 {
                window.mvaddch(line, 2, '|');
                window.mvaddch(line, 25, '|');
            }
        }
        Command::Right => {
            window.mvaddch(12, 2, '^');
            window.mvaddch(16, 25, 'v');
            for line in 13..=15 {
                window.mvaddch(line, 2, '|');
                window.mvaddch(line, 25, '|');
            }
        }
        Command::Stop => reset(),
        Command::SetSpeed(_) => {}
    }
    window.mvaddstr(4, 1, format!("{:^26}", format!("{} cm", distance_cm)));
    if left {
        window.color_set(2);
        window.mvaddch(8, 10, '!');
        window.color_set(0);
    }
    if right {
        window.color_set(2);
        window.mvaddch(8, 17, '!');
        window.color_set(0);
    }
}

fn clean_up_on_panic() {
    // Set the panic hook to end the curses window
    let handler = take_hook();
    set_hook(Box::new(move |info| {
        endwin();
        handler(info);
    }));
}

fn get_port_name() -> String {
    // Get the available ports
    let ports = available_ports().unwrap();
    // Prompt the user to select a port
    let name = Select::new(
        "Select a port",
        ports.iter().map(|p| p.port_name.clone()).collect(),
    )
    .prompt()
    .unwrap();
    name
}

fn init_window() -> Window {
    // Initialize the curses window
    let window = initscr();
    // Turn on keypad mode
    window.keypad(true);
    // Turn off echoing
    noecho();
    // Don't wait for input
    window.nodelay(true);
    // Clear the screen
    window.clear();
    start_color();
    use_default_colors();
    init_pair(0, -1, -1);
    init_pair(1, -1, 7);
    init_pair(2, 1, -1);
    window
}

fn recv_data(port: &mut Box<dyn SerialPort>) -> (bool, bool, u16) {
    let mut buffer = [None; 2];
    while !buffer.iter().all(Option::is_some) {
        let mut byte = [0];
        if port.read_exact(&mut byte).is_err() {
            continue;
        };
        let byte = byte[0];
        match byte & 0b0000_0001 {
            kind @ (0 | 1) => {
                buffer[kind as usize] = Some(byte);
            }
            _ => unreachable!(),
        }
    }
    let buffer = u16::from_be_bytes(buffer.map(Option::unwrap));
    let left = (buffer >> 15) == 1;
    let right = ((buffer >> 14) & 1) == 1;
    let distance_last_4_bits = (buffer & 0b1111_0000) >> 4;
    let distance_first_5_bits = (buffer >> 8) & 0b0001_1111;
    let distance_cm = (distance_first_5_bits << 5) | distance_last_4_bits;
    (left, right, distance_cm)
}
