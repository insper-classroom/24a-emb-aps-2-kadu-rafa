#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <string.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "hc06.h"
#include "control.h"

QueueHandle_t QueueData;
QueueHandle_t QueueBTN;
QueueHandle_t QueuePADColumn;
SemaphoreHandle_t SemaphoreANL;

void gpio_callback(uint gpio, uint32_t events) {
    package data = {-1, 0};
    int column = -1;

    if (gpio == ANL_BTN_PIN) { 
        if (events == 0x4) data.val = 1;
        data.id = ANL_BTN_ID;

    } else if (events == 0x8) {
        if (gpio == BLACK_BTN_PIN) { 
            data.id = BLACK_BTN_ID;
        } else if (gpio == RED_BTN_PIN) { 
            data.id = RED_BTN_ID;
        } else if (gpio == GREEN_BTN_PIN) { 
            data.id = GREEN_BTN_ID;
        } else if (gpio == BLUE_BTN_PIN) { 
            data.id = BLUE_BTN_ID;
        }  else if (gpio == YEllOW_BTN_PIN) { 
            data.id = YEllOW_BTN_ID;
        } else if (gpio == ANL_MODE_BTN_PIN) {
            data.id = ANL_MODE_BTN_ID;
        } else if (gpio == KPAD_C1_PIN) {
            column = 0;
        } else if (gpio == KPAD_C2_PIN) {
            column = 1;
        } else if (gpio == KPAD_C3_PIN) {
            column = 2;
        }
    }   

    if (data.id != -1) xQueueSendFromISR(QueueBTN, &data, 0);
    else if (column != -1) xQueueSendFromISR(QueuePADColumn, &column, 0);
}

void wasd(package data) {
    data.id = data.id*2 + 2;
    if (abs(data.val) >= ANL_DEAD_ZONE) {
        if (data.val > 0) {
            data.val = 0;
            write_package(HC06_UART_ID, data);
            data.id++;
            data.val = 1;
            write_package(HC06_UART_ID, data);
        } else {
            data.val = 1;
            write_package(HC06_UART_ID, data);
            data.id++;
            data.val = 0;
            write_package(HC06_UART_ID, data);
        }
    } else {
        data.val = 0;
        write_package(HC06_UART_ID, data);
        data.id++;
        write_package(HC06_UART_ID, data);
    }
}

