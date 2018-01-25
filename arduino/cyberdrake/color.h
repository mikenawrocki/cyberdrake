#ifndef COLOR_H
#define COLOR_H


#include <Adafruit_NeoPixel.h>

/**
 * Scale a full intensity color to a maximum amount. log_chan_max is the base 2
 * logarithm of the maximum channel value.
 */
uint32_t scaled_color(uint32_t color, int log_chan_max);

uint32_t color_wheel(Adafruit_NeoPixel* strip, uint8_t pos);

#endif
