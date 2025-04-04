#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "message_buffer.h"
#include "semphr.h"

#include "inc/defines.h"
#include "inc/tasks.h"
#include "inc/queues.h"
#include "inc/pwm.h"






void main() {
    stdio_init_all();  // Initializes USB serial communication
    busy_wait_ms(1000);  // Delay for stabilization

    // Mutex for handling usb serial channel
    USBmutex = xSemaphoreCreateMutex();


    commandQueue = xQueueCreate(10, sizeof(char[BUFFER_SIZE]));

    // Create tasks
    // xTaskCreate(vBlinkTask, "blink task", 128, (void *)BUFFER_SIZE, 1, NULL);
    xTaskCreate(vReceiverTask, "Receiver Task", 128, (void *)BUFFER_SIZE, 1, NULL);
    xTaskCreate(vCommandRunTask, "CommandRun", 4096, NULL, 1, NULL);
    xTaskCreate(vPrintAliveTask, "PrintAlive", 128, NULL, 3, NULL);

    
    // Start the scheduler
    vTaskStartScheduler();
}
