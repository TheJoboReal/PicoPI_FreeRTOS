#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "defines.h"
#include "queues.h"
#include "commands.h"

#endif // FUNCTIONS_H

const size_t BUFFER_SIZE = 64;

void vBlinkTask() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    for (;;) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        vTaskDelay(250);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(250);
    }
}


void vReceiverTask(void *pvParameters) {
    char commandMessage[BUFFER_SIZE];
    int commandLength = 0;

    for(;;) {
        // Read command from USB serial
        if (fgets(commandMessage, BUFFER_SIZE, stdin) != NULL) {
            // Ensure the command message is null-terminated
            commandMessage[strcspn(commandMessage, "\n")] = '\0'; // Remove newline character
            
            // Send the message to the queue
            if (xQueueSend(commandQueue, &commandMessage, portMAX_DELAY) != pdTRUE) {
                printf("Error sending command to queue\n");
            } else {
                printf("Received and queued command: %s\n", commandMessage);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void vCommandRunTask(void *pvParameters) {
    char commandMessage[BUFFER_SIZE];
    char commandData[16];

    for(;;) {
        if (xQueueReceive(commandQueue, &commandMessage, portMAX_DELAY) == pdTRUE) {
            printf("Processing command: %s\n", commandMessage);
            
            // Extract the command ID 
            int command = commandMessage[0];

            // Extract the command data
            int i = 2;
            int paramIndex = 0;
            
            // Extract command data, stop at the first space or end of the string
            while (commandMessage[i] != '\0' && commandMessage[i] != '\n' && paramIndex < sizeof(commandData) - 1) {
                printf("Processing command: %c\n", commandMessage[i]);
                if (commandMessage[i] == ' ') {
                    // Skip spaces between parameters
                    i++;
                    continue;
                }
                commandData[paramIndex++] = commandMessage[i++];
            }
            commandData[paramIndex] = '\0';

            // Process the command
            switch(command) {
                case 0:
                    // Stop command
                    printf("Executing Stop command\n");
                    STOP();  // Implement your stop function here
                    break;

                case 1:
                    // Drive command
                    printf("Executing Drive command: %s\n", commandData);
                    DRV(commandData);  // Implement your drive function here
                    break;

                case 2:
                    // Turn command
                    printf("Executing Turn command: %s\n", commandData);
                    break;

                default:
                    // Command not recognized
                    printf("Command not recognized: %s\n", commandMessage);
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}