#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pico/cyw43_arch.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "tusb.h"

#define LED_PIN PICO_DEFAULT_LED_PIN
#define SERVER_IP "192.168.0.137"
#define SERVER_PORT 8080

void vBlinkTask(void *pvParameters) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for (;;) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void wifiInit() {
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed!\n");
        return;
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Failed to connect to Wi-Fi.\n");
        return;
    }
    printf("Connected to Wi-Fi!\n");
}

void tcpClientTask(void *pvParameters) {
    wifiInit();
    
    while (1) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("Socket creation failed.\n");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            printf("Connection failed.\n");
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        printf("Connected to server!\n");

        char *message = "Hello from FreeRTOS Client!";
        send(sock, message, strlen(message), 0);
        printf("Message sent to server.\n");

        char buffer[128] = {0};
        int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len > 0) {
            buffer[len] = '\0';
            printf("Received from server: %s\n", buffer);
        }

        close(sock);
        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait before reconnecting
    }
}

int main() {
    stdio_init_all();
    xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);
    xTaskCreate(tcpClientTask, "TCP Client Task", 512, NULL, 2, NULL);
    vTaskStartScheduler();
}
