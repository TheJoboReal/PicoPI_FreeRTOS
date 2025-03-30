#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"

#endif // FUNCTIONS_H


void printCommand(const char* commandMessage) {
    for (int i = 0; i < strlen(commandMessage); i++) {
        printf("%c", commandMessage[i]);
    }
    printf("\n");
}