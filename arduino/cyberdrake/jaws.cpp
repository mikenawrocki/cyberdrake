#include <Adafruit_GFX.h>

#include "cyberdrake.h"
#include "jaws.h"

bool init_jaws(void)
{
    // Canines always on
    for (int i = JAW_U_CAN_OFFSET; i < (JAW_U_CAN_OFFSET + JAW_U_CAN_NUM); ++i) {
        head_np.setPixelColor(i, 0);
    }
    // Jawsets only on if maw open
    for (int i = JAW_U_OFFSET; i < (JAW_U_OFFSET + JAW_U_NUM); ++i) {
        head_np.setPixelColor(i, 0);
    }
    for (int i = JAW_U_CAN_OFFSET; i < JAW_U_OFFSET; ++i) {
        head_np.setPixelColor(i, 0);
    }
    return true;
}

bool update_jaws(void)
{
    uint32_t j_color = state.disp_color;
    // Color is full intensity, no scaling needed.
   
    // Canines always on
    for (int i = JAW_U_CAN_OFFSET; i < (JAW_U_CAN_OFFSET + JAW_U_CAN_NUM); ++i) {
        head_np.setPixelColor(i, j_color);
    }
    j_color = (state.jaw_open) ? j_color : 0;
    // Jawsets only on if maw open
    for (int i = JAW_U_OFFSET; i < (JAW_U_OFFSET + JAW_U_NUM); ++i) {
        head_np.setPixelColor(i, j_color);
    }
    for (int i = JAW_L_OFFSET; i < (JAW_L_OFFSET + JAW_L_NUM); ++i) {
        head_np.setPixelColor(i, j_color);
    }
    return true;
}
