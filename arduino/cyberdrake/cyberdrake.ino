#include "cyberdrake.h"
#include "eyes.h"
#include "gesture.h"
#include "monitor.h"

Adafruit_NeoPixel head_np{HEAD_NP_NUM_PIXELS, HEAD_NP_PIN, NEO_GRB | NEO_KHZ800};

struct cyberdrake_state state;

const char* const mood_str_map[N_MOODS] = {
    "NEUTRAL",
    "HAPPY",
    "LOVING",
    "IRRITATED",
    "ANGRY",
    "CONFUSED",
    "OWO",
};

static void read_jaw(void)
{
    state.jaw_open = !digitalRead(JAW_PIN);
}

static void init_jaw(void)
{
    pinMode(JAW_PIN, INPUT_PULLUP);
    read_jaw();
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

    init_jaw();
    init_eyes();
    init_monitor();
    init_gesture();
    head_np.begin();
    head_np.show();
}

static void update_display(void)
{
    update_eyes();
    update_monitor();

    head_np.show();
}

static void update_mood(gesture gst)
{
    static unsigned long last_update_ms = millis();
    static const unsigned long anim_thresh = 6000;
    bool mood_updated = true;

    if (gst & GESTURE_NOD) {
        if (state.mood == MOOD_HAPPY || state.mood == MOOD_LOVING) {
            state.mood = MOOD_LOVING;
        } else {
            state.mood = MOOD_HAPPY;
        }
    } else if (gst & GESTURE_SHAKE) {
        if (state.mood == MOOD_IRRITATED || state.mood == MOOD_ANGRY) {
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
        read_jaw();
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