void hc06_task(void *p) {
    init_rgb_led();
    gpio_config(LED_W_PIN, GPIO_OUT, false, NULL);
    set_rgb_led(1,0,0);
    printf("bluetooth booting...\n");
    hc06_init("dontStarve_control", "1234");
    printf("bluetooth initialized!\n\n");
    set_rgb_led(0,0,1);

    package data;
    int wasd_mode = false;
    int connect = false;

    while (true) {
        if (gpio_get(HC06_STATE_PIN)) {
            set_rgb_led(0,1,0);
            connect = true;
        } else {
            set_rgb_led(0,0,1);
            connect = false;
        }

        if (xSemaphoreTake(SemaphoreANL, pdMS_TO_TICKS(1)) == pdTRUE) {
            wasd_mode = !wasd_mode;
            gpio_put(LED_W_PIN, wasd_mode);
        }
        
        if (xQueueReceive(QueueData, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (connect) {
                if (data.id < 2) {
                    if (wasd_mode) {
                        wasd(data);
                        continue;
                    } else if (abs(data.val) < ANL_DEAD_ZONE) {
                        data.val = 0;
                    }
                }
                write_package(HC06_UART_ID, data);
            } else {
                printf("no device connected\n");
            }
        }
    }
}

void serial_task(void *p) {

    package data;
    int anl_mouse = true;

    while (true) {
        if (xSemaphoreTake(SemaphoreANL, pdMS_TO_TICKS(1)) == pdTRUE) anl_mouse = !anl_mouse;
        if (xQueueReceive(QueueData, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (data.id < 2 && !anl_mouse) {
                data.val = 1;
                data.id = data.id*2 + 2;
                if (data.val > 0) data.id++;
            } 
            write_package(uart_default, data);
            printf("id: %d value: %d\n", data.id, data.val);
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

    while (true) {
        adc_select_input(arg->adc); 
        results[(i++)%5] = adc_read();
        data.val = (((results[0] + results[1] + results[2] + results[3] + results[4])/5) - 2047)/40;
        xQueueSend(QueueData, &data, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void btn_task(void *p) {
    int btn_list[6] = {BLACK_BTN_PIN, RED_BTN_PIN, GREEN_BTN_PIN, BLUE_BTN_PIN, YEllOW_BTN_PIN, ANL_MODE_BTN_PIN};
    gpio_config(ANL_BTN_PIN, GPIO_IN, true, &gpio_callback);
    for (int i=0; i < 6; i++) {
        gpio_config(btn_list[i], GPIO_IN, true, NULL);
    }

    int last_times[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    package data;

    while (true) {
        if (xQueueReceive(QueueBTN, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
            int index = 20*data.val + data.id - 6;
            int now = to_ms_since_boot(get_absolute_time());
            printf("debouncing %d %d\n", data.id, index);
            if (now - last_times[index] >= 300) {
                last_times[index] = now;
                if (data.id == ANL_MODE_BTN_ID) {
                    printf("troca, porra!\n");
                    xSemaphoreGive(SemaphoreANL);
                }else xQueueSend(QueueData, &data, 0);
            }
        }
    }
}

void keypad_task(void *p) {
    int kpad_rows[KPAD_ROWS] = {KPAD_R1_PIN, KPAD_R2_PIN, KPAD_R3_PIN, KPAD_R4_PIN};
    for(int i = 0; i < KPAD_ROWS; i++) {
        gpio_config(kpad_rows[i], GPIO_OUT, false, NULL);
    }
    int kpad_columns[kPAD_COLUMNS] = {KPAD_C1_PIN, KPAD_C2_PIN, KPAD_C3_PIN};
    for(int i = 0; i < kPAD_COLUMNS; i++) {
        gpio_config(kpad_columns[i], GPIO_IN, false, NULL);
    }
    int kpad_matrix[KPAD_ROWS][kPAD_COLUMNS] = {
        {KPAD_1,   KPAD_2,    KPAD_3},
        {KPAD_4,   KPAD_5,    KPAD_6},
        {KPAD_7,   KPAD_8,    KPAD_9},
        {KPAD_AST, KPAD_0, KPAD_HASH}
    };

    int i = 0;
    int column;

    package data = {-1, 0};

    while(true) {
        gpio_put(kpad_rows[i%4], 1);
        if(xQueueReceive(QueuePADColumn, &column, pdMS_TO_TICKS(10)) == pdTRUE) {
            data.id = kpad_matrix[i%4][column];
            xQueueSend(QueueBTN, &data, 0);
        }
        gpio_put(kpad_rows[(i++)%4], 0);
    }
}

int main() {
    stdio_init_all();

    QueueData = xQueueCreate(32, sizeof(package));
    QueueBTN = xQueueCreate(32, sizeof(package));
    QueuePADColumn = xQueueCreate(32, sizeof(int));
    SemaphoreANL = xSemaphoreCreateBinary();

    // Não use as duas Task ao mesmo tempo
    xTaskCreate(hc06_task, "UART Task", 4096, NULL, 2, NULL);
    // xTaskCreate(serial_task, "UART Task", 4096, NULL, 2, NULL); 

    adc_task_arg anlX = {ANL_X_PIN, ANL_X_ADC};
    xTaskCreate(adc_task, "ANL X Task", 4096, &anlX, 1, NULL);
    adc_task_arg anlY = {ANL_Y_PIN, ANL_Y_ADC};
    xTaskCreate(adc_task, "ANL Y Task", 4096, &anlY, 1, NULL);

    xTaskCreate(btn_task, "BTN Task", 4096, NULL, 1, NULL);
    xTaskCreate(keypad_task, "KeyPad Task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true);
}
