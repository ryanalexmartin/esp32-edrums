// Language: c
// Path: main/interrupt.c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "driver/gpio.h"

#define BTN 26
#define LED 5

QueueHandle_t interruptQueue;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    int gpio_num = (uint32_t) arg;
    xQueueSendFromISR(interruptQueue, &gpio_num, NULL);
}

void buttonTask(void* arg)
{
    int ledState = (int) arg;
    int gpio_num;

    for(;;) {
        if(xQueueReceive(interruptQueue, &gpio_num, portMAX_DELAY)) 
        {
            // Toggle led
            ledState = !ledState;
            gpio_set_level(LED, ledState);
            printf("Button pressed.  Value: %d\n", ledState);
        }
    }
}

void app_main()
{
    int *ledState = 0;

    gpio_reset_pin(BTN);
    gpio_reset_pin(LED);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN, GPIO_INTR_NEGEDGE);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN, gpio_isr_handler, (void*) BTN);
    interruptQueue = xQueueCreate(10, sizeof(int));

    xTaskCreate(buttonTask, "buttonTask", 2048, ledState, 10, NULL); 
}
