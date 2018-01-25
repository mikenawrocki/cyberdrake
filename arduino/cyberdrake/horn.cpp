#include "cyberdrake.h"
#include "horn.h"

bool init_horn(void)
{
    for (int i = HORN_OFFSET; i < (HORN_OFFSET + HORN_NUM); ++i) {
        head_np.setPixelColor(i, 0);
    }
    return true;
}

bool update_horn(void)
{
    for (int i = HORN_OFFSET; i < (HORN_OFFSET + HORN_NUM); ++i) {
        head_np.setPixelColor(i, state.disp_color);
    }
    return true;
}
