#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "inc/defines.h"


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


void main() {
    stdio_init_all();  // Initializes USB serial communication
    busy_wait_ms(1000);  // Delay for stabilization
    MessageBufferHandle_t buffer = xMessageBufferCreate(BUFFER_SIZE);

    // Create tasks
    xTaskCreate(vBlinkTask, "blink task", 128, (void *)buffer, 2, NULL);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
}
