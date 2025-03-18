import serial
import time

# Set the serial port and baud rate
port = '/dev/ttyACM0'  # Change this if necessary
baud_rate = 115200

# Open the serial port
ser = serial.Serial(port, baud_rate, timeout=1)
time.sleep(2)  # Give the connection some time to establish

def send_message(message):
    """Sends a message to the Pico via serial."""
    ser.write((message + '\n').encode())  # Encode message as bytes and send
    print(f"Sent: {message}")

# Example usage
while True:
    msg = input("Enter message to send: ")  # User input
    send_message(msg)
    if msg.lower() == "exit":
        break

ser.close()  # Close the serial connection when done
