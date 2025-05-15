#ifndef DEFINES_H
#define DEFINES_H


#define PICO_DEFAULT_LED_PIN 0
#define COMMAND_QUEUE_SIZE 16

uint pins[] = {0, 1, 2, 3, 4, 5, 6, 7};


// The following allow one to use the defined names instead of numbers.
// This is to make the code more readable. These are the commands that can be send and executed on the pico and is used in the commandTask
#define STOP_COMMAND 0
#define DRV_COMMAND 1
#define TURN_COMMAND 2
#define CON_DRIVE_COMMAND 3
#define CON_TURN_COMMAND 4
#define USB_INIT_COMMAND 5

const size_t BUFFER_SIZE = 64;
volatile bool timeOutFlag = false;      // The timeout flag that stop the motors if it is set to true

#endif //DEFINES_H