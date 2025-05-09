# Intro
This is a self study project with the goal of creating a framework that can be used for testing or used in teaching. 
The project is an OS written in C with FreeRTOS that allows one to control a Pico PI W that is mounted on the linefollower robots used in by robtek on SDU. The OS is written, with the ability to easely add functionality in mind. 

## Setup
To run the code you must first run the [setup.sh](picoPI/setup.sh): the script pulls the needed libraries and sets up paths for your environment. Next, run the [cmake.sh](picoPI/cmake.sh) script to set up the build folder. Now run the [compile.sh](picoPI/compile.sh) script to make the binary files: this script should also be run when you make changes to the code. Lastly, plug the usb cable into the pico pi while holding down the reset button, the pi should now be in flash mode and be discoverable as a media, run the [run.sh](picoPI/run.sh) script to falsh the pico. The pico should now run the code that you compiled.

To see the messages send by the pico, run the following code in a terminal:
Install screen:
```bash
sudo apt install screen
```
Next run this:
```bash
sudo screen /dev/ttyACM0 115200
```
This will open a screen where the messages sent by the pico will stream.

## How the code works
The OS works by listening on the serial port for command strings sent by any other program from any device. The command strings are recived by the pico pi and then handled by intern tasks. This allows for simple communication between any device and the Pico.

### Messages
Messages are on the following form:
```bash
    0000 CMD CMD_DATA 0000
```
The message must start and end with the flags (the four zeroes) for the OS to know when a message starts and ends. The CMD is the **[command](#commands)** the pico must execute, this could be drive forward/backward or send sensor data back the the operator. The **CMD_DATA** is the parameters that a command might need, such as direction to drive, PWM percentage and amount of microsteps. The following is an example command string that tells the robot to move forward in $500$ microsteps with $25\%$ PWM:
```bash
    0000 1 F 25 500 0000
```

### Tasks
There are two main tasks running on the pico: `vReceiverTask` and `vCommandRunTask`. The `vReceiverTask` listens to the serial port every $100$ tics, if any message is being sent, the task first checks if the message is long enough to be valid, it then checks the start and end flags and removes them if they are correct. When the flags have been removed, the stripped message is send a `commandQueue`, the task then goes back to listening on the serial. The `vCommandRunTask` takes the commands out of the queue and strips the command out of the message to find the command that should be run, this is done with a switch statement. The command then calls a function with the **CMD_DATA** as input to execute the chosen command.

### Commands
Commands are numbered 0-99 where 0-5 is taken and the rest is free to use. The following is a table over the predefined commands:
| CMD Number | CMD Definition              |   CMD Format |
|------------|-----------------------------|-----------|
| 0          | Stop                             | 0000 0 0000  |
| 1          | Step Forward/Backward            | 0000 1 F/B PWM% Microsteps 0000  |
| 2          | Step Left/Rigth                  | 0000 2 L/R PWM% Microsteps 0000  |
| 3          | Continuos Drive Forward/Backward | 0000 3 F/B PWM% 0000  |
| 4          | Continuos Turn Left/Right        | 0000 4 L/R PWM% 0000  |
| 5          | Initialize USB                   | 0000 5 0000           |
| 6          | Free to use                      |   |
| ...        | ...                              |   |
| 99         | Free to use                      |   |

## Task Diagram
:TODO


## Mutexes
To avoid [Deadlocks](https://www.geeksforgeeks.org/introduction-of-deadlock-in-operating-system/?) in the OS we use [mutexes](https://www.geeksforgeeks.org/difference-between-binary-semaphore-and-mutex/). The mutexes are defined in [queues.h](picoPI/src/inc/queues.h) and they are **QueueMutex** and **USBMutex**, these are to protect the usb serial as we use it both for receiving commands and sending messages back, and to protect the queue as we access the queue from 3 different task at the *same* time.


## Credits
Credit to [dj144](https://github.com/aws-iot-builder-tools/freertos-pi-pico) for the foundation for FreeRTOS on a Pico PI.

[FreeRTOS](https://github.com/FreeRTOS/FreeRTOS-Kernel)