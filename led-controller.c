#include "led-controller.h"
#include "airled.pio.h"
#include "ws2812.h"
#include <stdio.h>

bool led_controller_init(led_controller_t* controller, uint pin, uint32_t freq, bool is_rgbw, bool buffered, uint num_pixels, uint num_patterns) {
    if (!controller) {
        return false;
    }

    printf("Attempting to initialize LED controller on pin %d...\n", pin);

    // Initialize the controller structure
    controller->pin = pin;
    controller->is_initialized = false;
    controller->is_buffered = buffered;
    controller->num_pixels = num_pixels;
    controller->num_patterns = num_patterns;

    // This will find a free pio and state machine for our program and load it for us
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &airled_program,
        &controller->pio,
        &controller->pio_sm,
        &controller->pio_offset,
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
           controller->pio, controller->pio_sm, controller->pio_offset);

    if (buffered) {
        size_t string_size = sizeof(uint32_t) * num_pixels;
        size_t buffer_size = sizeof(uint32_t) * num_pixels * num_patterns;
        controller->num_buffers = (buffer_size <= MAX_ALLOCATABLE_BUFFER_SIZE)
            ? num_patterns
            : (MAX_ALLOCATABLE_BUFFER_SIZE / num_pixels) * num_pixels;

        controller->buffer = malloc(sizeof(uint32_t*) * controller->num_buffers);
        for (uint i = 0; i < controller->num_buffers; ++i) {
            controller->buffer[i] = malloc(string_size);
            if (!controller->buffer[i]) {
                printf("ERROR: Failed to allocate memory for LED buffer %d!\n", i);
                return false;
            }
        }

        if (!controller->buffer) {
            printf("ERROR: Failed to allocate memory for LED buffers!\n");
            return false;
        }

        controller->current_buffer_index = 0;
        controller->current_buffer = 0;

        int dma_channel = dma_claim_unused_channel(true);
        if (dma_channel < 0) {
            printf("ERROR: Failed to claim DMA channel for LED buffering!\n");
            return false;
        }

        printf("Buffering enabled. DMA channel %d claimed for LED controller.\n", dma_channel);

        dma_channel_config cfg = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);
        channel_config_set_read_increment(&cfg, true);
        channel_config_set_write_increment(&cfg, false);
        channel_config_set_dreq(&cfg, pio_get_dreq(controller->pio, controller->pio_sm, true));
        dma_channel_configure(
            dma_channel,
            &cfg,
            &controller->pio->txf[controller->pio_sm],
            controller->buffer[controller->current_buffer],
            num_pixels,
            false
        );
    }

    airled_program_init(controller->pio, controller->pio_sm, controller->pio_offset, pin, freq, is_rgbw);
    controller->is_initialized = true;
    controller->needs_pattern_update = true;

    printf("LED controller initialized successfully.\n");
    return true;
}

void led_controller_set_pixel(led_controller_t* controller, uint32_t color) {
    if (!controller || !controller->is_initialized) {
        return;
    }

    if (controller->is_buffered) {
        controller->buffer[controller->current_buffer][controller->current_buffer_index++]
        = color;
        if (controller->current_buffer_index >= controller->num_pixels) {
            led_controller_flush_buffer(controller);
        }
    } else {
        put_pixel(controller->pio, controller->pio_sm, color);
    }
}

void led_controller_set_all_pixels(led_controller_t* controller, uint32_t color, uint num_pixels) {
    if (!controller || !controller->is_initialized) {
        return;
    }

    for (uint i = 0; i < num_pixels; ++i) {
        put_pixel(controller->pio, controller->pio_sm, color);
    }
}

void led_controller_flush_buffer(led_controller_t* controller) {
    if (!controller || !controller->is_initialized || !controller->is_buffered) {
        return;
    }

    for (uint i = 0; i < controller->current_buffer_index; ++i) {
        put_pixel(controller->pio, controller->pio_sm, controller->buffer[controller->current_buffer][i]);
    }

    controller->current_buffer_index = 0;
    controller->current_buffer = (controller->current_buffer + 1) % controller->num_buffers;
}

void led_controller_cleanup(led_controller_t* controller) {
    if (controller && controller->is_initialized) {
        pio_remove_program_and_unclaim_sm(&airled_program, controller->pio, controller->pio_sm, controller->pio_offset);
        controller->is_initialized = false;
        printf("LED controller cleaned up.\n");
    }
}