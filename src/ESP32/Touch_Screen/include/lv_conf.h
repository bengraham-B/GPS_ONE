#ifndef LV_CONF_H
#define LV_CONF_H

/* ============ Features needed by LVGL Pro generated code ============ */
#define LV_USE_TRANSLATION 1
#define LV_USE_OBJ_ID 1
#define LV_USE_OBJ_NAME 1

/* ============ Memory tuning for ESP32 ============ */
#define LV_MEM_SIZE (40 * 1024U)
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE (16 * 1024)
#define LV_DRAW_THREAD_STACK_SIZE (4 * 1024)

/* ============ Color depth for RGB565 ============ */
#define LV_COLOR_DEPTH 16

/* ============ Tick source ============ */
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

/* ============ Logging (optional, disable to save DRAM) ============ */
#define LV_USE_LOG 0

/* ============ Fonts (disable unused to save flash) ============ */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_DEFAULT &lv_font_montserrat_14

#endif /* LV_CONF_H */