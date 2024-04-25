#ifndef CONTROL_H
#define CONTROL_H
#include "pico/stdlib.h"

#define abs(x) ((x < 0) ? -(x) : x)

// IDS
#define ANL_X_ID 0
#define ANL_Y_ID 1

#define ANL_LEFT_ID 2
#define ANL_RIGHT_ID 3
#define ANL_UP_ID 4
#define ANL_DOWN_ID 5

#define ANL_BTN_ID 6
#define BLACK_BTN_ID 7

#define COLLECT_BTN_ID 8
#define ATTACK_BTN_ID 9
#define ESC_BTN_ID 10
#define ENTER_BTN_ID 11
#define ANL_MODE_BTN_ID 100

#define KPAD_0 12
#define KPAD_1 13
#define KPAD_2 14
#define KPAD_3 15
#define KPAD_4 16
#define KPAD_5 17
#define KPAD_6 18
#define KPAD_7 19
#define KPAD_8 20
#define KPAD_9 21
#define KPAD_AST 22
#define KPAD_HASH 23

// JoyStick
#define ANL_X_PIN 26
#define ANL_X_ADC 0
#define ANL_Y_PIN 27
#define ANL_Y_ADC 1

#define ANL_DEAD_ZONE 10

// Led RGB
#define LED_R_PIN 6
#define LED_G_PIN 7
#define LED_B_PIN 8
#define LED_W_PIN 28

// Botões
#define ANL_BTN_PIN 16
#define BLACK_BTN_PIN 17
#define COLLECT_BTN_PIN 18
#define ATTACK_BTN_PIN 19
#define ESC_BTN_PIN 20
#define ENTER_BTN_PIN 21
#define ANL_MODE_BTN_PIN 22

// Teclas keypad
#define KPAD_ROWS 4
#define KPAD_COLUMNS 3

#define KPAD_R1_PIN 15
#define KPAD_R2_PIN 14
#define KPAD_R3_PIN 13
#define KPAD_R4_PIN 12
#define KPAD_C1_PIN 11
#define KPAD_C2_PIN 10
#define KPAD_C3_PIN 9

typedef struct {
    int pin;
    int adc;
} adc_task_arg;

typedef struct {
    int id;
    int val;
} package;

void gpio_config(int gpio, int direction, bool pull_up, gpio_irq_callback_t callback);
void init_rgb_led();
void set_rgb_led(int r, int g, int b);
void write_package(uart_inst_t *uart, package data);
void wasd(package data);

#endif