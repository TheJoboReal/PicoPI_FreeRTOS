#ifndef DEFINES_H
#define DEFINES_H


#define PICO_DEFAULT_LED_PIN 0
#define COMMAND_QUEUE_SIZE 16

uint pins[] = {0, 1, 2, 3, 4, 5, 6, 7};

QueueHandle_t commandQueue;


#define STOP_COMMAND 0
#define DRV_COMMAND 1
#define STATUS_COMMAND 2

const size_t BUFFER_SIZE = 64;
volatile bool timeOutFlag = false;
volatile int timeOut = 50;
volatile int timer = 0;

#endif //DEFINES_H