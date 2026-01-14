#ifndef COLOR_TEST_H
#define COLOR_TEST_H

#include <stdint.h>
#include "led-controller.h"

// Color test functionality
void color_test_init(void);
void color_test_advance_phase(void);
uint32_t color_test_get_current_color(void);
void color_test_run(led_controller_t* controller, uint num_pixels);
const char* color_test_get_phase_name(void);
int color_test_get_phase(void);

#endif // COLOR_TEST_H