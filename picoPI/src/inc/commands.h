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

    StepperMotor motor;
    init_stepper(&motor, pins, 1, pwm_pct, MICRO_STEPS);

    for (int i = 0; i < steps; i++) {
        if (timeOutFlag) {        // Checks for the stop command
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Timed out, stopping steppers\n");
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
        
        if (i % 10 == 0) {
            taskYIELD(); // Yield to let FreeRTOS handle other tasks
        }
    }

    stop_stepper(pins);
}
void Turn(char *commandData){
    // Command format: "CON DIRECTION SPEED"
    // DIRECTION: L or R
    // SPEED: 0-100

    int direction;

    if (commandData[3] == 'L') {
        direction = 1;  // Left
    } else if (commandData[3] == 'R') {
        direction = -1; // Left
    } else {
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Invalid direction\n");
        xSemaphoreGive(USBmutex);
        return;
    }

    int pwm_pct = atoi(&commandData[4]);        // Speed percentage
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


    StepperMotor motor;
    init_stepper(&motor, pins, 1, pwm_pct, MICRO_STEPS);

            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Entering drive loop\n");
            xSemaphoreGive(USBmutex);

    int i = 0;
    for (int i = 0; i < steps; i++) {
        if (timeOutFlag) {        // Checks for the stop command
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Timed out, stopping steppers\n");
            xSemaphoreGive(USBmutex);

            stop_stepper(pins);
            return;
        }

        // Execute the motor step
        set_step(&motor, 0, (motor.step_counter[0] - 1 * direction) % 8);  // Left motor
        set_step(&motor, 1, (motor.step_counter[1] + 1 * direction) % 8);  // Right motor

        // Delay based on speed
        vTaskDelay(STEP_DELAY_US / 1000 / portTICK_PERIOD_MS);

        // Update motor step counters
        motor.step_counter[0] -= 1 * direction;
        motor.step_counter[1] += direction;
        
        if (i % 10 == 0) {
            taskYIELD(); // Yield to let FreeRTOS handle other tasks
        }
        i++;
    }

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Turn command completed\n");
    xSemaphoreGive(USBmutex);
}

void STOP(){

    stop_stepper(pins);

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Stopped Robot\n");
    xSemaphoreGive(USBmutex);

    xTaskNotifyGive(xTaskGetCurrentTaskHandle());
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

    int pwm_pct = atoi(&commandData[4]);        // Speed percentage
    if (pwm_pct < 0 || pwm_pct > 100) {
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Invalid speed percentage\n");
        xSemaphoreGive(USBmutex);
        return;
    }


    StepperMotor motor;
    init_stepper(&motor, pins, 1, pwm_pct, MICRO_STEPS);

            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Entering drive loop\n");
            xSemaphoreGive(USBmutex);

    int i = 0;

    for (;;) {
        if (timeOutFlag) {        // Checks for the stop command
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Timed out, stopping steppers\n");
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
        
        if (i % 10 == 0) {
            taskYIELD(); // Yield to let FreeRTOS handle other tasks
        }
        i++;
    }

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("Continuous drive command completed\n");
    xSemaphoreGive(USBmutex);
}


void conTurn(char *commandData){
    // Command format: "CON DIRECTION SPEED"
    // DIRECTION: L or R
    // SPEED: 0-100

    int direction;

    if (commandData[3] == 'L') {
        direction = 1;  // Left
    } else if (commandData[3] == 'R') {
        direction = -1; // Left
    } else {
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Invalid direction\n");
        xSemaphoreGive(USBmutex);
        return;
    }

    int pwm_pct = atoi(&commandData[4]);        // Speed percentage
    if (pwm_pct < 0 || pwm_pct > 100) {
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Invalid speed percentage\n");
        xSemaphoreGive(USBmutex);
        return;
    }


    StepperMotor motor;
    init_stepper(&motor, pins, 1, pwm_pct, MICRO_STEPS);

            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Entering drive loop\n");
            xSemaphoreGive(USBmutex);

    int i = 0;
    for (;;) {
        if (timeOutFlag) {        // Checks for the stop command
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Timed out, stopping steppers\n");
            xSemaphoreGive(USBmutex);

            stop_stepper(pins);
            return;
        }

        // Execute the motor step
        set_step(&motor, 0, (motor.step_counter[0] - 1 * direction) % 8);  // Left motor
        set_step(&motor, 1, (motor.step_counter[1] + 1 * direction) % 8);  // Right motor

        // Delay based on speed
        vTaskDelay(STEP_DELAY_US / 1000 / portTICK_PERIOD_MS);

        // Update motor step counters
        motor.step_counter[0] -= 1 * direction;
        motor.step_counter[1] += direction;
        
        if (i % 10 == 0) {
            taskYIELD(); // Yield to let FreeRTOS handle other tasks
        }
        i++;
    }

    xSemaphoreTake(USBmutex, portMAX_DELAY);
    printf("ConTurn command completed\n");
    xSemaphoreGive(USBmutex);
}