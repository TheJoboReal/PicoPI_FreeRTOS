#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

const size_t BUFFER_SIZE = 32;
#define PICO_DEFAULT_LED_PIN 0

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

// Sender task sends a message over the serial
void vSenderTask(void *pvParameters) {
    MessageBufferHandle_t buffer = (MessageBufferHandle_t) pvParameters;
    char message[] = "FreeRTOS + Pi Pico";
    for (;;) {        
        // Send message to the buffer (not used in this case for serial)
        xMessageBufferSend(buffer, (void *)message, strlen(message), 0);
        
        // Send the message over USB serial so Python can read it
        printf("%s\n", message);  // Sends the message over the USB serial port
        vTaskDelay(1000);
    }
}

void main() {
    stdio_init_all();  // Initializes USB serial communication
    busy_wait_ms(1000);  // Delay for stabilization
    MessageBufferHandle_t buffer = xMessageBufferCreate(BUFFER_SIZE);

    // Create tasks
    xTaskCreate(vSenderTask, "Sender", 128, (void *)buffer, 1, NULL);
    xTaskCreate(vBlinkTask, "blink task", 128, (void *)buffer, 2, NULL);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
}
