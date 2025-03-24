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
    // Command format: "DRV DIRECTION SPEED INTERVAL"
    int direction;
    if (commandData[0] == 'F') {
        direction = 1;  // Forward
    } else if (commandData[0] == 'B') {
        direction = -1; // Backward
    } else {
        printf("Invalid direction\n");
        return;
    }

    int pwm_pct = atoi(&commandData[2]);
    int steps = atoi(&commandData[4]); 

    printf("DRV command received: %c %d %d\n", direction, pwm_pct, steps);

    StepperMotor motor;
    init_stepper(&motor, pins, 1, 15, MICRO_STEPS);
    move_stepper(&motor, steps, direction, STEP_DELAY_US);
}

void STOP(){
    // Stop command
    // Stops the robot
    printf("Stop command received\n");
    stop_stepper(pins);


}

void process_command(int command_id, char *message) {
    switch (command_id) {
        case DRV_COMMAND:
            DRV(message);
            break;
        case STOP_COMMAND:
            STOP();
            break;
        default:
            printf("Invalid command\n");
    }
}




