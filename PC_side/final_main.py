# import serial
# import threading

# # Configure your serial port and baud rate
# SERIAL_PORT = 'COM3'  # Or '/dev/ttyUSB0' on Linux
# BAUD_RATE = 9600
# #---------------------------------------------------------------------------------------
# # Acquire the lock (automatically released when exiting the 'with' block)
# # Critical section:  Only one thread can execute this code at a time
# # Perform operations on the shared 'ser'
# #---------------------------------------------------------------------------------------
# lock = threading.Lock()  # Create a lock object

# def read_from_serial(ser):
#     while True:
#         if ser.in_waiting:
#             with lock:
#                 data = ser.read_until(expected=b'\n')
#                 print("\nReceived:", data.decode('utf-8', errors='ignore'),end="")
# #---------------------------------------------------------------------------------------
# def main():
#     try:
#         # Open serial port
#         ser = serial.Serial(SERIAL_PORT, BAUD_RATE, bytesize=serial.EIGHTBITS,
#               parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1)

#         print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")

#         # Start a thread to read incoming data
#         thread = threading.Thread(target=read_from_serial, args=(ser,), daemon=True)
#         thread.start()

#         # Main loop: send data from user input
#         while True:
#             print("Transmit:", end=" ")
#             user_input = input()
#             ser.write(user_input.encode('utf-8'))

#     except serial.SerialException as e:
#         print(f"Error: {e}")
#     except KeyboardInterrupt:
#         print("Exiting...")
#     finally:
#         if 'ser' in locals() and ser.is_open:
#             ser.close()

# if __name__ == '__main__':
#     main()

import serial

def main():
    options = ['1','2','3','4','5','6','7','8']
    flag = 1

    menu = """                            Menu
    1. Counting onto the LCD screen with a delay of X[ms]
    2. Circular tone series via Buzzer with delay of X[ms]
    3. Get delay time X[ms]
    4. Potentiometer 3-digit value [v] onto LCD
    5. On each PB1 pressed, send a Message "I love my Negev"
    6. Clear LCD screen
    7. Show menu
    8. Sleep"""
    print(menu)

    ser = serial.Serial('COM3', 9600, bytesize=8,
                        parity=serial.PARITY_NONE, stopbits=1, timeout=5) #waits 5seconds, otherwise continues

    # clear buffers
    ser.reset_input_buffer()
    ser.reset_output_buffer()       

    while True:  # while the output buffer isn't empty
        if flag:
            choice = input("Select from Menu: ")
        if choice in ['1', '2', '4', '6', '8']:
            ser.write(choice.encode())
            flag=1

        elif choice == '3':
            ser.write(choice.encode())
            delay = input("Enter Delay [ms]: ")
            delay += '\n'
            ser.write(delay.encode())
            flag=1


        elif choice == '5':
            ser.write(choice.encode())
            line = ser.readline().decode()
            print(line[:-1])
            flag=1
            continue

        elif choice == '7':
            print(menu)
            flag=1

        elif choice == '9':
            ser.write(choice.encode())
            in_str = input("Enter a string or other menu option:")
            for option in options:
                if option in in_str:
                    choice = option
                    flag=0
                    continue
            in_str += '\n'
            ser.write(in_str.encode())
            flag=0

        else:
            print('Invalid choice, try again')
            print(menu)




if __name__ == '__main__':
    main()
