// File for creating different commands that can be executed when a command call is received

#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "defines.h"
#include "pwm.h"

#endif //COMMANDS_H

void DRV(char *commandData){
    // Drive command
    // Command data should be in the format of "DRV DIRECTION SPEED INTERVAL"
    // DIRECTION is either F or B for forward and backward
    // SPEED is a number from 0 to 100
    int direction;
    if (commandData[0] == 'F') {
        direction = 1;  // Forward
    } else if (commandData[0] == 'B') {
        direction = -1; // Backward
    } else {
        printf("Invalid direction\n");
        return;
    }
    int pwm_pct = atoi(&commandData[2]);  // Speed is the number after the space in the command data
    int steps = atoi(&commandData[4]);   // Interval is the number after the second space in the command data
    printf("Drive command received: %c %d\n", direction, pwm_pct);

    StepperMotor motor;

    init_stepper(&motor, pins, 1, 15, MICRO_STEPS);
    move_stepper(&motor, steps, direction, STEP_DELAY_US);

}

void STP(){
    // Stop command
    // Stops the robot
    printf("Stop command received\n");
    stop_stepper(pins);


}




