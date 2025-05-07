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

QueueHandle_t commandQueue;

SemaphoreHandle_t USBmutex;
SemaphoreHandle_t QueueMutex;