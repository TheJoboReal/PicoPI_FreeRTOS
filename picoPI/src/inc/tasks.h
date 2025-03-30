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
#include "functions.h"

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
    
    for (;;) {
        // Read command from USB serial
        if (fgets(commandMessage, BUFFER_SIZE, stdin) != NULL) {
            int length = strlen(commandMessage);

            // Ensure the message is long enough for both start and end flags
            if (length < 9) {
                printf("Invalid command: Too short\n");
                continue;
            }

            // Remove trailing newline if present
            if (commandMessage[length - 1] == '\n' || commandMessage[length - 1] == '\r') {
                commandMessage[length - 1] = '\0';
                length--;
            }

            // Debugging: Print raw received command
            printf("Raw received command: ");
            for (int i = 0; i < length; i++) {
                printf("%c", commandMessage[i]);
            }
            printf("\n");

            // Check for start flag "0000"
            if (!(commandMessage[0] == '0' && commandMessage[1] == '0' &&
                  commandMessage[2] == '0' && commandMessage[3] == '0')) {
                printf("Invalid command format: Missing start flag\n");
                continue;
            }

            // Check for end flag "0000"
            if (!(commandMessage[length - 4] == '0' && commandMessage[length - 3] == '0' &&
                  commandMessage[length - 2] == '0' && commandMessage[length - 1] == '0')) {
                printf("Invalid command format: Missing end flag\n");
                continue;
            }

            // Remove start and end flags manually
            int newLength = length - 8;  // Exclude 4 from start and 4 from end
            if (newLength <= 0) {
                printf("Invalid command format: No valid data\n");
                continue;
            }

            char processedCommand[newLength + 1];  // +1 for null terminator
            for (int i = 0; i < newLength; i++) {
                processedCommand[i] = commandMessage[i + 4];  // Shift left by 4 to remove start flag
            }
            processedCommand[newLength] = '\0';  // Null terminate

            // Debugging: Print cleaned command
            printf("Processed command: %s\n", processedCommand);

            // Allocate memory to store the command in the queue
            char *commandCopy = pvPortMalloc(newLength + 1);
            if (commandCopy == NULL) {
                printf("Memory allocation failed\n");
                continue;
            }
            strcpy(commandCopy, processedCommand);

            // Send to queue
            if (xQueueSend(commandQueue, &commandCopy, portMAX_DELAY) != pdTRUE) {
                printf("Error sending command to queue\n");
                vPortFree(commandCopy);
            } else {
                printf("Command successfully queued: %s\n", processedCommand);
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
            printf("Processing command: ");
            printCommand(commandMessage);
            
            // Extract the command ID 
            int command = commandMessage[0];

            // Extract the command data
            int i = 2;
            int paramIndex = 0;
            
            // Extract command data, stop at the first space or end of the string
            while (commandMessage[i] != '\0' && commandMessage[i] != '\n' && paramIndex < sizeof(commandData) - 1) {
                printf("Processing command: ");
                printCommand(commandMessage);
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
                    printf("Executing Drive command: ");
                    printCommand(commandData);
                    DRV(commandData);  // Implement your drive function here
                    break;

                case 2:
                    // Turn command
                    printf("Executing Turn command: ");
                    break;

                default:
                    // Command not recognized
                    printf("Command not recognized:");
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}