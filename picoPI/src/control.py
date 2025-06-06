import curses
import serial
import time

# === Configuration ===
port = '/dev/ttyACM0'  # Update as needed
baud_rate = 115200

# Send command function
def send_command(ser, cmd):
    wrapped_cmd = f"0000 {cmd} 0000\n"
    ser.write(wrapped_cmd.encode())
    ser.flush()
    return wrapped_cmd.strip()

# The control loop
def control_loop(stdscr, ser):
    stdscr.clear()
    stdscr.nodelay(True)  # Don't block on key press
    stdscr.addstr(0, 0, "Robot Controller (Vim-style keys)")
    stdscr.addstr(1, 0, "Use: h/a (left), j/s (back), k/w (forward), l/d (right), q (quit)")

    # === Commands Mapping ===
    while True:
        key = stdscr.getch()

        if key == ord('q'):
            break
        elif key == ord('k') or key == ord('w'):  # Forward
            cmd = send_command(ser, "1 F 40 500")
            stdscr.addstr(3, 0, f"Sent: {cmd}         ")
        elif key == ord('j') or key == ord('s'):  # Backward
            cmd = send_command(ser, "1 B 40 500")
            stdscr.addstr(3, 0, f"Sent: {cmd}         ")
        elif key == ord('h') or key == ord('a'):  # Left
            cmd = send_command(ser, "2 L 40 400")
            stdscr.addstr(3, 0, f"Sent: {cmd}         ")
        elif key == ord('l') or key == ord('d'):  # Right
            cmd = send_command(ser, "2 R 40 400")
            stdscr.addstr(3, 0, f"Sent: {cmd}         ")
        elif key == ord(' '):  # Spacebar for stop
            cmd = send_command(ser, "0")
            stdscr.addstr(3, 0, f"Sent: {cmd}         ")

        stdscr.refresh()
        time.sleep(0.1)

def main():
    try:
        ser = serial.Serial(port, baud_rate, timeout=1)
        time.sleep(2)  # Wait for serial connection to initialize


        command = "0000 5 0000\n"  # Example command
        ser.write(command.encode())  # Send command over USB serial
        ser.flush()  # Ensure data is sent immediately


        send_command(ser, "5")  # USB init command
        curses.wrapper(control_loop, ser)
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    main()
