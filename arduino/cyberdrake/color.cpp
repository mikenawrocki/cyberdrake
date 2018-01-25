#include "color.h"

uint32_t scaled_color(uint32_t color, int log_chan_max)
{
    uint8_t c1, c2, c3, c4; // color channels rgb(w
    uint8_t shift = 8 - log_chan_max;
    uint8_t mask = (1 << log_chan_max) - 1;

    c1 = (color >> ( 0 + shift)) & mask;
    c2 = (color >> ( 8 + shift)) & mask;
    c3 = (color >> (16 + shift)) & mask;
    c4 = (color >> (24 + shift)) & mask;

    color = (uint32_t)c1 | ((uint32_t)c2 << 8) | ((uint32_t)c3 << 16) | (uint32_t)c4 << 24;
    return color;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// Courtesy of PaintYourDragon
uint32_t color_wheel(Adafruit_NeoPixel* strip, uint8_t pos)
{
    pos = 255 - pos;
    if(pos < 85) {
        return strip->Color(255 - pos * 3, 0, pos * 3);
    }
    if(pos < 170) {
        pos -= 85;
        return strip->Color(0, pos * 3, 255 - pos * 3);
    }
    pos -= 170;
    return strip->Color(pos * 3, 255 - pos * 3, 0);
}
