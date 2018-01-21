#ifndef EYES_H
#define EYES_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// 8x8 matrix eyes
#define EYE_L_ADDR 0x71
#define EYE_R_ADDR 0x73
#define EYE_L_OFFSET 0
#define EYE_R_OFFSET 1
#define EYE_L_ROTATION 2
#define EYE_R_ROTATION 2
#define EYE_BRIGHTNESS 15

// Neopixel eyes
#define EYE_NP_NUM 12
#define EYE_L_NP_OFF 0
#define EYE_R_NP_OFF 12


extern Adafruit_8x8matrix eyes[2];

bool init_eyes(void);
bool update_eyes(void);

#endif
