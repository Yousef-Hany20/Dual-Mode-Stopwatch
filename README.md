# AVR Stopwatch System with 7-Segment Display

## Overview
This project implements a stopwatch system on the **ATmega32** microcontroller. It allows the user to increment/decrement **hours**, **minutes**, and **seconds**, toggle between **count up** and **count down** modes, and control the stopwatch using **external interrupts**. The current time is displayed on **6 multiplexed 7-segment displays**.

## Features
- Real-time stopwatch display (HH:MM:SS)
- Count Up / Count Down modes with mode toggle
- Time adjustment using push buttons (increment/decrement each unit)
- Pause, Resume, and Reset functions via external interrupts (INT0, INT1, INT2)
- Visual feedback with **Red** (count up) and **Yellow** (count down) LEDs
- **Buzzer alert** when countdown reaches `00:00:00`
- Debounced button input handling

## Microcontroller
- **ATmega32** (16 MHz clock)

## Drivers Used
- **GPIO**
- **Timer1** in **CTC mode**
- **External Interrupts (INT0, INT1, INT2)**
- **7-Segment Display Multiplexing**

## Hardware Connections

| Component       | Port/Pin     | Description                     |
|----------------|--------------|---------------------------------|
| 7-Segment Ctrl  | PORTA[0:5]   | Digit Enable Signals (6 digits)|
| 7-Segment Data  | PORTC[0:3]   | BCD Data Output                 |
| Time Buttons    | PORTB[0,1,3,4,5,6] | Hour/Min/Sec Inc & Dec     |
| Mode Toggle     | PORTB7       | Toggle count mode               |
| INT0            | PD2          | Reset timer                     |
| INT1            | PD3          | Pause timer                     |
| INT2            | PD6          | Resume timer                    |
| LED Up/Down     | PD4 (Red), PD5 (Yellow) | Count mode indication  |
| Buzzer          | PD0          | Alarm when time ends            |

## Button Assignments

| Button Pin | Function            |
|------------|---------------------|
| PB0        | Decrement Hours     |
| PB1        | Increment Hours     |
| PB3        | Decrement Minutes   |
| PB4        | Increment Minutes   |
| PB5        | Decrement Seconds   |
| PB6        | Increment Seconds   |
| PB7        | Toggle Count Mode   |

## Interrupt Assignments

| Interrupt | Pin  | Function         |
|-----------|------|------------------|
| INT0      | PD2  | Reset Time       |
| INT1      | PD3  | Pause Stopwatch  |
| INT2      | PD6  | Resume Stopwatch |


## How It Works
- Timer1 generates a **1-second interrupt** using **CTC mode** with a prescaler of 1024.
- On each interrupt, the system either increments or decrements the time based on the selected mode.
- The display is refreshed continuously using multiplexed outputs.
- Buttons are debounced in software using delays and state flags.
- External interrupts provide responsive control over stopwatch operation.

## Author
**Yousef** – Developed as part of the Embedded Systems Diploma.

## License
This project is open source under the MIT License.
