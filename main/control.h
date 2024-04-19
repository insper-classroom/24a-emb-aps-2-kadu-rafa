#ifndef CONTROL_H
#define CONTROL_H
#include "pico/stdlib.h"

#define abs(x) ((x < 0) ? -(x) : x)

typedef struct {
    int id;
    int val;
} package;

// IDS
#define ANL_X_ID 0
#define ANL_Y_ID 1
#define LARGE_LEFT_BTN_ID 2
#define LARGE_RIGHT_BTN_ID 3
#define RED_BTN_ID 4
#define GREEN_BTN_ID 5
#define BLUE_BTN_ID 6
#define YEllOW_BTN_ID 7
#define BLACK_BTN_ID 8
#define ANL_BTN_ID 9

// IDs keypad
#define KPAD_0 10
#define KPAD_1 11
#define KPAD_2 12
#define KPAD_3 13
#define KPAD_4 14
#define KPAD_5 15
#define KPAD_6 16
#define KPAD_7 17
#define KPAD_8 18
#define KPAD_9 19
#define KPAD_AST 20
#define KPAD_HASH 21

// HC06
#define STATE_PIN 9

// Analógico
#define ANL_X_PIN 26
#define ANL_X_ADC 0
#define ANL_Y_PIN 27
#define ANL_Y_ADC 1

// Led RGB
#define LED_R_PIN 6
#define LED_G_PIN 7
#define LED_B_PIN 8

// Botões
#define LARGE_LEFT_BTN_PIN 14
#define LARGE_RIGHT_BTN_PIN 15
#define RED_BTN_PIN 18
#define GREEN_BTN_PIN 19
#define BLUE_BTN_PIN 20
#define YEllOW_BTN_PIN 21
#define BLACK_BTN_PIN 17
#define ANL_BTN_PIN 16

// Teclas keypad
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

void init_led(int gpio);
void init_btn(int gpio, gpio_irq_callback_t callback);

#endif