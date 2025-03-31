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
        if (fgets(commandMessage, BUFFER_SIZE, stdin) != NULL) {    // Read command from stdin
            int length = strlen(commandMessage);

            // Ensure the message is long enough for both start and end flags
            if (length < 9) {
                // printf("Invalid command: Too short\n");
                continue;
            }

            // Remove trailing newline if present
            if (commandMessage[length - 1] == '\n' || commandMessage[length - 1] == '\r') {
                commandMessage[length - 1] = '\0';
                length--;
            }

            // Print raw received command
            // printf("Raw received command: %s\n", commandMessage);

            // Check for start flag "0000"
            if (!(commandMessage[0] == '0' && commandMessage[1] == '0' &&
                  commandMessage[2] == '0' && commandMessage[3] == '0')) {
                // printf("Invalid command format: Missing start flag\n");
                continue;
            }

            // Check for end flag "0000"
            if (!(commandMessage[length - 4] == '0' && commandMessage[length - 3] == '0' &&
                  commandMessage[length - 2] == '0' && commandMessage[length - 1] == '0')) {
                // printf("Invalid command format: Missing end flag\n");
                continue;
            }

            // Remove start and end flags manually
            int newLength = length - 8;  // Exclude 4 from start and 4 from end
            if (newLength <= 0) {
                // printf("Invalid command format: No valid data\n");
                continue;
            }

            // Allocate memory for the processed command
            char *processedCommand = (char *)pvPortMalloc(newLength + 1);
            if (processedCommand == NULL) {
                // printf("Memory allocation failed\n");
                continue;
            }

            // Copy processed command without flags
            strncpy(processedCommand, &commandMessage[4], newLength);
            processedCommand[newLength] = '\0';

            // Debugging: Print cleaned command
            // printf("Processed command: %s\n", processedCommand);

            // Send pointer to queue
            if (xQueueSend(commandQueue, &processedCommand, portMAX_DELAY) != pdTRUE) {
                // printf("Error sending command to queue\n");
                vPortFree(processedCommand);
            } else {
                // printf("Command successfully queued: %s\n", processedCommand);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}




void vCommandRunTask(void *pvParameters) {
    char *commandMessage;  // Pointer to hold the received command

    for (;;) {
        // Receive pointer to command string from queue
        if (xQueueReceive(commandQueue, &commandMessage, portMAX_DELAY) == pdTRUE) {
            // printf("Received Command from queue:%s\n", commandMessage);

            // Trim leading spaces to find the actual command ID
            char *ptr = commandMessage;
            while (*ptr == ' ') ptr++;  // Move past spaces

            // Extract command ID
            if (*ptr < '0' || *ptr > '9') {     // Check if it's a valid digit
                // printf("Invalid command ID: %c\n", *ptr);
                vPortFree(commandMessage);      // Release memory
                continue;
            }

            int command = *ptr - '0';  // Convert char to integer

            // printf("Command ID: %d\n", command);

            // Process the command
            switch (command) {
                case 0:
                    // printf("Executing Stop command\n");
                    STOP();
                    break;
                case 1:
                    // printf("Executing Drive command: %s\n", commandMessage + 2);
                    DRV(commandMessage + 2);
                    // printf("Drive command executed\n");  // TODO: den her line crasher pico'en ved first message sent
                    break;
                case 2:
                    // printf("Executing Turn command: %s\n", commandMessage + 2);
                    break;
                default:
                    // printf("Command not recognized: %s\n", commandMessage);
                    break;
            }

            // Free allocated memory after processing
            vPortFree(commandMessage);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
