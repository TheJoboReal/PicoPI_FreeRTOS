#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN 0  // Built-in LED on Raspberry Pi Pico

void vBlinkTask() {
    for (;;) {
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);

        gpio_put(LED_PIN, 1);
        vTaskDelay(1000);
        gpio_put(LED_PIN, 0);
        vTaskDelay(1000);
    }
}

void main() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);
    vTaskStartScheduler();
}
