#include "color-test.h"
#include "ws2812.h"
#include <stdio.h>

// Test phase control
static int current_test_phase = 0;
static const char* phase_names[] = {"RED", "GREEN", "BLUE", "WHITE", "ALL ON", "ALL OFF"};

void color_test_init(void) {
    current_test_phase = 0;
    printf("Color test initialized. Starting with %s phase.\n", phase_names[current_test_phase]);
}

void color_test_advance_phase(void) {
    current_test_phase = (current_test_phase + 1) % 6;
    printf("*** Switched to Phase %d: %s channel ***\n", current_test_phase, phase_names[current_test_phase]);
}

uint32_t color_test_get_current_color(void) {
    uint32_t color;
    switch (current_test_phase % 6) {
        case 0:
            color = urgbw_u32(0xff, 0, 0, 0); // Red channel
            break;
        case 1:
            color = urgbw_u32(0, 0xff, 0, 0); // Green channel
            break;
        case 2:
            color = urgbw_u32(0, 0, 0xff, 0); // Blue channel
            break;
        case 3:
            color = urgbw_u32(0, 0, 0, 0xff); // White channel
            break;
        case 4:
            color = 0xffffffff; // All off
            break;
        case 5:
            color = 0; // All off
            break;
        default:
            color = 0;
            break;
    }
    return color;
}

void color_test_run(led_controller_t* controller, uint num_pixels) {
    if (!controller) {
        return;
    }

    uint32_t test_color = color_test_get_current_color();
    led_controller_set_all_pixels(controller, test_color, num_pixels);
}

const char* color_test_get_phase_name(void) {
    return phase_names[current_test_phase % 6];
}

int color_test_get_phase(void) {
    return current_test_phase;
}