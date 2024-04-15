#ifndef CONTROL_H
#define CONTROL_H
#include "pico/stdlib.h"

#define abs(x) ((x < 0) ? -(x) : x)

typedef struct {
    int id;
    int val;
} package;


// Analógico
#define ANL_X_PIN 26
#define ANL_X_ADC 0
#define ANL_X_ID 0
#define ANL_Y_PIN 27
#define ANL_Y_ADC 1
#define ANL_Y_ID 1
#define ANL_BTN_PIN 22
#define ANL_BTN_ID 2

//Led RGB
#define LED_R_PIN 6
#define LED_G_PIN 7
#define LED_B_PIN 8

typedef struct {
    int pin;
    int adc;
} adc_task_arg;

void init_led(int gpio);
void init_btn(int gpio, gpio_irq_callback_t callback);


#endif