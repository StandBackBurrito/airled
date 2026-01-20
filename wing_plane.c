#include <stdlib.h>
#include <stdio.h>
#include "wing_plane.h"

// Global wing plane configuration instance
wing_plane_config wing_plane;

void wing_plane_init(wing_plane_config *config, int numPixels, int pixelDensityM, int beaconSizeMm) {
    config->beaconSize = beaconSizeMm * pixelDensityM / 1000;
    config->farBeaconStart = numPixels - config->beaconSize;
    config->nonBeaconLeds = numPixels - (config->beaconSize * 2);
    config->center = numPixels / 2;
    config->centerCount = (numPixels % 2 == 0) ? 2 : 1;
    config->wingLedCount = (config->nonBeaconLeds - config->centerCount) / 2;
    config->numberOfColors = 5;
    config->whiteIntensity = 0;

    // Allocate and generate colors
    config->colors = malloc(sizeof(uint32_t) * config->numberOfColors);
    if (config->colors != NULL) {
        generate_vibrant_colors(config->colors, config->numberOfColors);
    }

    printf("Wing Plane - Beacon size: %d, Far beacon start: %d, Non-beacon LEDs: %d, Center count: %d, Wing LED count: %d, Number of colors: %d\n",
           config->beaconSize, config->farBeaconStart, config->nonBeaconLeds, config->centerCount, config->wingLedCount, config->numberOfColors);
}

void wing_plane_cleanup(wing_plane_config *config) {
    if (config->colors != NULL) {
        free(config->colors);
        config->colors = NULL;
    }
}

void wing_plane_toggle_white_intensity(wing_plane_config *config) {
    config->whiteIntensity = config->whiteIntensity == 0 ? 0xFF : 0;
    printf("Wing plane white intensity toggled to %d\n", config->whiteIntensity);
}

void pattern_single_wing_plane(led_controller_t* controller, uint t) {
    for (uint i = 0; i < controller->num_pixels; ++i) {
        if (i < wing_plane.beaconSize) {
            // Near beacon (red)
            led_controller_set_pixel(controller, urgbw_u32(0xff, 0, 0, wing_plane.whiteIntensity));
        }
        else if (i >= wing_plane.farBeaconStart) {
            // Far beacon (green)
            led_controller_set_pixel(controller, urgbw_u32(0, 0xff, 0, wing_plane.whiteIntensity));
        }
        else if (i > wing_plane.center && i <= wing_plane.center + wing_plane.centerCount) {
            // Center navigation light (white)
            led_controller_set_pixel(controller, urgbw_u32(0xff, 0xff, 0xff, 0xff));
        }
        else {
            // Wing lights - alternating colors from center outward
            int wingPixel = i < wing_plane.center
                ? wing_plane.center - i - 1
                : i - wing_plane.center - 1;
            int colorIndex = wingPixel % wing_plane.numberOfColors;

            if (wing_plane.colors != NULL) {
                led_controller_set_pixel(controller, wing_plane.colors[(colorIndex + t) % wing_plane.numberOfColors] | wing_plane.whiteIntensity);
            } else {
                led_controller_set_pixel(controller, 0); // Fallback if colors not allocated
            }
        }
    }
}