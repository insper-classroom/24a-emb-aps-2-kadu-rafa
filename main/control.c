#include "control.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hc06.h"

void write_package(uart_inst_t *uart, package data) {
    int msb = data.val >> 8;
    int lsb = data.val & 0xFF ;

    uart_putc_raw(uart, data.id);
    uart_putc_raw(uart, lsb);
    uart_putc_raw(uart, msb);
    uart_putc_raw(uart, -1);
    printf("id: %d value: %d\n", data.id, data.val);
}

void gpio_config(int gpio, int direction, bool pull_up, gpio_irq_callback_t callback) {
    gpio_init(gpio);
    gpio_set_dir(gpio, direction);
    if (direction == GPIO_IN) {
        if (pull_up) gpio_pull_up(gpio);
        if (callback != NULL) gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, callback);
        else gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    }
}

void init_rgb_led() {
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
}

void set_rgb_led(int r, int g, int b) {
    gpio_put(LED_G_PIN, g);
    gpio_put(LED_R_PIN, r);
    gpio_put(LED_B_PIN, b);
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