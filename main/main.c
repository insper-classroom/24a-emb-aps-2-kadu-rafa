#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <string.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/adc.h"

#include "hc06.h"

#include "control.h"

QueueHandle_t QueueData;
QueueHandle_t QueueBTN;

void gpio_callback(uint gpio, uint32_t events) {
    package data;
    if (events == 0x4) data.val = 1;
    else if (events == 0x8) data.val = 0;

    if (gpio == ANL_BTN_PIN) { 
        data.id = ANL_BTN_ID;
    } else if (gpio == LARGE_LEFT_BTN_PIN) { 
        data.id = LARGE_LEFT_BTN_ID;
    } else if (gpio == LARGE_RIGHT_BTN_PIN) { 
        data.id = LARGE_RIGHT_BTN_ID;
    } else if (gpio == RED_BTN_PIN) { 
        data.id = RED_BTN_ID;
    } else if (gpio == GREEN_BTN_PIN) { 
        data.id = GREEN_BTN_ID;
    } else if (gpio == BLUE_BTN_PIN) { 
        data.id = BLUE_BTN_ID;
    }  else if (gpio == YEllOW_BTN_PIN) { 
        data.id = YEllOW_BTN_ID;
    } else if (gpio == BLACK_BTN_PIN) { 
        data.id = BLACK_BTN_ID;
    }
    xQueueSendFromISR(QueueBTN, &data, 0);
}

void write_package(package data) {
    int msb = data.val >> 8;
    int lsb = data.val & 0xFF ;

    uart_putc_raw(HC06_UART_ID, data.id);
    uart_putc_raw(HC06_UART_ID, lsb);
    uart_putc_raw(HC06_UART_ID, msb);
    uart_putc_raw(HC06_UART_ID, -1);
}

void hc06_task(void *p) {
    
    printf("bluetooth booting...\n");
    hc06_init("diabloIV_control", "1234");
    printf("bluetooth initialized!\n\n");

    package data;

    while (true) {
        if (xQueueReceive(QueueData, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
            write_package(data);
            printf("%d %d\n", data.id, data.val);
        }
    }
}

void adc_task(void *p) {
    adc_task_arg *arg = (adc_task_arg *) p;

    adc_init();
    adc_gpio_init(arg->pin);

    int results[5] = {0, 0, 0, 0, 0};
    package data = {arg->adc, 0};
    int i = 0;

    vTaskDelay(pdMS_TO_TICKS(10));
    while (true) {
        adc_select_input(arg->adc); 
        results[(i++)%5] = adc_read();
        int value = (((results[0] + results[1] + results[2] + results[3] + results[4])/5) - 2047)/32;

        if (abs(value) >= 8) {
            data.val = value;
            xQueueSend(QueueData, &data, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void btn_task(void *p) {
    int btn_list[8] = {LARGE_LEFT_BTN_PIN, LARGE_RIGHT_BTN_PIN, RED_BTN_PIN, GREEN_BTN_PIN, BLUE_BTN_PIN, YEllOW_BTN_PIN, BLACK_BTN_PIN, ANL_BTN_PIN};
    for (int i=0; i < 8; i++) {
        if (i == 0) init_btn(btn_list[i], &gpio_callback);
        else init_btn(btn_list[i], NULL);
    }

    package data;
    int last_time = 0;
    package last_btn = {-1, -1};

    while (true) {
        if (xQueueReceive(QueueBTN, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
            int now =  to_ms_since_boot(get_absolute_time());
            if (data.id != last_btn.id || data.val != last_btn.val || now - last_time >= 300) {
                last_time = now;
                last_btn.id = data.id;
                last_btn.val = data.val;
                xQueueSend(QueueData, &data, 0);
            }
        }
    }
}

int main() {
    stdio_init_all();

    QueueData = xQueueCreate(32, sizeof(package));
    QueueBTN = xQueueCreate(32, sizeof(package));

    xTaskCreate(hc06_task, "UART Task", 4096, NULL, 2, NULL);

    adc_task_arg anlX = {ANL_X_PIN, ANL_X_ADC};
    xTaskCreate(adc_task, "ANL X Task", 4096, &anlX, 1, NULL);
    adc_task_arg anlY = {ANL_Y_PIN, ANL_Y_ADC};
    xTaskCreate(adc_task, "ANL Y Task", 4096, &anlY, 1, NULL);

    xTaskCreate(btn_task, "BTN Task", 1028, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true);
}
