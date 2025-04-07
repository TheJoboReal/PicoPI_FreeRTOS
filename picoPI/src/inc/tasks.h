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
#include "semphr.h"

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

void vPrintAliveTask(){
    for(;;){
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Alive\n");
        xSemaphoreGive(USBmutex);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void vReceiverTask(void *pvParameters) {
    char commandMessage[BUFFER_SIZE];

    for (;;) {
        // Read command from USB serial
        xSemaphoreTake(USBmutex, portMAX_DELAY); 
        if (fgets(commandMessage, BUFFER_SIZE, stdin) != NULL) {    // Read command from stdin
            int length = strlen(commandMessage);
            xSemaphoreGive(USBmutex); 

            // Ensure the message is long enough for both start and end flags
            if (length < 9) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                // printf("Invalid command: Too short\n");
                xSemaphoreGive(USBmutex);
                continue;
            }

            // Remove trailing newline if present
            if (commandMessage[length - 1] == '\n' || commandMessage[length - 1] == '\r') {
                commandMessage[length - 1] = '\0';
                length--;
            }

            // Print raw received command
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Raw received command: %s\n", commandMessage);
            xSemaphoreGive(USBmutex);

            // Check for start flag "0000"
            if (!(commandMessage[0] == '0' && commandMessage[1] == '0' &&
                  commandMessage[2] == '0' && commandMessage[3] == '0')) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command format: Missing start flag\n");
                xSemaphoreGive(USBmutex);
                continue;
            }

            // Check for end flag "0000"
            if (!(commandMessage[length - 4] == '0' && commandMessage[length - 3] == '0' &&
                  commandMessage[length - 2] == '0' && commandMessage[length - 1] == '0')) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command format: Missing end flag\n");
                xSemaphoreGive(USBmutex);
                continue;
            }

            // Remove start and end flags manually
            int newLength = length - 8;  // Exclude 4 from start and 4 from end
            if (newLength <= 0) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command format: No valid data\n");
                xSemaphoreGive(USBmutex);
                continue;
            }

            // Allocate memory for the processed command
            char *processedCommand = (char *)pvPortMalloc(newLength + 1);
            if (processedCommand == NULL) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Memory allocation failed\n");
                xSemaphoreGive(USBmutex);
                continue;
            }

            // Copy processed command without flags
            strncpy(processedCommand, &commandMessage[4], newLength);
            processedCommand[newLength] = '\0';

            // Debugging: Print cleaned command
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Processed command: %s\n", processedCommand);
            xSemaphoreGive(USBmutex);

            // Send pointer to queue
            if (xQueueSend(commandQueue, &processedCommand, portMAX_DELAY) != pdTRUE) {

                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Error sending command to queue\n");
                xSemaphoreGive(USBmutex);

            } else {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Command successfully queued: %s\n", processedCommand);
                xSemaphoreGive(USBmutex);
            }
        }
        xSemaphoreGive(USBmutex);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}




void vCommandRunTask(void *pvParameters) {
    char *commandMessage;  // Pointer to hold the received command

    for (;;) {
        // Receive pointer to command string from queue
        if (xQueueReceive(commandQueue, &commandMessage, portMAX_DELAY) == pdTRUE) {
            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Received Command from queue:%s\n", commandMessage);
            xSemaphoreGive(USBmutex);

            // Trim leading spaces to find the actual command ID
            char *ptr = commandMessage;
            while (*ptr == ' ') ptr++;  // Move past spaces

            // Extract command ID
            if (*ptr < '0' || *ptr > '9') {     // Check if it's a valid digit
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command ID: %c\n", *ptr);
                xSemaphoreGive(USBmutex);

                continue;
            }

            int command = *ptr - '0';  // Convert char to integer

            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Command ID: %d\n", command);
            xSemaphoreGive(USBmutex);

            // Process the command
            switch (command) {
                case 0:

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Executing Stop command\n");
                    xSemaphoreGive(USBmutex);
                    STOP();

                    vPortFree(commandMessage);
                    break;
                case 1:

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Executing Drive command: %s\n", commandMessage + 2);
                    xSemaphoreGive(USBmutex);

                    DRV(commandMessage + 2);

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Drive command executed\n");
                    xSemaphoreGive(USBmutex);

                    vPortFree(commandMessage);
                    break;
                case 2:

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Executing Turn command: %s\n", commandMessage + 2);
                    xSemaphoreGive(USBmutex);

                    vPortFree(commandMessage);
                    break;
                
                case 3:
                // Continual drive
                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Executing Continual Drive\n");
                    xSemaphoreGive(USBmutex);

                    conDrive(commandMessage);

                    vPortFree(commandMessage);

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Continual Drive command executed\n");
                    xSemaphoreGive(USBmutex);

                case 9:
                // USB serial init.

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("USB initialized\n");
                    xSemaphoreGive(USBmutex);

                    vPortFree(commandMessage);
                    break;

                default:

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Command not recognized: %s\n", commandMessage);
                    xSemaphoreGive(USBmutex);

                    vPortFree(commandMessage);
                    break;
            }

            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Command executed\n");
            xSemaphoreGive(USBmutex);

        }

            xSemaphoreTake(USBmutex, portMAX_DELAY);
            printf("Udenfor switch\n");
            xSemaphoreGive(USBmutex);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
