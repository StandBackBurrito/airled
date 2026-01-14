#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "hardware/pio.h"
#include <stdint.h>
#include <stdbool.h>

// LED controller initialization and control
typedef struct {
    PIO pio;
    uint sm;
    uint offset;
    uint pin;
    bool is_initialized;
} led_controller_t;

// Controller functions
bool led_controller_init(led_controller_t* controller, uint pin, uint32_t freq, bool is_rgbw);
void led_controller_set_pixel(led_controller_t* controller, uint32_t color);
void led_controller_set_all_pixels(led_controller_t* controller, uint32_t color, uint num_pixels);
void led_controller_cleanup(led_controller_t* controller);

#endif // LED_CONTROLLER_H