#ifndef WING_H
#define WING_H

#include "led-controller.h"

typedef enum {
    BEACON_AT_START,
    BEACON_AT_END
} beacon_position_t;

typedef struct {
    uint num_pixels;
    uint num_colors;
    bool has_beacon;
    beacon_position_t beacon_position;
    uint32_t beacon_color;
    uint beacon_size;
    uint32_t *colors;
    uint8_t whiteIntensity;
} wing_config;


void wing_init(wing_config *config, uint numPixels, uint pixelDensityM, uint beaconSizeMm, bool hasBeacon, beacon_position_t beaconPosition);
void wing_cleanup(wing_config *config);
void wing_toggle_white_intensity(wing_config *config);
void wing_pattern(led_controller_t* controller, uint t);

extern wing_config wing;

#endif