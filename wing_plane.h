#ifndef WING_PLANE_H
#define WING_PLANE_H

#include "ws2812.h"
#include "led-controller.h"

// Wing plane configuration structure
typedef struct {
    int farBeaconStart;
    int nonBeaconLeds;
    int center;
    int centerCount;
    int wingLedCount;
    int numberOfColors;
    int beaconSize;
    uint32_t *colors;
    uint8_t whiteIntensity;
} wing_plane_config;

// Initialize wing plane configuration
void wing_plane_init(wing_plane_config *config, int numPixels, int pixelDensityM, int beaconSizeMm);

// Cleanup wing plane resources
void wing_plane_cleanup(wing_plane_config *config);

// Toggle white intensity for wing plane
void wing_plane_toggle_white_intensity(wing_plane_config *config);

// Wing plane pattern function
void pattern_single_wing_plane(led_controller_t* controller, uint t);

// Global wing plane configuration instance
extern wing_plane_config wing_plane;

#endif