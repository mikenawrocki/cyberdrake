#ifndef CYBERDRAKE_H
#define CYBERDRAKE_H

#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>

#define HEAD_NP_PIN 6
#define JAW_PIN 8

#define HEAD_NP_NUM_PIXELS 24

enum mood {
    MOOD_NEUTRAL,
    MOOD_HAPPY,
    MOOD_LOVING,
    MOOD_IRRITATED,
    MOOD_ANGRY,
    MOOD_CONFUSED,
    MOOD_OWO,

    N_MOODS
};

extern const char* const mood_str_map[N_MOODS];

#define SENSOR_EVENT_RINGBUF_SIZE 5
struct sensor_event_ringbuf {
    unsigned int head_idx;
    unsigned int tail_idx;
    sensors_event_t events[SENSOR_EVENT_RINGBUF_SIZE];
};

struct cyberdrake_state {
    // Sensor information
    bool jaw_open;
    // TODO add historical gyro/accel data
    mood mood;
    sensor_event_ringbuf gyro_buf;
    sensor_event_ringbuf accel_buf;
};

extern struct cyberdrake_state state;
extern Adafruit_NeoPixel head_np;

#endif
