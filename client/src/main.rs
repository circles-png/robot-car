// Tell clippy to ignore certain warnings
#![warn(clippy::pedantic, clippy::nursery)]
// Allow some warnings
#![allow(
    clippy::cast_possible_truncation,
    clippy::cast_sign_loss,
    clippy::cast_precision_loss,
    clippy::too_many_lines,
    clippy::cast_possible_wrap
)]

// Import standard library stuff
use std::{
    panic::{set_hook, take_hook},
    sync::{Arc, Mutex},
    thread::spawn,
    time::{Duration, Instant},
};

// Import third-party crates
use inquire::Select;
use pancurses::{
    endwin, init_pair, initscr, noecho, start_color, use_default_colors, Input, Window,
};
use serialport::{available_ports, new, SerialPort};

// Represents output commands sent to the robot
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

// Represents the input controls from the user
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
// Initial speed of the robot is 255
const INITIAL_SPEED: u8 = u8::MAX;

fn main() {
    let name = get_port_name();
    // The time of the last input used for debouncing and stopping the robot
    let mut last_input_time = Instant::now();
    // The current speed of the robot
    let mut speed = INITIAL_SPEED;
    // Reset the panic hook so that it cleans up the pancurses screen when panicking
    clean_up_on_panic();
    // Open the serial port
    let mut port = new(name, BAUD_RATE)
        .timeout(Duration::from_millis(500))
        .open()
        .unwrap();
    // Initialize the pancurses window
    let screen = init_window();
    // Make an atomic reference counted (Arc) smart pointer container to hold the mutual exclusion
    // container (Mutex) which then holds the data received from the robot
    let data = Arc::new(Mutex::new((false, false, 0)));
    {
        // Clone the port handle so that it can be moved into the thread
        let mut port = port.try_clone().unwrap();
        // Clone the Arc
        let data = Arc::clone(&data);
        // Spawn a thread to receive data from the serial port
        spawn(move || loop {
            // Receive data from the serial port
            let received = recv_data(&mut port);
            // Lock the data Mutex and put the data in it
            *data.lock().unwrap() = received;
        });
    }
    // Main loop
    loop {
        // Input received
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
        // Make a new window for the directional UI
        let keys_window = screen.subwin(7, 15, 1, 2).unwrap();
        // Draw the directions to the window
        draw_keys(&keys_window, command);
        // Make a new window for the speed UI
        let speed_window = screen.subwin(7, 255 / 3 + 3, 1, 19).unwrap();
        // Draw the speed to the window
        draw_speed(&speed_window, speed);
        // Make a new window for the car UI
        let car_window = screen.subwin(22, 28, 8, 2).unwrap();
        // Draw the car, including ultrasonic and line-following sensors, to the window
        draw_car(&car_window, command, left, right, distance_cm);
        // If a control is received, update the last input time
        if control.is_some() {
            last_input_time = Instant::now();
        }
        // Write the command to the serial port
        port.write_all(&[command.to_byte()]).unwrap();
    }
    // End the curses window when the loop is exited
    endwin();
}

// Draws the speed to the speed window
fn draw_speed(window: &Window, speed: u8) {
    // Clears the window
    window.clear();
    // Draw the border
    window.border('|', '|', '-', '-', '+', '+', '+', '+');
    // Move to (3, 1) relative to the speed window and write some dashes
    window.mvaddstr(3, 1, "-".repeat(255 / 3 + 1));
    // Change to red
    window.color_set(2);
    // For each line in the window, draw a pipe character at the same abscissa
    for line in 1..=5 {
        window.mvaddch(line, i32::from(speed) / 3 + 1, '|');
    }
    // Reset the colour
    window.color_set(0);
}

// Draws the arrow keys to the directional window
fn draw_keys(window: &Window, command: Command) {
    // Draw the border
    window.border('|', '|', '-', '-', '+', '+', '+', '+');
    // Define a function to draw a square at some position
    let draw_key = |window: &Window, x, y| {
        window.mvaddstr(y, x, "+---+");
        window.mvaddstr(y + 1, x, "|   |");
        window.mvaddstr(y + 2, x, "+---+");
    };
    // Define a function to reset all the keys
    let reset_keys = || {
        window.mvaddstr(1, 5, "+---+");
        window.mvaddstr(2, 5, "|   |");
        window.mvaddstr(3, 1, "+---+---+---+");
        window.mvaddstr(4, 1, "|   |   |   |");
        window.mvaddstr(5, 1, "+---+---+---+");
    };
    // Match the command to a key position.
    // None               The keys should not be reset or drawn
    // Some(None)         The keys should be reset, but not drawn
    // Some(Some(x, y))   The keys should be reset and drawn at (x, y)
    let Some(inner) = (match command {
        Command::Forward => Some(Some((5, 1))),
        Command::Backward => Some(Some((5, 3))),
        Command::Left => Some(Some((1, 3))),
        Command::Right => Some(Some((9, 3))),
        Command::Stop => Some(None),
        Command::SetSpeed(_) => None,
    }) else {
        return;
    };
    // Reset the keys
    reset_keys();
    let Some((x, y)) = inner else {
        return;
    };
    // Draw the key on a black foreground and a white background
    window.color_set(1);
    draw_key(window, x, y);
    window.color_set(0);
}

fn draw_car(window: &Window, command: Command, left: bool, right: bool, distance_cm: u16) {
    // Define a reset function to clear the car window
    let reset = || {
        // Clear the window
        window.clear();
        // Draw a border
        window.border('|', '|', '-', '-', '+', '+', '+', '+');
        // For each index and line of the image, draw it to the window
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
    // Call reset to reset the window
    reset();
    // Match the command to a drawing
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
    // Write the distance to the window
    window.mvaddstr(4, 1, format!("{:^26}", format!("{} cm", distance_cm)));
    // Display the status of the infrared sensors
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
    // Create a buffer of two bytes
    let mut buffer = [None; 2];
    // While the buffer is not full, read some data into it
    while !buffer.iter().all(Option::is_some) {
        // Make a buffer of one byte
        let mut byte = [0];
        // Read from the port into the buffer
        if port.read_exact(&mut byte).is_err() {
            continue;
        };
        // Take the byte out of the array
        let byte = byte[0];
        // Match the last bit and write into the two byte buffer accordingly
        match byte & 0b0000_0001 {
            kind @ (0 | 1) => {
                buffer[kind as usize] = Some(byte);
            }
            _ => unreachable!(),
        }
    }
    // Make a u16 out of the bytes
    let buffer = u16::from_be_bytes(buffer.map(Option::unwrap));
    // Decode the data out of the buffer
    let left = (buffer >> 15) == 1;
    let right = ((buffer >> 14) & 1) == 1;
    let distance_last_4_bits = (buffer & 0b1111_0000) >> 4;
    let distance_first_5_bits = (buffer >> 9) & 0b0001_1111;
    let distance_cm = (distance_first_5_bits << 5) | distance_last_4_bits;
    (left, right, distance_cm)
}
