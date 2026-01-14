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
#include "button.h"
#include "led-controller.h"
#include "color-test.h"
#include "airled.pio.h"

#define WS2812_PIN 1
#define BUTTON_PIN 27
#define IS_RGBW true
#define NUM_PIXELS 60

// Global LED controller
static led_controller_t led_controller;

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

    printf("About to initialize button...\n");
    button_init(BUTTON_PIN);
    printf("Button initialized successfully\n");

    printf("About to initialize LED controller...\n");
    if (!led_controller_init(&led_controller, WS2812_PIN, 800000, IS_RGBW)) {
        printf("Failed to initialize LED controller. System halted.\n");
        while (1) {
            printf("LED initialization failed. System halted.\n");
            sleep_ms(5000);
        }
    }
    printf("LED controller initialization completed\n");

    printf("About to initialize color test...\n");
    color_test_init();
    printf("Color test initialized\n");

    button_set_callback(color_test_advance_phase);
    printf("Channel mapping test enabled. Press button to cycle through R, G, B, W, OFF.\n");

    // Main loop
    int t = 0;
    printf("Starting button-controlled channel mapping test...\n");
    printf("Press button to cycle: RED -> GREEN -> BLUE -> WHITE -> OFF -> RED...\n");

    while (1) {
        button_check_state(t, "channel test");
        color_test_run(&led_controller, NUM_PIXELS);
        sleep_ms(50);
        t++;

        if (t % 100 == 0) {
            printf("Heartbeat: t=%d\n", t);
        }
    }

    led_controller_cleanup(&led_controller);
    return 0;
}