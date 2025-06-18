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


// Remember to compile with "bash compile.sh" and flash with "bash run.sh" when you make changes to the code


void main() {
    stdio_init_all();  // Initializes USB serial communication
    busy_wait_ms(1000);  // Delay for stabilization

    // Mutex for handling usb serial channel and the commandQueue
    USBmutex = xSemaphoreCreateMutex();     // Mutex to protect the serial
    QueueMutex = xSemaphoreCreateMutex();   // Mutex to protect the commandQueue

    commandQueue = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(char[BUFFER_SIZE]));     // The commandQueue that stores all received messages

    // The following is all the tasks that is running on the pico. 
    // Comment out a task to stop it from running

    // xTaskCreate(vBlinkTask, "blink task", 128, (void *)BUFFER_SIZE, 1, NULL);
    xTaskCreate(vReceiverTask, "Receiver Task", 128, (void *)BUFFER_SIZE, 3, NULL);
    xTaskCreate(vCommandRunTask, "CommandRun", 4096, NULL, 2, NULL);
    xTaskCreate(vPrintAliveTask, "PrintAlive", 4096, NULL, 4, NULL);
    xTaskCreate(vQueuePeekerTask, "Queue Peeker", 4096, NULL, 3, NULL);
    
    // Start the scheduler
    vTaskStartScheduler();
}
