#include "wing.h"

void wing_init(wing_config *config, uint numPixels, uint pixelDensityM, uint beaconSizeMm, bool hasBeacon, beacon_position_t beaconPosition) {
    config->num_pixels = numPixels;
    config->has_beacon = hasBeacon;
    config->beacon_position = beaconPosition;
    config->beacon_size = hasBeacon ? (beaconSizeMm * pixelDensityM / 1000) : 0;
    config->num_colors = 5;
    config->whiteIntensity = 0;

    // Allocate and generate colors
    config->colors = malloc(sizeof(uint32_t) * config->num_colors);
    if (config->colors != NULL) {
        generate_vibrant_colors(config->colors, config->num_colors);
    }

    printf("Wing - Beacon size: %d, Position: %d, Number of colors: %d\n",
           config->beacon_size, config->beacon_position, config->num_colors);
}

void wing_cleanup(wing_config *config) {
    if (config->colors != NULL) {
        free(config->colors);
        config->colors = NULL;
    }
}

void wing_toggle_white_intensity(wing_config *config) {
    config->whiteIntensity = config->whiteIntensity == 0 ? 0xFF : 0;
    printf("Wing white intensity toggled to %d\n", config->whiteIntensity);
}

void wing_pattern(led_controller_t* controller, uint t) {

    wing_config* config = (wing_config*)controller->pattern_config;

    for (uint i = 0; i < controller->num_pixels; ++i) {
        // Determine if this pixel is part of a beacon
        bool is_beacon_pixel = false;
        if ((controller->num_pixels >= config->beacon_size) && config->has_beacon) {
            if (config->beacon_position == BEACON_AT_START && i < config->beacon_size) {
                is_beacon_pixel = true;
            } else if (config->beacon_position == BEACON_AT_END && i >= controller->num_pixels - config->beacon_size) {
                is_beacon_pixel = true;
            }
        }

        if (is_beacon_pixel) {
            // Set beacon color
            led_controller_set_pixel(controller, config->beacon_color | config->whiteIntensity);
        } else {
            // Set wing colors - alternating colors from start to end
            int colorIndex = i % config->num_colors;

            if (config->colors != NULL) {
                led_controller_set_pixel(controller, config->colors[(colorIndex + t) % config->num_colors] | config->whiteIntensity);
            }
        }
    }
}