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
    // Task that blinks every 500ms to indicate the pico is still runnning
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
    // Task that prints Im alive every 1000ms to indicate the pico is still runnning
    for(;;){
        xSemaphoreTake(USBmutex, portMAX_DELAY);
        printf("Im alive\n");
        xSemaphoreGive(USBmutex);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vQueuePeekerTask(){ 
    // Task that peeks the queue without altering it. This is to look for stop commands to stop the stepper
    for(;;){
        xSemaphoreTake(QueueMutex, portMAX_DELAY);
        char *commandMessage;
        if (xQueuePeek(commandQueue, &commandMessage, 0) == pdTRUE) {

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

            if(command == STOP_COMMAND){
                timeOutFlag = true;
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Stop command recived");
                xSemaphoreGive(USBmutex);
            }
        }
        xSemaphoreGive(QueueMutex);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vReceiverTask(void *pvParameters) {
    // Task that checks the serial port to see if any message has been sent. Commands are then pushed to the commandQueue
    char commandMessage[BUFFER_SIZE];

    for (;;) {

        vTaskDelay(pdMS_TO_TICKS(100));
        // Read command from USB serial
        if (fgets(commandMessage, BUFFER_SIZE, stdin) != NULL) {
            int length = strlen(commandMessage);

            // Ensure the message is long enough for both start and end flags. 4 start and end flags, so a length of 9 minimum
            if (length < 9) {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Invalid command: Too short\n");
                xSemaphoreGive(USBmutex);
                continue;
            }


            // Remove trailing newline if present
            if (commandMessage[length - 1] == '\n' || commandMessage[length - 1] == '\r') {
                commandMessage[length - 1] = '\0';
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

            // Find length of message without the flags
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

            // Send pointer to queue
            if (xQueueSend(commandQueue, &processedCommand, portMAX_DELAY) != pdTRUE) {

                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Error sending command to queue\n");
                xSemaphoreGive(USBmutex);

            } else {
                xSemaphoreTake(USBmutex, portMAX_DELAY);
                printf("Command successfully queued: %s\n", processedCommand);
                xSemaphoreGive(USBmutex);
                timeOutFlag = false;
            }
        }
        xSemaphoreGive(USBmutex);

    }
}


void vCommandRunTask(void *pvParameters) {
    char *commandMessage;  // Pointer to hold the received command

    for (;;) {
        // Receive pointer to command string from queue
        if (xQueueReceive(commandQueue, &commandMessage, portMAX_DELAY) == pdTRUE) {

            // Trim leading spaces to find the actual command ID
            char *ptr = commandMessage;
            while (*ptr == ' ') ptr++;  // Move past spaces

            // Convert processed command to integer (0–99 expected)
            int commandValue = atoi(commandMessage);  // Converts string to int

            int command = *ptr - '0';  // Convert char to integer

            // Process the command, and run the corresponding command function
            switch (command) {
                case STOP_COMMAND:

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Executing Stop command\n");
                    xSemaphoreGive(USBmutex);
                    STOP();

                    vPortFree(commandMessage);
                    break;
                case DRV_COMMAND:

                    xSemaphoreTake(USBmutex, portMAX_DELAY);
                    printf("Executing Drive command: %s\n", commandMessage + 2);
                    xSemaphoreGive(USBmutex);

                        DRV(commandMessage + 2);

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Drive command executed\n");
                        xSemaphoreGive(USBmutex);

                        vPortFree(commandMessage);
                        break;
                    case TURN_COMMAND:

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Executing Turn command: %s\n", commandMessage + 2);
                        xSemaphoreGive(USBmutex);

                        Turn(commandMessage);

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Turn command executed\n");
                        xSemaphoreGive(USBmutex);

                        vPortFree(commandMessage);
                        break;
                    
                    case CON_DRIVE_COMMAND:
                    // Continual drive
                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Executing Continual Drive\n");
                        xSemaphoreGive(USBmutex);

                        conDrive(commandMessage);

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("ConDrive command executed\n");
                        xSemaphoreGive(USBmutex);

                        vPortFree(commandMessage);

                    case CON_TURN_COMMAND:
                    // Continual turn
                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("Executing Continual Turn\n");
                        xSemaphoreGive(USBmutex);

                        conTurn(commandMessage);

                        xSemaphoreTake(USBmutex, portMAX_DELAY);
                        printf("ConTurn command executed\n");
                        xSemaphoreGive(USBmutex);

                        vPortFree(commandMessage);

                    case USB_INIT_COMMAND:
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
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }