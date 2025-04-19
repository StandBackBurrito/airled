#include "ws2812.h"
void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}

void generate_random_colors(uint32_t *colors, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        uint8_t r = rand() % 256;
        uint8_t g = rand() % 256;
        uint8_t b = rand() % 256;
        colors[i] = urgb_u32(r, g, b);
    }
}

void generate_vibrant_colors(uint32_t *colors, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        uint8_t r, g, b;
        int max_component = rand() % 3;
        switch (max_component)
        {
            case 0:
                r = 255;
                g = rand() % 64;
                b = rand() % 128;
                break;
            case 1:
                r = rand() % 128;
                g = 255;
                b = rand() % 64;
                break;
            case 2:
                r = rand() % 64;
                g = rand() % 128;
                b = 255;
                break;
        }
        colors[i] = urgb_u32(r, g, b);
    }

    int offIndex = rand() % length;
    colors[offIndex] = urgb_u32(0, 0, 0); // Add a black color to the array
}
