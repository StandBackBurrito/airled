#ifndef BUTTON_H
#define BUTTON_H

#include "pico/stdlib.h"
#include "pico/time.h"

typedef void (*button_press_callback_t)(void);

typedef struct {
    uint pin;
    button_press_callback_t callback;
    volatile absolute_time_t last_interrupt_time;
    volatile bool interrupt_enabled;
    bool button_pressed;
    bool last_button_state;
    absolute_time_t last_poll_time;
    bool poll_initialized;
} button_t;

void button_init(button_t* button, uint pin);
void button_set_callback(button_t* button, button_press_callback_t callback);
void button_enable_interrupts(button_t* button);
void button_disable_interrupts(button_t* button);
void button_check_state(button_t* button, int t, const char* context);

#endif // BUTTON_H