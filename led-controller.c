#include "led-controller.h"
#include "airled.pio.h"
#include "ws2812.h"
#include <stdio.h>

bool led_controller_init(led_controller_t* controller, uint pin, uint32_t freq, bool is_rgbw) {
    if (!controller) {
        return false;
    }

    printf("Attempting to initialize LED controller on pin %d...\n", pin);

    // Initialize the controller structure
    controller->pin = pin;
    controller->is_initialized = false;

    // This will find a free pio and state machine for our program and load it for us
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &airled_program,
        &controller->pio,
        &controller->sm,
        &controller->offset,
        pin,
        1,
        true
    );

    if (!success) {
        printf("ERROR: Failed to claim PIO state machine for LED controller!\n");
        printf("This might be due to insufficient PIO resources.\n");
        return false;
    }

    printf("PIO initialization successful! PIO: %p, SM: %d, Offset: %d\n",
           controller->pio, controller->sm, controller->offset);

    // Initialize the LED program
    airled_program_init(controller->pio, controller->sm, controller->offset, pin, freq, is_rgbw);
    controller->is_initialized = true;

    printf("LED controller initialized successfully.\n");
    return true;
}

void led_controller_set_pixel(led_controller_t* controller, uint32_t color) {
    if (!controller || !controller->is_initialized) {
        return;
    }
    put_pixel(controller->pio, controller->sm, color);
}

void led_controller_set_all_pixels(led_controller_t* controller, uint32_t color, uint num_pixels) {
    if (!controller || !controller->is_initialized) {
        return;
    }

    for (uint i = 0; i < num_pixels; ++i) {
        put_pixel(controller->pio, controller->sm, color);
    }
}

void led_controller_cleanup(led_controller_t* controller) {
    if (controller && controller->is_initialized) {
        pio_remove_program_and_unclaim_sm(&airled_program, controller->pio, controller->sm, controller->offset);
        controller->is_initialized = false;
        printf("LED controller cleaned up.\n");
    }
}