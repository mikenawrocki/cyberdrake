/**
 * A number of gestures will be used to control expression. The following
 * outlines the definitions for each gesture that will be used.
 * 
 * 1. Shaking head - gyro Z axis
 * 2. Nodding - An alternating sequence of positive and negative gyro forces
 *    in the y direction indicate nodding. This data can be corroborated with data from
 *    the accelerometer; expect accelerometer x to be positive when gyro y is negative and
 *    vice versa. If this sequence continues for a half second, a nod has been observed.
 *    If it continues for 1 second or more, a "vigorous" nod has been observed.
 * 3. Tilting head left - positive X gyro, y accel ~ 9
 * 4. Tilting head right - negative X gyro, y accel ~ -9
 * 5. Neutral bearing - none of the above conditions apply within a time threshold.
 * 
 * Possibly, gather 5 samples. Attempt to detect sequence indicating a gesture in the samples.
 * Store samples in a ring buffer. Every 3 samples, do this check. Ensures ~.3sec delay in processing.
 */
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_FXOS8700.h>
#include <Adafruit_FXAS21002C.h>

#include "cyberdrake.h"
#include "gesture.h"

/* Assign a unique ID to this sensor at the same time */
Adafruit_FXOS8700 accelmag{0x8700A, 0x8700B};
/* Assign a unique ID to this sensor at the same time */
Adafruit_FXAS21002C gyro{0x0021002C};

bool init_gesture(void)
{
    if(!accelmag.begin(ACCEL_RANGE_4G)) {
        return false;
    }
    if (!gyro.begin()) {
        return false;
    }

    state.gyro_buf.head_idx = state.gyro_buf.tail_idx = 0;
    memset(&state.gyro_buf.events, 0x00,
            sizeof(sensors_event_t)*SENSOR_EVENT_RINGBUF_SIZE);
    state.accel_buf.head_idx = state.accel_buf.tail_idx = 0;
    memset(&state.accel_buf.events, 0x00,
            sizeof(sensors_event_t)*SENSOR_EVENT_RINGBUF_SIZE);

    return true;
}

void poll_gesture_sensors(void)
{
    struct sensor_event_ringbuf* erb = &state.gyro_buf;
    gyro.getEvent(&(erb->events[erb->head_idx]));
    if (++erb->head_idx >= SENSOR_EVENT_RINGBUF_SIZE) {
        erb->head_idx = 0;
    }

    sensors_event_t mevent;  // XXX Currently ignoring magnetic information.
    erb = &state.accel_buf;
    accelmag.getEvent(&(erb->events[erb->head_idx]), &mevent);
    if (++erb->head_idx >= SENSOR_EVENT_RINGBUF_SIZE) {
        erb->head_idx = 0;
    }
}

static inline sensors_event_t* ringbuf_get_idx(unsigned int idx,
        sensor_event_ringbuf* buf)
{
    unsigned int buf_idx = (buf->tail_idx + idx) % SENSOR_EVENT_RINGBUF_SIZE;
    return &(buf->events[buf_idx]);
}

static inline float sensor_offset_get(sensors_event_t* ev, size_t sense_val_off)
{
    return *(float *)((char *)ev + sense_val_off);
}


static unsigned int count_inflections(size_t sense_val_off, float threshold,
        sensor_event_ringbuf* buf)
{
    unsigned int inflection_count = 0;
    float last_major_val = NAN;
    for (int i = 0; i < SENSOR_EVENT_RINGBUF_SIZE; ++i) {
        sensors_event_t* ev = ringbuf_get_idx(i, buf);
        float sense_data = sensor_offset_get(ev, sense_val_off);

        if (abs(sense_data) < threshold) {
            continue;
        }

        if (isnan(last_major_val)) {
            last_major_val = sense_data;
            continue;
        }
        if ((sense_data > 0.0 && last_major_val < 0.0) ||
                (sense_data < 0.0 && last_major_val > 0.0)) {  // Inflection observed
            ++inflection_count;      
        }
    }

    return inflection_count;
}


static bool detect_nod(void)
{
    const static float nod_gyro_thresh = 0.20;
    const size_t gyro_y_off = offsetof(sensors_event_t, gyro.y);
    unsigned int inflection_count = count_inflections(gyro_y_off,
            nod_gyro_thresh,
            &state.gyro_buf);
    return (inflection_count >= 2);
}

static bool detect_shake(void)
{
    const static float shake_gyro_thresh = 0.20;
    const size_t gyro_z_off = offsetof(sensors_event_t, gyro.z);
    unsigned int inflection_count = count_inflections(gyro_z_off,
            shake_gyro_thresh,
            &state.gyro_buf);
    return (inflection_count >= 2);   
}

// 0 = left, 1 = right
static bool detect_headtilt_dir(int dir)
{
    const static float accel_thresh = 8.0;
    const static float gyro_x_thresh = 0.10;

    float avg_y_accel = 0.0;
    bool gyro_x_meets_thresh = false;

    for (int i = 0; i < 5; ++i) {
        sensors_event_t* ev = ringbuf_get_idx(i, &state.accel_buf);
        avg_y_accel += ev->acceleration.y;
        ev = ringbuf_get_idx(i, &state.gyro_buf);
        if ((dir && -ev->gyro.x >= gyro_x_thresh) ||
                (!dir && ev->gyro.x >= gyro_x_thresh)) {
            gyro_x_meets_thresh = true;
        }
    }
    avg_y_accel /= 5.0;

    bool meets_thresh = (abs(avg_y_accel) > accel_thresh) && gyro_x_meets_thresh;
    bool neg_avg = (avg_y_accel < 0.0);

    return (dir) ? (meets_thresh && neg_avg) : (meets_thresh && !neg_avg);    
}

static bool detect_headtilt_l(void)
{
    return detect_headtilt_dir(0); 
}


static bool detect_headtilt_r(void)
{
    return detect_headtilt_dir(1); 
}

static void advance_ringbuf_tail(sensor_event_ringbuf* buf, int num)
{
    buf->tail_idx = (buf->tail_idx + num) % SENSOR_EVENT_RINGBUF_SIZE;
}

gesture process_gesture_data(void)
{
    gesture gst = GESTURE_NEUTRAL;

    if (detect_nod()) {
        gst |= GESTURE_NOD;
    } else if (detect_shake()) {
        gst |= GESTURE_SHAKE;
    } else if (detect_headtilt_l()) {
        gst |= GESTURE_TILT_L;
    } else if (detect_headtilt_r()) {
        gst |= GESTURE_TILT_R;
    }

    advance_ringbuf_tail(&state.gyro_buf, 5);
    advance_ringbuf_tail(&state.accel_buf, 5);

    return gst;
}
