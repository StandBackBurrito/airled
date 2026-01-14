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
#include "led-controller.h"
#include "button.h"

#define WS2812_PIN 1
#define BUTTON_PIN 27
#define IS_RGBW true
#define NUM_PIXELS 60

static led_controller_t led_controller;
static button_t button;
#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

void button_pressed_callback(void) {
    printf("Button pressed callback invoked. Toggling white intensity.\n");
    toggle_white_intensity();
}

int main() {
    stdio_init_all();

    printf("=== AIRLED STARTUP ===\n");
    printf("Program started successfully\n");

    int timeout_ms = 1000;
    int waited = 0;
    while (!stdio_usb_connected() && waited < timeout_ms) {
        sleep_ms(25);
        waited += 25;
    }

    printf("Waited %d ms for USB serial connection\n", waited);
    printf("WS2812 Smoke Test, using pin %d\n", WS2812_PIN);

    printf("About to setup plane patterns...\n");
    setup_plane();
    printf("Plane patterns setup completed\n");

    printf("About to initialize LED controller...\n");
    if (!led_controller_init(&led_controller, WS2812_PIN, 800000, IS_RGBW)) {
        printf("Failed to initialize LED controller. System halted.\n");
        while (1) {
            printf("LED initialization failed. System halted.\n");
            sleep_ms(5000);
        }
    }
    printf("LED controller initialization completed\n");

    printf("Starting pattern display loop...\n");

    button_init(&button, BUTTON_PIN);
    button_set_callback(&button, button_pressed_callback);
    button_enable_interrupts(&button);
    printf("Button initialized and callback set.\n");

    int t = 0;
    while (1) {
        int pat = rand() % pattern_table_size;
        int dir = (rand() >> 30) & 1 ? 1 : -1;
        printf("Selected pattern: %s %s\n", pattern_table[pat].name,
               dir == 1 ? "(forward)" : "(backward)");

        for (int i = 0; i < 1000; ++i) {
            //pattern_table[pat].pat(led_controller.pio, led_controller.sm, NUM_PIXELS, t);
            pattern_single_wing_plane(led_controller.pio, led_controller.sm, NUM_PIXELS, t);
            sleep_ms(150);
            t += dir;

            if (i % 34 == 0) {
                printf("Pattern heartbeat: %s, t=%d\n", pattern_table[pat].name, t);
            }
        }
    }

    led_controller_cleanup(&led_controller);
    return 0;
}
