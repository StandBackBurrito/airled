#include "hardware/pio.h"
#include <stdint.h>
#include <stdlib.h>

void put_pixel(PIO pio, uint sm, uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void generate_random_colors(uint32_t *colors, size_t length);
void generate_vibrant_colors(uint32_t *colors, size_t length);