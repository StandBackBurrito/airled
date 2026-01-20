#include <stdlib.h>
#include <stdio.h>
#include "patterns.h"
#include "wing_plane.h"

uint8_t whiteIntensity = 0;

void toggle_white_intensity() {
    whiteIntensity = whiteIntensity == 0 ? 0xFF : 0;
    printf("Toggled white intensity to %d\n", whiteIntensity);
}

const pattern pattern_table[] = {
    { pattern_snakes,  "Snakes!" },
    { pattern_random,  "Random data" },
    { pattern_sparkle, "Sparkles" },
    { pattern_greys,   "Greys" },
    { pattern_single_wing_plane, "Single wing plane" },
};

const int pattern_table_size = sizeof(pattern_table) / sizeof(pattern_table[0]);

void pattern_snakes(led_controller_t* controller, uint t) {
    for (uint i = 0; i < controller->num_pixels; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            led_controller_set_pixel(controller, urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            led_controller_set_pixel(controller, urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            led_controller_set_pixel(controller, urgb_u32(0, 0, 0xff));
        else
            led_controller_set_pixel(controller, 0);
    }
}

void pattern_random(led_controller_t* controller, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < controller->num_pixels; ++i)
        led_controller_set_pixel(controller, rand());
}

void pattern_sparkle(led_controller_t* controller, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < controller->num_pixels; ++i)
        led_controller_set_pixel(controller, rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(led_controller_t* controller, uint t) {
    uint max = 100; // let's not draw too much current!
    t %= max;
    for (uint i = 0; i < controller->num_pixels; ++i) {
        led_controller_set_pixel(controller, t * 0x10101);
        if (++t >= max) t = 0;
    }
}

