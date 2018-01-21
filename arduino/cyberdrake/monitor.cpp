#include <Adafruit_GFX.h>

#include "anim.h"
#include "cyberdrake.h"
#include "monitor.h"

Adafruit_8x8matrix monitor;

bool init_monitor(void)
{
    monitor.begin(MON_ADDR);
    monitor.clear();
    monitor.setBrightness(MONITOR_BRIGHTNESS);
    monitor.setRotation(MON_ROTATION);
    monitor.writeDisplay();
}

bool update_monitor(void)
{
    static unsigned int ctr = 0;
    static mood cur_mood = N_MOODS;

    if (state.mood != cur_mood) {
        cur_mood = state.mood;
        ctr = 0;
    }

    if (ctr++ > 10) {
        monitor.clear();
        monitor.writeDisplay();
        return true;
    }

    monitor.drawBitmap(0, 0, mood_animation_map[cur_mood].l_data, 8, 8, LED_ON);

    monitor.writeDisplay();
}
