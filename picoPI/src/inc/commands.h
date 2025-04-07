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
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Command too short\n");
        xSemaphoreGive(USBmutex);
        return;
    }

    int direction;

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

    // Initialize stepper motor (setup)
    StepperMotor motor;
    init_stepper(&motor, pins, 1, 15, MICRO_STEPS);

    // Move stepper with a polling check for the stop command
    for (int i = 0; i < steps; i++) {
        if (stopMotorFlag) {
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Stop command received, stopping motor\n");
            xSemaphoreGive(USBmutex);

            stop_stepper(pins);  // Stop the motor immediately
            return;  // Exit the function, effectively stopping further movement
        }

        // Move motor by one step (this should include a small delay between steps)
        move_stepper(&motor, 1, direction, STEP_DELAY_US);
        
        // Optionally, add a small delay here to allow for periodic checking
        vTaskDelay(pdMS_TO_TICKS(1));  // Delay to allow checking of the stop flag
    }

    // Finalize and stop stepper if completed
    stop_stepper(pins);
}


void conDrive(char *commandData){
    // Command format: "CON DIRECTION SPEED"
    // DIRECTION: F or B
    // SPEED: 0-100

    int direction;

    if (commandData[3] == 'F') {
        direction = 1;  // Forward
    } else if (commandData[3] == 'B') {
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

    int steps = 99999;

    StepperMotor motor;
    init_stepper(&motor, pins, 1, pwm_pct, MICRO_STEPS);

    for (int i = 0; i < steps; i++) {
        // Check for notification to stop the motor
        if (ulTaskNotifyTake(pdTRUE, 0)) {
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Continuous drive command interrupted (Stop signal received)\n");
            xSemaphoreGive(USBmutex);
            stop_stepper(pins);
            return;
        }

        // Execute the motor step
        set_step(&motor, 0, (motor.step_counter[0] + direction) % 8);  // Left motor
        set_step(&motor, 1, (motor.step_counter[1] + direction) % 8);  // Right motor

        // Delay based on speed
        vTaskDelay(STEP_DELAY_US / 1000 / portTICK_PERIOD_MS);

        // Update motor step counters
        motor.step_counter[0] += direction;
        motor.step_counter[1] += direction;

        // Yield every few steps to allow the system to process new commands
        if (i % 10 == 0) {
            taskYIELD(); // Yield to let FreeRTOS handle other tasks
        }
    }

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Continuous drive command completed\n");
    xSemaphoreGive(USBmutex);
}



void STOP(){

    stop_stepper(pins);

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Stopped Robot\n");
    xSemaphoreGive(USBmutex);

    xTaskNotifyGive(xTaskGetCurrentTaskHandle());
}
