#include <stdlib.h>
#include <stdio.h>
#include "patterns.h"

// Pattern globals - moved from airled.c
#define PIXEL_DENSITY_M 60
#define NUM_PIXELS 60
#define BEACON_SIZE_MM 110

uint32_t *colors = NULL;
uint8_t whiteIntensity = 0;

int farBeanconStart;
int nonBeaconLeds;
int center;
int centerCount;
int wingLedCount;
int numberOfColors;
int beaconSize;

void toggle_white_intensity() {
    whiteIntensity = whiteIntensity == 0 ? 0xFF : 0;
    printf("Toggled white intensity to %d\n", whiteIntensity);
}

void setup_plane() {
    beaconSize = BEACON_SIZE_MM * PIXEL_DENSITY_M / 1000;
    farBeanconStart = NUM_PIXELS - beaconSize;
    nonBeaconLeds = NUM_PIXELS - (beaconSize * 2);
    center = (NUM_PIXELS) / 2;
    centerCount = (NUM_PIXELS % 2 == 0) ? 2 : 1;
    wingLedCount = (nonBeaconLeds - centerCount) / 2;
    numberOfColors = 5;

    colors = malloc(sizeof(uint32_t) * numberOfColors);
    generate_vibrant_colors(colors, numberOfColors);

    printf("Beacon size: %d, Far beacon start: %d, Non-beacon LEDs: %d, Center count: %d, Wing LED count: %d, Number of colors: %d\n",
           beaconSize, farBeanconStart, nonBeaconLeds, centerCount, wingLedCount, numberOfColors);
}

void pattern_single_wing_plane(PIO pio, uint sm, uint len, uint t) {
    for (uint i = 0; i < len; ++i)
    {
        if (i < beaconSize)
        {
            put_pixel(pio, sm, urgbw_u32(0xff, 0, 0, whiteIntensity));
        }
        else if (i >= farBeanconStart)
        {
            put_pixel(pio, sm, urgbw_u32(0, 0xff, 0, whiteIntensity));
        }
        else if (i > center & i <= center + centerCount)
        {
            put_pixel(pio, sm, urgbw_u32(0xff, 0xff, 0xff, 0xff));
        }
        else
        {
            /*
            This will set the pixel color based on the position of the pixel on the wing
            it will start from the center of the wing and go outwards, alternating colors
            */
            int wingPixel = i < center
                ? center - i - 1
                : i - center - 1;
            int colorIndex = wingPixel % numberOfColors;
            put_pixel(pio, sm, colors[(colorIndex + t) % numberOfColors] | (whiteIntensity));
        }
    }
}

const pattern pattern_table[] = {
    { pattern_snakes,  "Snakes!" },
    { pattern_random,  "Random data" },
    { pattern_sparkle, "Sparkles" },
    { pattern_greys,   "Greys" },
    { pattern_single_wing_plane, "Single wing plane" },
};

const int pattern_table_size = sizeof(pattern_table) / sizeof(pattern_table[0]);

void pattern_snakes(PIO pio, uint sm, uint len, uint t) {
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(pio, sm, urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(pio, sm, urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            put_pixel(pio, sm, urgb_u32(0, 0, 0xff));
        else
            put_pixel(pio, sm, 0);
    }
}

void pattern_random(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand());
}

void pattern_sparkle(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(PIO pio, uint sm, uint len, uint t) {
    uint max = 100; // let's not draw too much current!
    t %= max;
    for (uint i = 0; i < len; ++i) {
        put_pixel(pio, sm, t * 0x10101);
        if (++t >= max) t = 0;
    }
}

