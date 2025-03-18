#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "message_buffer.h"
#include "inc/defines.h"
#include "inc/functions.h"
#include "inc/queues.h"





void main() {
    stdio_init_all();  // Initializes USB serial communication
    busy_wait_ms(1000);  // Delay for stabilization

    commandBufferQueue = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(int32_t));

    // Create tasks
    xTaskCreate(vBlinkTask, "blink task", 128, (void *)BUFFER_SIZE, 2, NULL);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
}
