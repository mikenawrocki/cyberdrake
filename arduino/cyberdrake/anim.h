#ifndef ANIM_H
#define ANIM_H

#include "cyberdrake.h"

struct animation {
    int n_frames;
    const uint8_t* const frame_durations;
    const uint8_t* const PROGMEM l_data;
    const uint8_t* const PROGMEM r_data;
};

extern struct animation mood_animation_map[N_MOODS];

#endif
