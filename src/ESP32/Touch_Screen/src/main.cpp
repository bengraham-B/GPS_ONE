#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#include "lvgl_ui_project.h"
#include "screens/screen1_gen.h"   // adjust if your screen file has a different name

// ---------- Display ----------
TFT_eSPI tft = TFT_eSPI();

// ---------- Touch ----------
#define XPT2046_IRQ   36
#define XPT2046_MOSI  32
#define XPT2046_MISO  39
#define XPT2046_CLK   25
#define XPT2046_CS    33

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// ---------- LVGL Buffers ----------
static lv_color_t buf1[320 * 10];   //  6,400 bytes
static lv_color_t buf2[320 * 20];

// ---------- Display Flush (LVGL 9) ----------
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)px_map, w * h, true);
    tft.endWrite();

    lv_display_flush_ready(disp);   // LVGL 9: "display", not "disp"
}

// ---------- Touch Read (LVGL 9) ----------
void my_touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
    if (touchscreen.touched()) {
        TS_Point p = touchscreen.getPoint();
        data->point.x = map(p.x, 200, 3700, 0, 319);
        data->point.y = map(p.y, 240, 3800, 0, 239);
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    // ---------- Display ----------
    tft.init();
    tft.setRotation(1);           // landscape 320x240
    tft.fillScreen(TFT_BLACK);

    // ---------- Touch ----------
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(1);

    // ---------- LVGL ----------
    lv_init();

    // Display (LVGL 9)
    lv_display_t *disp = lv_display_create(320, 240);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_default(disp);

    // Touch (LVGL 9)
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read);
    lv_indev_set_display(indev, disp);

    // ---------- LVGL Pro UI ----------
    lvgl_ui_project_init("");
    lv_screen_load(screen1_create());
}

void loop() {
    lv_timer_handler();
    delay(5);
}