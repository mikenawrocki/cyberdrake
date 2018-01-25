#ifndef CYBERDRAKE_H
#define CYBERDRAKE_H

#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>

#define HEAD_NP_PIN 6
#define JAW_PIN 8

#define HEAD_NP_NUM_PIXELS 33

#define EYE_NP_NUM 12
#define EYE_L_NP_OFF 0
#define EYE_R_NP_OFF 12
#define HORN_OFFSET 24
#define HORN_NUM 3
#define JAW_U_CAN_OFFSET 27
#define JAW_U_CAN_NUM 2
#define JAW_U_OFFSET 29
#define JAW_U_NUM 2
#define JAW_L_OFFSET 31
#define JAW_L_NUM 2

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
    mood mood;
    uint32_t disp_color;
    sensor_event_ringbuf gyro_buf;
    sensor_event_ringbuf accel_buf;
};

extern struct cyberdrake_state state;
extern Adafruit_NeoPixel head_np;

#endif
