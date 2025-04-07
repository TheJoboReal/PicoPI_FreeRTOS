import serial
import time

port = '/dev/ttyACM0'  # Ensure this is correct for your device
baud_rate = 115200

# Open the serial port
ser = serial.Serial(port, baud_rate, timeout=1)
time.sleep(2)  # Allow time for the serial connection to establish

# Send the command with a newline
command = "0000 9 0000\n"  # Example command

ser.write(command.encode())  # Send command over USB serial
ser.flush()  # Ensure data is sent immediately

time.sleep(2)

ser.write(command.encode())  # Send command over USB serial
ser.flush()  # Ensure data is sent immediately

print(f"Sent command: {command.strip()}")  # Debugging output

# Send the command with a newline
command = "0000 1 F 40 9000 0000\n"  # Example command

ser.write(command.encode())  # Send command over USB serial
ser.flush()  # Ensure data is sent immediately

print(f"Sent command: {command.strip()}")  # Debugging output

time.sleep(1)

# Send the command with a newline
command = "0000 0 0000\n"  # Example command

ser.write(command.encode())  # Send command over USB serial
ser.flush()  # Ensure data is sent immediately

print(f"Sent command: {command.strip()}")  # Debugging output

ser.close()