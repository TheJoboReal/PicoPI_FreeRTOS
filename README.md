# Intro
This is a individual student project that aims at improving my own understanding of inter process communication.

This project uses ZeroMQ to establish communication between two processes of different languages

## Setup
To run the code you must first run the [[picoPI/setup_lib.sh|setup.sh]]: the script pulls the needed libraries and sets up paths for your environment. Next, run the [[picoPI/cmake.sh|cmake.sh]] script to set up the build folder. Now run the [[picoPI/compile.sh|compile.sh]] script to make the binary files: this script should also be run when you make changes to the code. Lastly, plug the usb cable into the pico pi while holding down the reset button, the pi should now be in flash mode and be discoverable as a media, run the [[picoPI/run.sh|run.sh]] script to falsh the pico. The pico should now run the code that you compiled.

## How the code works


## Task Diagram


## Mutexes
To avoid [[https://www.geeksforgeeks.org/introduction-of-deadlock-in-operating-system/?|deadlocks]] in the OS we use [[https://www.geeksforgeeks.org/difference-between-binary-semaphore-and-mutex/|Mutexes]]. The mutexes are defined in [[picoPI/src/inc/queues.h|queues.h]] and they are **QueueMutex** and **USBMutex**, these are to protect the usb serial as we use it both for receiving commands and sending messages back, and to protect the queue as we access the queue from 3 different task at the *same* time.

