#ifndef GESTURE_H
#define GESTURE_H

#include <Wire.h>

enum gesture {
    GESTURE_NEUTRAL = 0,
    GESTURE_NOD = 1,
    GESTURE_SHAKE = 2,
    GESTURE_TILT_L = 4,
    GESTURE_TILT_R = 8
};

bool init_gesture(void);
void poll_gesture_sensors(void);
gesture process_gesture_data(void);
#endif
