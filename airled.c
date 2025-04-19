/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "patterns.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "airled.pio.h"

#define WS2812_PIN 1
#define IS_RGBW true

#define PIXEL_DENSITY_M 60
#define NUM_PIXELS 60

#define BEACON_SIZE_MM 110

uint32_t *colors = NULL;

int farBeanconStart;
int nonBeaconLeds;
int center;
int centerCount;
int wingLedCount;
int numberOfColors;
int beaconSize;

// Check the pin is compatible with the platform
#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

static inline void setup_plane()
{
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

void pattern_single_wing_plane(PIO pio, uint sm, uint len, uint t)
{
    for (uint i = 0; i < len; ++i)
    {
        if (i < beaconSize)
        {
            put_pixel(pio, sm, urgb_u32(0xff, 0, 0)); // Red
            printf("Index: %d, Color: Red\n", i);
        }
        else if (i >= farBeanconStart)
        {
            put_pixel(pio, sm, urgb_u32(0, 0xff, 0)); // Green
            printf("Index: %d, Color: Green\n", i);
        }
        else if (i > center & i <= center + centerCount)
        {
            put_pixel(pio, sm, urgb_u32(0xff, 0xff, 0xff));
            printf("Index: %d, Color: White\n", i);
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
            put_pixel(pio, sm, colors[(colorIndex + t) % numberOfColors]);
            printf("Index: %d, Wing pixel: %d, Color index: %d\n", i , wingPixel, colorIndex);
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

int main() {
    //set_sys_clock_48();
    stdio_init_all();

    int timeout_ms = 1000; // Wait up to 5 seconds
    int waited = 0;
    while (!stdio_usb_connected() && waited < timeout_ms)
    {
        sleep_ms(25);
        waited += 25;
    }

    printf("Waited %d ms for USB serial connection\n", waited);
    printf("WS2812 Smoke Test, using pin %d\n", WS2812_PIN);

    setup_plane(); // Setup the plane for the LED patterns

    // todo get free sm
    PIO pio;
    uint sm;
    uint offset;

    // This will find a free pio and state machine for our program and load it for us
    // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
    // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&airled_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);

    airled_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 0;
    while (1) {
        int pat = rand() % count_of(pattern_table);
        int dir = (rand() >> 30) & 1 ? 1 : -1;
        puts(pattern_table[pat].name);
        puts(dir == 1 ? "(forward)" : "(backward)");
        for (int i = 0; i < 1000; ++i) {
            //pattern_table[pat].pat(pio, sm, NUM_PIXELS, t);
            pattern_single_wing_plane(pio, sm, NUM_PIXELS, t);
            sleep_ms(150);
            t += dir;
        }
    }

    // This will free resources and unload our program
    pio_remove_program_and_unclaim_sm(&airled_program, pio, sm, offset);
}
