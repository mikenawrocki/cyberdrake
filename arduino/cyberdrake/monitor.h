#ifndef MONITOR_H
#define MONITOR_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// 8x8 matrix monitor
#define MON_ADDR 0x70
#define MON_ROTATION 2
#define MONITOR_BRIGHTNESS 0

extern Adafruit_8x8matrix monitor;

bool init_monitor(void);
bool update_monitor(void);

#endif
