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
#include "semphr.h"

#endif // FUNCTIONS_H



void vBlinkTask() {
    // Task that blinks every 250 miliseconds. This is to indicate that the pico is still running and have not crashed.
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
    // Task that send Im alive over serial every 1000 miliseconds. This is to indicate that the pico is still running and have not crashed.
    for(;;){
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Im alive\n");
        xSemaphoreGive(USBmutex);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// void vQueuePeekerTask(){
//     for(;;){
//         xSemaphoreTake(QueueMutex, portMAX_DELAY);
//         char *commandMessage;
//         if (xQueuePeek(commandQueue, &commandMessage, 0) == pdTRUE) {
//             // Check if command in queue is a stop command

//             // Trim leading spaces to find the actual command ID
//             char *ptr = commandMessage;
//             while (*ptr == ' ') ptr++;  // Move past spaces

//             // Convert processed command to integer (0–99 expected)
//             int commandValue = atoi(commandMessage);  // Converts string to int

//             // Validate range
//             if (commandValue < 0 || commandValue > 99) {
//                 xSemaphoreTake(USBmutex, portMAX_DELAY);
//                 printf("Invalid command: out of range (%d)\n", commandValue);
//                 xSemaphoreGive(USBmutex);
//                 vPortFree(commandMessage);
//                 continue;
//             }


//             int command = *ptr - '0';  // Convert char to integer

//             if(command == 0){
//                 timeOutFlag = true;
//                 xSemaphoreTake(USBmutex, portMAX_DELAY);
//                 printf("Stop command recived");
//                 xSemaphoreGive(USBmutex);
//             }
//         }
//         xSemaphoreGive(QueueMutex);
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }

void vReceiverTask(void *pvParameters) {
    // Reciever task that recieves strings over serial, removes start and stop flags and adds the remaining message to a Queue that other tasks can access.
    char commandMessage[BUFFER_SIZE];

    for (;;) {

        vTaskDelay(pdMS_TO_TICKS(100));
        // Read command from USB serial
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        if (fgets(commandMessage, BUFFER_SIZE, stdin) != NULL) {    // Read command from stdin. Returns true if anything is received over serial.
            xSemaphoreGive(QueueMutex);

            int length = strlen(commandMessage);

            // Ensure the message is long enough for both start and end flags
            if (length < 9) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command: Too short\n");
                xSemaphoreGive(USBmutex);
                continue;
            }


            // Remove trailing newline if present
            if (commandMessage[length - 1] == '\n' || commandMessage[length - 1] == '\r') {     // Check for newline or carriage return
                commandMessage[length - 1] = '\0';      // Replace with null terminator
                length--;
            }

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
            int newLength = length - 8;  // Exclude 4 from start and 4 from end(start and end flags)
            if (newLength <= 0) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command format: No valid data\n");
                xSemaphoreGive(USBmutex);
                continue;
            }

            // Allocate memory for the processed command
            xSemaphoreTake(QueueMutex, portMAX_DELAY);
            char *processedCommand = (char *)pvPortMalloc(newLength + 1);
            if (processedCommand == NULL) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Memory allocation failed\n");
                xSemaphoreGive(USBmutex);
                continue;
            }
            xSemaphoreGive(QueueMutex);

            // Copy processed command without flags
            strncpy(processedCommand, &commandMessage[4], newLength);   // Copy the commandMessage from 4 to newLength to get the message without flags
            processedCommand[newLength] = '\0';     // Null-terminate the string

            // Send pointer to queue
            xSemaphoreTake(QueueMutex, portMAX_DELAY);
            if (xQueueSend(commandQueue, &processedCommand, portMAX_DELAY) != pdTRUE) {

                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Error sending command to queue\n");
                xSemaphoreGive(USBmutex);

            } else {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Command successfully queued: %s\n", processedCommand);
                xSemaphoreGive(USBmutex);
            }
            xSemaphoreGive(QueueMutex);
        }
        xSemaphoreGive(USBmutex);

    }
}


void vCommandRunTask(void *pvParameters) {
    char *commandMessage;  // Pointer to hold the received command

    for (;;) {
        // Receive pointer to command string from queue
        xSemaphoreTake(QueueMutex, portMAX_DELAY);
        if (xQueueReceive(commandQueue, &commandMessage, portMAX_DELAY) == pdTRUE) {

            // Trim leading spaces to find the actual command ID
            char *ptr = commandMessage;
            while (*ptr == ' ') ptr++;  // Move past spaces

            // Convert processed command to integer (0–99 expected)
            int commandValue = atoi(commandMessage);  // Converts string to int

            // Validate range
            if (commandValue < 0 || commandValue > 99) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command: out of range (%d)\n", commandValue);
                xSemaphoreGive(USBmutex);
                vPortFree(commandMessage);
                continue;
            }


            int command = *ptr - '0';  // Convert char to integer

            xSemaphoreGive(QueueMutex);

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

                        Turn(commandMessage);

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Turn command executed\n");
                        xSemaphoreGive(USBmutex);

                        vPortFree(commandMessage);
                        break;
                    
                    case 3:
                    // Continual drive
                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Executing Continual Drive\n");
                        xSemaphoreGive(USBmutex);

                        conDrive(commandMessage);

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("ConDrive command executed\n");
                        xSemaphoreGive(USBmutex);

                        vPortFree(commandMessage);

                    case 4:
                    // Continual turn
                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Executing Continual Turn\n");
                        xSemaphoreGive(USBmutex);

                        conTurn(commandMessage);

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("ConTurn command executed\n");
                        xSemaphoreGive(USBmutex);

                        vPortFree(commandMessage);

                    case 5:
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
            xSemaphoreGive(QueueMutex);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
