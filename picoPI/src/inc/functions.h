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

#endif // FUNCTIONS_H


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
            xQueueSend(commandBufferQueue, &message, 1)
        }
    }
}

