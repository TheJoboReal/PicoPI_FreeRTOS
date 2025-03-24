#ifndef PWM_H
#define PWM_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <string.h>

// Define constants
#define PWM_MAX 65535  // Maximum PWM value (16-bit resolution)
#define MICRO_STEPS 4  // Microstepping
#define STEP_DELAY_US 1000  // Step delay in microseconds

// Stepper motor pin setup
typedef struct {
    uint pin[8];        // GPIO pins connected to both motors (pins 0-3 for left motor, pins 4-7 for right motor)
    uint pwm_val;       // PWM duty cycle value (0 to 65535)
    uint micro_steps;   // Number of microsteps per full step
    uint step_mode;     // Step mode: 0=FULL, 1=HALF, 2=MICRO
    uint step_counter[2];  // Step counters for left and right motors
    uint step_sequence[8][4];  // Step sequence for FULL and HALF mode
} StepperMotor;

// Function to initialize stepper motor
void init_stepper(StepperMotor* motor, uint pins[8], uint step_mode, uint pwm_pct, uint micro_steps) {
    motor->step_counter[0] = 0;  // Left motor step counter
    motor->step_counter[1] = 0;  // Right motor step counter
    motor->step_mode = step_mode;
    motor->micro_steps = micro_steps;
    motor->pwm_val = (PWM_MAX * pwm_pct) / 100;

    // Initialize GPIO pins
    for (int i = 0; i < 8; i++) {
        motor->pin[i] = pins[i];
        gpio_init(motor->pin[i]);
        gpio_set_dir(motor->pin[i], GPIO_OUT);
        gpio_put(motor->pin[i], 0);  // Initially set to low
    }

    // Setup step sequences based on step_mode
    if (step_mode == 0) {
        // FULL step mode
        motor->step_sequence[0][0] = motor->pwm_val; motor->step_sequence[0][1] = motor->pwm_val; motor->step_sequence[0][2] = 0; motor->step_sequence[0][3] = 0;
        motor->step_sequence[1][0] = 0; motor->step_sequence[1][1] = motor->pwm_val; motor->step_sequence[1][2] = motor->pwm_val; motor->step_sequence[1][3] = 0;
        motor->step_sequence[2][0] = 0; motor->step_sequence[2][1] = 0; motor->step_sequence[2][2] = motor->pwm_val; motor->step_sequence[2][3] = motor->pwm_val;
        motor->step_sequence[3][0] = motor->pwm_val; motor->step_sequence[3][1] = 0; motor->step_sequence[3][2] = 0; motor->step_sequence[3][3] = motor->pwm_val;
    }
    else if (step_mode == 1) {
        // HALF step mode
        motor->step_sequence[0][0] = motor->pwm_val; motor->step_sequence[0][1] = 0; motor->step_sequence[0][2] = 0; motor->step_sequence[0][3] = 0;
        motor->step_sequence[1][0] = motor->pwm_val; motor->step_sequence[1][1] = motor->pwm_val; motor->step_sequence[1][2] = 0; motor->step_sequence[1][3] = 0;
        motor->step_sequence[2][0] = 0; motor->step_sequence[2][1] = motor->pwm_val; motor->step_sequence[2][2] = 0; motor->step_sequence[2][3] = 0;
        motor->step_sequence[3][0] = 0; motor->step_sequence[3][1] = motor->pwm_val; motor->step_sequence[3][2] = motor->pwm_val; motor->step_sequence[3][3] = 0;
        motor->step_sequence[4][0] = 0; motor->step_sequence[4][1] = 0; motor->step_sequence[4][2] = motor->pwm_val; motor->step_sequence[4][3] = 0;
        motor->step_sequence[5][0] = 0; motor->step_sequence[5][1] = 0; motor->step_sequence[5][2] = motor->pwm_val; motor->step_sequence[5][3] = motor->pwm_val;
        motor->step_sequence[6][0] = 0; motor->step_sequence[6][1] = 0; motor->step_sequence[6][2] = 0; motor->step_sequence[6][3] = motor->pwm_val;
        motor->step_sequence[7][0] = motor->pwm_val; motor->step_sequence[7][1] = 0; motor->step_sequence[7][2] = 0; motor->step_sequence[7][3] = motor->pwm_val;
    }
    else if (step_mode == 2) {
        // MICRO step mode
        motor->step_sequence[0][0] = motor->pwm_val; motor->step_sequence[0][1] = 0; motor->step_sequence[0][2] = 0; motor->step_sequence[0][3] = 0;
        motor->step_sequence[1][0] = motor->pwm_val / 2; motor->step_sequence[1][1] = motor->pwm_val / 2; motor->step_sequence[1][2] = 0; motor->step_sequence[1][3] = 0;
        motor->step_sequence[2][0] = 0; motor->step_sequence[2][1] = motor->pwm_val; motor->step_sequence[2][2] = 0; motor->step_sequence[2][3] = 0;
        motor->step_sequence[3][0] = 0; motor->step_sequence[3][1] = motor->pwm_val / 2; motor->step_sequence[3][2] = motor->pwm_val / 2; motor->step_sequence[3][3] = 0;
        motor->step_sequence[4][0] = 0; motor->step_sequence[4][1] = 0; motor->step_sequence[4][2] = motor->pwm_val; motor->step_sequence[4][3] = 0;
        motor->step_sequence[5][0] = 0; motor->step_sequence[5][1] = 0; motor->step_sequence[5][2] = motor->pwm_val / 2; motor->step_sequence[5][3] = motor->pwm_val / 2;
        motor->step_sequence[6][0] = 0; motor->step_sequence[6][1] = 0; motor->step_sequence[6][2] = 0; motor->step_sequence[6][3] = motor->pwm_val;
        motor->step_sequence[7][0] = motor->pwm_val / 2; motor->step_sequence[7][1] = 0; motor->step_sequence[7][2] = 0; motor->step_sequence[7][3] = motor->pwm_val / 2;
    }
}

// Set a specific step in the stepper motor (left or right motor)
void set_step(StepperMotor* motor, int motor_num, int step_index) {
    uint pin_offset = motor_num * 4; // Determine if it's left (0) or right (1) motor
    for (int i = 0; i < 4; i++) {
        if (motor->step_sequence[step_index][i] > 0) {
            gpio_put(motor->pin[pin_offset + i], 1);  // Set GPIO high for PWM "on"
        } else {
            gpio_put(motor->pin[pin_offset + i], 0);  // Set GPIO low for PWM "off"
        }
    }
}

// Move both motors (left and right) by a specific number of steps
void move_stepper(StepperMotor* motor, int steps, int direction_step, int delay_us) {
    for (int i = 0; i < steps; i++) {
        // Move both motors
        set_step(motor, 0, (motor->step_counter[0] + direction_step) % 8);  // Left motor
        set_step(motor, 1, (motor->step_counter[1] + direction_step) % 8);  // Right motor
        vTaskDelay(delay_us / 1000 / portTICK_PERIOD_MS);  // Delay
        motor->step_counter[0] += direction_step;  // Update left motor step counter
        motor->step_counter[1] += direction_step;  // Update right motor step counter
    }
}

void stop_stepper(uint pins[8]){
    for (int i = 0; i < 8; i++) {
        gpio_put(pins[i], 0);  // Set all GPIO pins low
    }
}

#endif