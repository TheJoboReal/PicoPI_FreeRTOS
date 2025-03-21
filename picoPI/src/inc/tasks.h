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

const size_t BUFFER_SIZE = 32;

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
    char message[BUFFER_SIZE];

    for (;;) {
        if (fgets(message, BUFFER_SIZE, stdin) != NULL) {  // Read from USB serial
            printf("Received: %s", message);  // Print received message
            xQueueSend(commandQueue, &message, 1);
        }
    }
}

void vCommandTask(){
    // Task that runs through and executes the command calls in the commandQueue
    // Commands are strings that contain a commmand ID and some maybe some data for the command(Drive direction or speed perhaps)
    char commandMessage[BUFFER_SIZE];
    char command[3];
    char commandData[16];


    for(;;){
        if(xQueueReceive(commandQueue, &commandMessage, portMAX_DELAY)){
            // Execute the command
            printf("Received command: %s", command);

            // Take the first 3 bits and load into command
            for(int i = 0; i < 3; i++){
                command[i] = commandMessage[i];
            }

            // Case to call the corresponding command function
            switch(command){
                case "DRV":
                    // Drive command
                    // Load the data into commandData
                    for(int i = 3; i < 11; i++){
                        commandData[i-3] = commandMessage[i];
                    }
                    // Call the drive function
                    
                    break;

                case "STP":
                    // Stop command
                    
                    break;

                case "TRN":
                    // Turn command
                    // Load the data into commandData
                    for(int i = 3; i < 11; i++){
                        commandData[i-3] = commandMessage[i];
                    }
                    // Call the turn function

                    break;

                default:
                    // Command not recognized
                    printf("Command not recognized");
                    break;
            }
        }
    }

}



