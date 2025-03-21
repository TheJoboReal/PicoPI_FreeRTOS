// File for creating different commands that can be executed when a command call is received

#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "defines.h"

#endif //COMMANDS_H

void DRV(char *commandData){
    // Drive command
    // Command data should be in the format of "DRV DIRECTION SPEED INTERVAL"
    // DIRECTION is either F or B for forward and backward
    // SPEED is a number from 0 to 100

    char direction = commandData[0];    // Direction is the first character of the command data
    int speed = atoi(&commandData[2]);  // Speed is the number after the space in the command data
    int interval = atoi(&commandData[4]);   // Interval is the number after the second space in the command data
    printf("Drive command received: %c %d\n", direction, speed);


}

void STP(){
    // Stop command
    // Stops the robot
    printf("Stop command received\n");


}