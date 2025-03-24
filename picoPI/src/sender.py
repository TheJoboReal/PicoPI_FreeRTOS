import serial
import time

port = '/dev/ttyACM0' 
baud_rate = 115200

# Open the serial port
ser = serial.Serial(port, baud_rate, timeout=1)
time.sleep(2)

# Command identifiers
DRV_COMMAND = 1
STOP_COMMAND = 2
STATUS_COMMAND = 3

def send_command(command_id, *args):
    """Send the command id followed by any necessary parameters."""
    command = f"{command_id} " + " ".join(map(str, args)) + "\n"
    ser.write(command.encode())
    print(f"Sent command: {command}")

while True:
    # Get input from the user
    print("\nSelect a command:")
    print("1: DRV (direction, speed, interval)")
    print("2: STOP")
    print("3: STATUS")
    
    command = input("Enter command (1/2/3): ").strip()
    
    if command == "1":
        direction = input("Enter direction (F/B): ").strip().upper()
        speed = input("Enter speed (0-100): ").strip()
        interval = input("Enter interval (number of steps): ").strip()
        send_command(DRV_COMMAND, direction, speed, interval)
    
    elif command == "2":
        send_command(STOP_COMMAND)
    
    elif command == "3":
        send_command(STATUS_COMMAND)
    
    elif command.lower() == "exit":
        break

ser.close()
