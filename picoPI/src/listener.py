import serial

# Set the serial port and baud rate
port = '/dev/ttyACM0'  # Change this if necessary
baud_rate = 115200

# Open the serial port
ser = serial.Serial(port, baud_rate)

# Set a timeout to avoid blocking forever
ser.timeout = 1  # 1 second timeout for reading

print("Listening for messages...")

# Continuously listen for messages
while True:
    # Read data from the serial port
    if ser.in_waiting > 0:  # Check if data is available to read
        message = ser.readline().decode('utf-8').strip()  # Read the message and decode
        print(f"Received message: {message}")
