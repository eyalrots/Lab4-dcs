# MSP430 Microcontroller Interface

## Description

This project allows a user to interface with an MSP430G2553 microcontroller from a PC via a serial (UART) connection. A Python script provides a command-line menu to control various peripherals connected to the MSP430, such as an LCD display, a buzzer, and to read values from a potentiometer.

## Project Structure

The project is organized into two main parts:

-   **`C_files/`**: This directory contains the C firmware for the MSP430G2553 microcontroller.
    -   `C_files/Lab_4/source/main.c`: The main application code, including a state machine to handle commands.
    -   `C_files/Lab_4/header/`: Header files defining APIs (`api.h`), Board Support Package (`bsp.h`), and Hardware Abstraction Layer for GPIO (`halGPIO.h`).
    -   `C_files/Lab_4/lnk_msp430g2553.cmd`: Linker command file for the MSP430G2553.
    -   `C_files/Lab_4/Debug/`: Contains build outputs (e.g., `.out` file to be flashed to the microcontroller).
    -   `C_files/Lab_4/.ccsproject`, `C_files/Lab_4/.cproject`: Code Composer Studio project files (assumed).
-   **`main.py`**: A Python script that runs on the PC. It provides a menu-driven interface to send commands to the connected MSP430 microcontroller over the serial port (default COM3, 9600 baud).

## Features

The Python interface allows the user to:

1.  Start a counter on the LCD screen with a configurable delay.
2.  Generate a circular tone series via a buzzer with a configurable delay.
3.  Set and get the delay time (X ms) used for features 1 and 2.
4.  Display a 3-digit potentiometer value (voltage) on the LCD.
5.  Send a pre-defined message ("I love my Negev") from the MSP430 upon a button press (PB1) on the microcontroller board.
6.  Clear the LCD screen.
7.  Display the command menu.
8.  Put the MSP430 into a sleep/low-power mode.

## Prerequisites

-   **Hardware:**
    -   MSP430G2553 microcontroller (or a development board like the MSP430 LaunchPad).
    -   Connected peripherals: LCD display, buzzer, potentiometer, pushbuttons.
    -   A way to program the MSP430 (e.g., MSP430 LaunchPad's onboard programmer).
    -   A serial connection between the PC and the MSP430 (e.g., via the MSP430 LaunchPad's application UART).
-   **Software (PC):**
    -   Python 3.x
    -   `pyserial` Python package (`pip install pyserial`)
-   **Software (Microcontroller):**
    -   A C compiler for MSP430 (e.g., TI Code Composer Studio, or MSPGCC).
    -   The firmware from `C_files/Lab_4/` compiled and flashed onto the MSP430G2553.

## How to Use

1.  **Firmware Setup:**
    *   Open the C project (e.g., using Code Composer Studio with the `.ccsproject` file in `C_files/Lab_4/`).
    *   Compile the firmware.
    *   Flash the compiled output (e.g., `C_files/Lab_4/Debug/Lab_4.out`) to the MSP430G2553 microcontroller.

2.  **Python Script:**
    *   Ensure the MSP430 board is connected to the PC and the correct serial port is identified. You might need to change `SERIAL_PORT = 'COM3'` in `main.py` to match your system's serial port (Note: the current `main.py` hardcodes 'COM3' directly in `serial.Serial('COM3', ...)`. This would need to be changed there).
    *   Install the `pyserial` package:
        ```bash
        pip install pyserial
        ```
    *   Run the Python script:
        ```bash
        python main.py
        ```
    *   The script will display a menu. Enter the number corresponding to the desired action.

## Notes

-   The `old_files/` directory is not part of the active project and can be ignored.
-   The C code is structured with a Hardware Abstraction Layer (HAL) and a Board Support Package (BSP) for better portability and organization.
-   The `main.c` on the microcontroller implements a state machine to process incoming serial commands.
