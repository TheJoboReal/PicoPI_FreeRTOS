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

    if (strlen(commandData) < 8) {
        printf("Command too short\n");
        return;
    }

    int direction;

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Commanddata received: %s\n", commandData);
    xSemaphoreGive(USBmutex);
    if (commandData[1] == 'F') {
        direction = 1;  // Forward
    } else if (commandData[1] == 'B') {
        direction = -1; // Backward
    } else {

        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Invalid direction\n");
        xSemaphoreGive(USBmutex);

        return;
    }

    int pwm_pct = atoi(&commandData[3]);        // Speed percentage
    if (pwm_pct < 0 || pwm_pct > 100) {

        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Invalid speed percentage\n");
        xSemaphoreGive(USBmutex);
        return;
    }
    int steps = atoi(&commandData[5]);          // Steps
    if (steps < 0) {

        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Invalid steps\n");
        xSemaphoreGive(USBmutex);

        return;
    }

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("DRV command received: %c %d %d\n", direction, pwm_pct, steps);
    xSemaphoreGive(USBmutex);

    StepperMotor motor;
    init_stepper(&motor, pins, 1, 15, MICRO_STEPS);
    move_stepper(&motor, steps, direction, STEP_DELAY_US);
}

void STOP(){
    // Stop command

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Stop command received\n");
    xSemaphoreGive(USBmutex);

    stop_stepper(pins);

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Stopped Robot\n");
    xSemaphoreGive(USBmutex);


}