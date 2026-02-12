#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "hardware/pio.h"
#include "hardware/dma.h"
#include <stdint.h>
#include <stdbool.h>

// LED controller initialization and control
typedef struct {
    PIO pio;
    uint pio_sm;
    uint pio_offset;
    uint pin;
    uint num_pixels;
    uint num_patterns;
    bool is_initialized;
    bool is_buffered;
    uint num_buffers;
    uint32_t** buffer;
    uint current_buffer_index;
    uint current_buffer;
    pattern_func *pattern_generator;
    void *pattern_config;
    bool needs_pattern_update;
} led_controller_t;


const int MAX_ALLOCATABLE_BUFFER_SIZE = 128 * 1024;
const int MAX_BUFFERS = MAX_ALLOCATABLE_BUFFER_SIZE / sizeof(uint32_t);

// Controller functions
bool led_controller_init(led_controller_t* controller, uint pin, uint32_t freq, bool is_rgbw, bool buffered, uint num_pixels);
void led_controller_set_pixel(led_controller_t* controller, uint32_t color);
void led_controller_set_all_pixels(led_controller_t* controller, uint32_t color, uint num_pixels);
void led_controller_set_buffer(led_controller_t* controller, const uint32_t* colors, uint num_pixels);
void led_controller_cleanup(led_controller_t* controller);
void led_controller_flush_buffer(led_controller_t* controller);

// pattern generation
typedef void (*pattern_func)(led_controller_t* controller, uint t);

typedef struct {
    pattern_func pat;
    const char *name;
} pattern;

#endif // LED_CONTROLLER_H