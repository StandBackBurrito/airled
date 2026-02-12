#ifndef PATTERNS_H
#define PATTERNS_H

#include "ws2812.h"
#include "wing_plane.h"
#include "led-controller.h"

void toggle_white_intensity(void);
void pattern_snakes(led_controller_t* controller, uint t);
void pattern_random(led_controller_t* controller, uint t);
void pattern_sparkle(led_controller_t* controller, uint t);
void pattern_greys(led_controller_t* controller, uint t);

/// @brief LED strip configuration structure
/// This structure holds configuration parameters for the LED strip,
/// including the total number of pixels, beacon size, beacon position,
/// and white LED intensity.
typedef struct {
    int pixelCount; // total number of pixels
    int beaconSize; // in pixels
    int beaconPosition; // index of first beacon pixel
    int whiteIntensity; // white LED intensity (0-255)
} led_strip;

extern const pattern pattern_table[];
extern const int pattern_table_size;

#endif
