#ifndef PATTERNS_H
#define PATTERNS_H

#include "ws2812.h"

void toggle_white_intensity(void);
void pattern_snakes(PIO pio, uint sm, uint len, uint t);
void pattern_random(PIO pio, uint sm, uint len, uint t);
void pattern_sparkle(PIO pio, uint sm, uint len, uint t);
void pattern_greys(PIO pio, uint sm, uint len, uint t);
void pattern_single_wing_plane(PIO pio, uint sm, uint len, uint t);

// Pattern setup function
void setup_plane(void);

typedef void (*pattern_func)(PIO pio, uint sm, uint len, uint t);

typedef struct {
    pattern_func pat;
    const char *name;
} pattern;

extern const pattern pattern_table[];
extern const int pattern_table_size;

#endif
