#ifndef PATTERNS_H
#define PATTERNS_H

#include "ws2812.h"

void pattern_snakes(PIO pio, uint sm, uint len, uint t);
void pattern_random(PIO pio, uint sm, uint len, uint t);
void pattern_sparkle(PIO pio, uint sm, uint len, uint t);
void pattern_greys(PIO pio, uint sm, uint len, uint t);

typedef void (*pattern_func)(PIO pio, uint sm, uint len, uint t);

typedef struct {
    pattern_func pat;
    const char *name;
} pattern;

extern const pattern pattern_table[]; // This will be defined in patterns.c
extern const int pattern_table_size; // This will be defined in patterns.c

#endif
