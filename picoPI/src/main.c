#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "inc/defines.h"

QueueHandle_t commandBufferQueue;




void main() {
    stdio_init_all();  // Initializes USB serial communication
    busy_wait_ms(1000);  // Delay for stabilization

    MessageBufferHandle_t buffer = xMessageBufferCreate(BUFFER_SIZE);

    commandBufferQueue = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(int32_t));

    // Create tasks
    xTaskCreate(vBlinkTask, "blink task", 128, (void *)buffer, 2, NULL);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
}
