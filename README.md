# MSP430 Microcontroller Interface

## Description

This project allows a user to interface with an MSP430G2553 microcontroller from a PC via a serial (UART) connection. A Python script (`main.py`) provides a command-line menu to send commands to the MSP430. The firmware on the microcontroller then executes these commands, controlling peripherals like an LCD display and a buzzer, reading sensor values, and communicating back to the PC.

## Project Structure

The project is organized into two main parts:

-   **`C_files/`**: This directory contains the C firmware for the MSP430G2553 microcontroller.
    -   `C_files/Lab_4/source/`: Contains the C source code.
    -   `C_files/Lab_4/header/`: Contains the C header files.
    -   `C_files/Lab_4/lnk_msp430g2553.cmd`: Linker command file.
    -   `C_files/Lab_4/Debug/`: Build output directory.
    -   `C_files/Lab_4/.ccsproject`, `C_files/Lab_4/.cproject`: Code Composer Studio project files.
-   **`main.py`**: A Python script that runs on the PC, providing a menu to interact with the MSP430.

## C Code Explanation

The firmware for the MSP430 is modular, divided into several C files:

### `C_files/Lab_4/source/main.c`
-   **Role**: This is the main entry point and central control flow for the microcontroller firmware.
-   **Functionality**:
    -   Initializes the system by calling `system_config()`.
    -   Initializes the LCD using `lcd_init()` and clears it.
    -   Enters an infinite loop (`while(1)`) that contains a state machine (`switch (state)`).
    -   The `state` variable (of type `FSM_state_t`) dictates the current operation of the microcontroller. This state is primarily changed by commands received via UART (handled in `USCI0RX_ISR` in `halGPIO.c`).
    -   Based on the current `state`, it calls appropriate functions from `api.c` (e.g., `count_on_LCD()`, `start_buzzing()`, `disp_pot_val()`).
    -   Manages Low Power Mode (LPM) entry when idle (`state0`).

### `C_files/Lab_4/source/api.c`
-   **Role**: Implements the high-level application programming interface (API) functions that define the core features of the device. These functions are called from `main.c` based on the current state.
-   **Functionality**:
    -   `count_on_LCD()`: Displays an incrementing counter on the LCD. Uses Timer A0 for periodic updates.
    -   `start_buzzing()`: Activates a sequence of tones on the buzzer. Uses Timer A0 for delays between tones and Timer A1 for tone generation.
    -   `x_to_int()`: Converts a string of digits (received via UART, representing a delay in ms) into an integer (`X`) and configures Timer A0 with this delay.
    -   `disp_pot_val()`: Reads the analog value from a potentiometer using the ADC, converts it to a voltage representation, and displays it on the LCD.
    -   `my_negev()`: Waits for a pushbutton (PB1) press. Upon detection (flagged by an ISR), it enables UART transmission to send the string "I love my Negev" to the PC.
    -   `zero_all()`: Clears the LCD screen and resets the counter variable.

### `C_files/Lab_4/source/bsp.c` (Board Support Package)
-   **Role**: Configures the low-level hardware peripherals of the MSP430G2553 microcontroller. This file sets up the microcontroller's pins and internal modules for their specific roles in the project.
-   **Functionality**:
    -   `__GPIO_config()`: Configures General Purpose I/O pins. Sets directions (input/output) and functions (e.g., GPIO, Timer PWM output, ADC input, UART TX/RX) for pins connected to the LCD, pushbuttons, potentiometer, and buzzer. Also enables global interrupts.
    -   `__timerA0_config()`: Configures Timer A0. Sets its clock source, division, and enables its interrupt. This timer is typically used for periodic events or delays.
    -   `__timerA1_config()`: Configures Timer A1. Sets its clock source and output mode, often used for PWM signal generation (e.g., for the buzzer).
    -   `__adc_config()`: Configures the ADC10 module. Sets reference voltage, sample and hold time, clock source, and enables the analog input channel connected to the potentiometer (P1.3).
    -   `__UART_config()`: Configures the USCI_A0 module for serial communication (UART). Sets pin functions for RXD (P1.1) and TXD (P1.2), clock source, and baud rate (9600). Enables UART receive interrupt.

### `C_files/Lab_4/source/halGPIO.c` (Hardware Abstraction Layer)
-   **Role**: Provides a hardware abstraction layer, offering functions that simplify interaction with the microcontroller's peripherals (GPIO, Timers, ADC, UART) and contains Interrupt Service Routines (ISRs).
-   **Functionality**:
    -   `system_config()`: A top-level function that calls all configuration routines from `bsp.c`.
    -   Delay Functions (`delay`, `DelayUs`, `DelayMs`): Provide software-based blocking delays.
    -   `enterLPM()`: Function to put the microcontroller into various Low Power Modes.
    -   Interrupt Control (`enable_interrupts`, `disable_interrupts`): Global interrupt enable/disable.
    -   LCD Control Functions (`lcd_cmd`, `lcd_data`, `lcd_puts`, `lcd_init`, `lcd_strobe`, `print_b`, `print_num`): Low-level functions to initialize and control the LCD, send commands, write data/strings, and print formatted numbers.
    -   Timer Control (`timer_enable`, `timer_disable`, `set_timer_value`): Functions to start/stop Timer A0 and set its period based on the value `X`.
    -   ADC Control (`adc_start_conversion`, `adc_stop_conversion`, `pot_read`): Functions to start and stop ADC conversions and retrieve the last converted potentiometer value.
    -   UART Control (`uart_tx_enable`, `uart_tx_disable`, `uart_rx_enable`, `uart_rx_disable`): Functions to enable/disable UART transmit and receive interrupts.
    -   **Interrupt Service Routines (ISRs)**:
        -   `ADC_handler()`: Triggered upon ADC conversion completion; stores the result.
        -   `timerA0_handler()`: Triggered on Timer A0 overflow. Used to increment the LCD counter (`state1`) or switch buzzer tones (`state2`).
        -   `PBs_handler()`: Triggered by a pushbutton press on Port 1 (used for PB1). It debounces the signal, sets a flag (`PB1_pressed`), and exits LPM.
        -   `USCI0TX_ISR()`: Triggered when the UART transmit buffer is empty. Sends the "I love my Negev" string character by character.
        -   `USCI0RX_ISR()`: Triggered when a byte is received via UART. It either collects digits for the delay value `X` or changes the main `state` based on the command received from the Python script. Exits LPM.

## How to Use

1.  **Firmware Setup:**
    *   Open the C project (e.g., using Code Composer Studio with the `.ccsproject` file in `C_files/Lab_4/`).
    *   Compile the firmware.
    *   Flash the compiled output (e.g., `C_files/Lab_4/Debug/Lab_4.out`) to the MSP430G2553 microcontroller.

2.  **Python Script:**
    *   Ensure the MSP430 board is connected to the PC and the correct serial port is identified. You might need to change the serial port in `main.py` (hardcoded as 'COM3') to match your system.
    *   Install the `pyserial` package:
        ```bash
        pip install pyserial
        ```
    *   Run the Python script:
        ```bash
        python main.py
        ```
    *   The script will display a menu. Enter the number corresponding to the desired action.
