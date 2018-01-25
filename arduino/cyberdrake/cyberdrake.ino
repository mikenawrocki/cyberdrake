#include "cyberdrake.h"
#include "eyes.h"
#include "jaws.h"
#include "color.h"
#include "horn.h"
#include "gesture.h"
#include "monitor.h"

Adafruit_NeoPixel head_np{HEAD_NP_NUM_PIXELS, HEAD_NP_PIN, NEO_GRB | NEO_KHZ800};

struct cyberdrake_state state;

static void read_jaw_sensor(void)
{
    state.jaw_open = !digitalRead(JAW_PIN);
}

static void init_jaw_sensor(void)
{
    pinMode(JAW_PIN, INPUT_PULLUP);
    read_jaw_sensor();
}

void setup()
{
    Serial.begin(9600);
    while(!Serial) {
        delay(1);
    }
    #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) {
        clock_prescale_set(clock_div_1);
    }
    #endif
    // Initializing system state
    state.jaw_open = 0;
    state.mood = MOOD_NEUTRAL;

    head_np.begin();

    init_jaw_sensor();
    init_jaws();
    init_horn();
    init_eyes();
    init_monitor();
    init_gesture();

    head_np.show();
}

static uint32_t get_next_disp_color(void)
{
    static uint8_t position = 127;
    static uint8_t ctr = 0;
    static int direction = 1;
    uint32_t next_color;

    ++ctr;

    switch (state.mood) {
    case MOOD_NEUTRAL:
        position += direction;
        if (position == 255) {
            direction = -1;
        } else if (position == 128) {
            direction = 1;
        }

        next_color = color_wheel(&head_np, position);
        break;
    case MOOD_HAPPY:
        // Green
        next_color = head_np.Color(0, 255, 63);
        break;
    case MOOD_LOVING:
        // Pink
        next_color = head_np.Color(255, 0, 127);
        break;
    case MOOD_IRRITATED:
        // Orange
        next_color = head_np.Color(255, 127,0);
        break;
    case MOOD_ANGRY:
        // Flashing red
        next_color = (ctr & 1) ? head_np.Color(255, 0, 0) : 0;
        break;
    case MOOD_CONFUSED:
    case MOOD_OWO:
        // Flashing blue
        next_color = (ctr & 1) ? head_np.Color(0, 63, 255) : 0;
        break;
    }
    return next_color;
}

static void update_display(void)
{
    state.disp_color = get_next_disp_color();

    update_eyes();
    update_horn();
    update_jaws();
    update_monitor();
    head_np.show();
}

static void update_mood(gesture gst)
{
    static unsigned long last_update_ms = millis();
    static const unsigned long anim_thresh = 6000;
    bool mood_updated = true;

    if (gst & GESTURE_NOD) {
        if (state.mood == MOOD_HAPPY) {
            state.mood = MOOD_LOVING;
        } else {
            state.mood = MOOD_HAPPY;
        }
    } else if (gst & GESTURE_SHAKE) {
        if (state.mood == MOOD_IRRITATED) {
            state.mood = MOOD_ANGRY;
        } else {
            state.mood = MOOD_IRRITATED;
        }
    } else if (gst & GESTURE_TILT_L) {
        state.mood = MOOD_CONFUSED;
    } else if (gst & GESTURE_TILT_R) {
        state.mood = MOOD_OWO;
    } else {
        mood_updated = false;
    }

    if (mood_updated) {
        last_update_ms = millis();
    } else {
        unsigned long cur_time = millis();
        if ((cur_time - last_update_ms) > anim_thresh) {
            state.mood = MOOD_NEUTRAL;
        }
    }
}

void loop()
{
    gesture gst = GESTURE_NEUTRAL;
    unsigned long last_update_ms = 0;
    unsigned long cur_ms = millis();
    unsigned int poll_ctr = 0;
    for (;;) {
        cur_ms = millis();
        read_jaw_sensor();
        if ((cur_ms - last_update_ms) >= 100) {
            poll_gesture_sensors();
            if (++poll_ctr >= 5) {
                poll_ctr = 0;
                gst = process_gesture_data();
                update_mood(gst);
            }
            last_update_ms = cur_ms;
            update_display();
        }
    }
}
