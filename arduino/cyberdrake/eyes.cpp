
#include <Adafruit_GFX.h>

#include "cyberdrake.h"
#include "color.h"
#include "eyes.h"
#include "anim.h"

Adafruit_8x8matrix eyes[2];

extern struct animation mood_animation_map[N_MOODS];

bool init_eyes(void)
{
    eyes[EYE_L_OFFSET].begin(EYE_L_ADDR);
    eyes[EYE_R_OFFSET].begin(EYE_R_ADDR);
    eyes[EYE_L_OFFSET].clear();
    eyes[EYE_R_OFFSET].clear();
    eyes[EYE_L_OFFSET].setBrightness(EYE_BRIGHTNESS);
    eyes[EYE_R_OFFSET].setBrightness(EYE_BRIGHTNESS);
    // FIXME verify the rotation
    eyes[EYE_L_OFFSET].setRotation(EYE_L_ROTATION);
    eyes[EYE_R_OFFSET].setRotation(EYE_R_ROTATION);

    eyes[EYE_L_OFFSET].writeDisplay();
    eyes[EYE_R_OFFSET].writeDisplay();
}

static void update_matrix_eyes(void)
{
    static unsigned int frame_counter = 0;
    static unsigned int intra_frame_counter = 0;
    static struct animation* cur_animation = NULL;
    static mood cur_mood = N_MOODS;

    if (state.mood != cur_mood) {
        cur_mood = state.mood;
        cur_animation = &mood_animation_map[cur_mood];
        frame_counter = 0;
        intra_frame_counter = 0;
    }
    if (intra_frame_counter++ > 0)
    {
        if (!cur_animation->frame_durations ||
                (intra_frame_counter >= cur_animation->frame_durations[frame_counter])) {
            intra_frame_counter = 0;
            if (++frame_counter >= cur_animation->n_frames) {
                frame_counter = 0;
            }
        } else {
            return true;
        }
    }
    eyes[EYE_L_OFFSET].clear();
    eyes[EYE_L_OFFSET].drawBitmap(0, 0, &cur_animation->l_data[8*frame_counter], 8, 8, LED_ON);
    eyes[EYE_R_OFFSET].clear();
    eyes[EYE_R_OFFSET].drawBitmap(0, 0, &cur_animation->r_data[8*frame_counter], 8, 8, LED_ON);

    eyes[EYE_L_OFFSET].writeDisplay();
    eyes[EYE_R_OFFSET].writeDisplay();
}

static void set_np_eyes_color(uint32_t color)
{
    for (int i = 0; i < EYE_NP_NUM; ++i) {
        head_np.setPixelColor(EYE_L_NP_OFF + i, color);
        head_np.setPixelColor(EYE_R_NP_OFF + i, color);
    }
}

static void clear_np_eyes(void)
{
    set_np_eyes_color(0);
}

static inline uint8_t conv_r_idx(uint8_t r_idx)
{
    // Only necessary because I mounted one eye strip upside down x.=.x
    return (r_idx + 9) % EYE_NP_NUM;
}


static inline uint8_t conv_l_idx(uint8_t l_idx)
{
    return (l_idx + 9) % EYE_NP_NUM;
}

static void np_neutral(void)
{
    static uint8_t idx = 0;
    static uint8_t iter = EYE_NP_NUM;
    static uint8_t ctr = 0;

    if (++ctr & 1) {
        // Skip every other frame, update at half framerate.
        return;
    }

    if (iter == idx) {
        iter = EYE_NP_NUM;
        if (++idx >= EYE_NP_NUM) {
            idx = 0;
            clear_np_eyes();
            return;
        }
    }
    uint32_t wheel_color = color_wheel(&head_np, 128 + 12*idx);
    wheel_color = scaled_color(wheel_color, EYE_NP_SCALE);

    if (iter < EYE_NP_NUM) {
        head_np.setPixelColor(EYE_L_NP_OFF + conv_l_idx(iter), 0);
        head_np.setPixelColor(EYE_R_NP_OFF + conv_r_idx(iter), 0);
    }
    --iter;
    head_np.setPixelColor(EYE_L_NP_OFF + conv_l_idx(iter), wheel_color);
    head_np.setPixelColor(EYE_R_NP_OFF + conv_r_idx(iter), wheel_color);
}


static void np_uniform(void)
{
    uint32_t color = scaled_color(state.disp_color, EYE_NP_SCALE);
    set_np_eyes_color(color);
}

static void color_chase(uint32_t color)
{
    static int idx = 0;

    clear_np_eyes();

    head_np.setPixelColor(EYE_L_NP_OFF + conv_l_idx(idx), color);
    head_np.setPixelColor(EYE_R_NP_OFF + conv_r_idx(idx), color);

    if (++idx >= EYE_NP_NUM) {
        idx = 0;
    }
}

static void np_confused(void)
{
    color_chase(head_np.Color(16, 0, 32));  
}

static void np_owo(void)
{
    color_chase(head_np.Color(32, 0, 16));  
}


static const void (*np_func[N_MOODS])(void) = {
    np_neutral,
    np_uniform,
    np_uniform,
    np_uniform,
    np_uniform,
    np_confused,
    np_owo
};

static void update_np_eyes(void)
{
    static mood last_mood = MOOD_NEUTRAL;

    if (state.mood != last_mood) {
        last_mood = state.mood;
        clear_np_eyes();
    }
    np_func[state.mood]();
}

bool update_eyes(void)
{
    update_matrix_eyes();
    update_np_eyes();
    return true;
}
