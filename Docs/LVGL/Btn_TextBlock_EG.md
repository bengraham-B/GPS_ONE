# LVGL Pro → PlatformIO: Button, Label, Callback & Dynamic Text

A complete end-to-end example showing how to:

1. Design a button and text element in LVGL Pro
2. Export the code and import it into PlatformIO
3. Attach a custom print method to the button
4. Update the text field dynamically from PlatformIO

Assumes a working LVGL Pro → PlatformIO setup on an ESP32-CYD (ILI9341 + XPT2046), with the project already compiling and displaying a screen.

---

## Table of Contents

- [Overview](#overview)
- [Step 1 — Design the UI in LVGL Pro](#step-1--design-the-ui-in-lvgl-pro)
- [Step 2 — Export and Import into PlatformIO](#step-2--export-and-import-into-platformio)
- [Step 3 — Attach a Custom Callback to the Button](#step-3--attach-a-custom-callback-to-the-button)
- [Step 4 — Update the Text Field Dynamically](#step-4--update-the-text-field-dynamically)
- [Complete `main.cpp`](#complete-maincpp)
- [How It Works](#how-it-works)
- [Alternative: Reactive Binding with Subjects](#alternative-reactive-binding-with-subjects)
- [Common Pitfalls](#common-pitfalls)

---

## Overview

The flow from design to runtime:

```
[LVGL Pro Editor]
    │  draw widgets, give them names
    ▼
main_screen.xml  →  Export  →  main_screen_gen.c / .h
    │
    ▼
[PlatformIO project]
    │  lib/lvgl_ui_project/screens/main_screen_gen.c
    │  lib/lvgl_ui_project/lvgl_ui_project.h
    ▼
main.cpp
    │  lvgl_ui_project_init("")
    │  lv_screen_load(main_screen_create())
    │  lv_obj_find_by_name(...)   ← find widgets
    │  lv_obj_add_event_cb(...)   ← attach behavior
    │  lv_label_set_text_fmt(...) ← update dynamically
    ▼
[ESP32 runs the UI]
```

Two things to remember:

- **LVGL Pro** owns *what the screen looks like*.
- **You** own *what happens when the user interacts with it*.

---

## Step 1 — Design the UI in LVGL Pro

### 1.1 Create the screen

In LVGL Pro, create a screen (e.g. `main_screen`). Add a button and a label.

### 1.2 The XML

Give each widget a **`name`** attribute. This is what lets C code find them at runtime.

```xml
<screen>
    <view extends="lv_obj">
        <lv_button name="my_btn" x="100" y="80" width="120" height="50">
            <lv_label name="btn_label" text="Click Me" align="center" />
        </lv_button>
        <lv_label name="my_text" x="80" y="160" text="Waiting..." />
    </view>
</screen>
```

**Key points:**

| Element | Purpose |
|---------|---------|
| `<lv_button name="my_btn">` | The button — found in C via `"my_btn"` |
| `<lv_label name="btn_label">` | The text on the button |
| `<lv_label name="my_text">` | The separate label we'll update dynamically |

### 1.3 Enable object names in `lv_conf.h`

The `name` attribute relies on LVGL's object ID/name system. Without these, `lv_obj_find_by_name()` will return `NULL`.

```c
#define LV_USE_OBJ_ID 1
#define LV_USE_OBJ_NAME 1
```

Also, if your generated code calls translation functions, enable:

```c
#define LV_USE_TRANSLATION 1
```

---

## Step 2 — Export and Import into PlatformIO

### 2.1 Export from LVGL Pro

1. Click **Export / Generate Code** in the LVGL Pro toolbar.
2. Wait for validation to pass.
3. The generated files appear next to their XML sources.

For a screen named `main_screen`, you get:

| File | Contents |
|------|----------|
| `screens/main_screen_gen.c` | `main_screen_create()` — builds the widget tree |
| `screens/main_screen_gen.h` | Prototype |
| `lvgl_ui_project.c` / `.h` | User-editable glue |
| `lvgl_ui_project_gen.c` / `.h` | Auto-generated init |

### 2.2 Copy into PlatformIO

Copy into your PlatformIO project's `lib/lvgl_ui_project/` folder:

```
lib/lvgl_ui_project/
├── screens/
│   ├── main_screen_gen.c
│   └── main_screen_gen.h
├── lvgl_ui_project.c
├── lvgl_ui_project.h
├── lvgl_ui_project_gen.c
└── lvgl_ui_project_gen.h
```

### 2.3 Do NOT copy

- `sim/`
- `.vscode/`
- `CMakeLists.txt`, `file_list_gen.cmake`, `user_config.cmake`
- `project.xml`, `globals.xml`, `translations.xml`

These are editor- or simulator-specific and will confuse PlatformIO's build.

### 2.4 Verify PlatformIO sees it

PlatformIO compiles any `.c` file under `lib/` automatically. No `platformio.ini` changes needed for the source itself.

---

## Step 3 — Attach a Custom Callback to the Button

### 3.1 Find the button by name

After loading the screen, search for the widget:

```c
lv_obj_t *btn = lv_obj_find_by_name(lv_screen_active(), "my_btn");
```

`lv_screen_active()` returns the currently displayed screen. The returned pointer is the widget you named in XML, or `NULL` if it wasn't found.

### 3.2 Define the callback

```c
static void my_custom_print(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        Serial.println(">>> Button was clicked! <<<");
    }
}
```

### 3.3 Attach it

```c
if (btn) {
    lv_obj_add_event_cb(btn, my_custom_print, LV_EVENT_CLICKED, NULL);
}
```

- `btn` — the target widget
- `my_custom_print` — your handler
- `LV_EVENT_CLICKED` — the event to listen for
- `NULL` — optional user data pointer

### 3.4 Event handler signature

Every LVGL 9 event handler has the same shape:

```c
static void my_handler(lv_event_t *e) {
    lv_event_code_t code      = lv_event_get_code(e);      // which event fired
    lv_obj_t *target          = lv_event_get_target(e);    // the widget
    void *user_data           = lv_event_get_user_data(e); // your extra pointer
    // ...
}
```

You can listen for multiple events with `LV_EVENT_ALL`, then branch on `code`.

---

## Step 4 — Update the Text Field Dynamically

### 4.1 Extend the callback

```c
static int click_count = 0;

static void my_custom_print(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        click_count++;
        Serial.printf(">>> Button clicked %d times <<<\n", click_count);

        // Find the label and update its text
        lv_obj_t *label = lv_obj_find_by_name(lv_screen_active(), "my_text");
        if (label) {
            lv_label_set_text_fmt(label, "Clicked %d times", click_count);
        }
    }
}
```

### 4.2 The three text-setting functions

| Function | When to use |
|----------|-------------|
| `lv_label_set_text(label, "hello")` | Fixed string; LVGL allocates and copies it |
| `lv_label_set_text_fmt(label, "x=%d", x)` | Formatted text (like `printf`) |
| `lv_label_set_text_static(label, buf)` | Constant buffer that stays alive — avoids heap churn |

### 4.3 Ensuring the label is safe to update

Always null-check before touching it:

```c
lv_obj_t *label = lv_obj_find_by_name(lv_screen_active(), "my_text");
if (label) {
    lv_label_set_text_fmt(label, "Clicked %d times", click_count);
}
```

If `label` is `NULL`, either:
- The widget wasn't named in XML, or
- `LV_USE_OBJ_ID` / `LV_USE_OBJ_NAME` aren't enabled, or
- The name is spelled differently

---

## Complete `main.cpp`

Drop-in example for the CYD. Adapt pin definitions as needed.

```cpp
#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#include "lvgl_ui_project.h"
#include "screens/main_screen_gen.h"

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

// ---------- LVGL buffer (small, DRAM is tight) ----------
static lv_color_t buf1[320 * 10];

// ---------- Display flush (LVGL 9) ----------
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)px_map, w * h, true);
    tft.endWrite();
    lv_display_flush_ready(disp);
}

// ---------- Touch read (LVGL 9) ----------
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

// ---------- Button callback ----------
static int click_count = 0;

static void my_custom_print(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        click_count++;
        Serial.printf(">>> Button clicked %d times <<<\n", click_count);

        lv_obj_t *label = lv_obj_find_by_name(lv_screen_active(), "my_text");
        if (label) {
            lv_label_set_text_fmt(label, "Clicked %d times", click_count);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    // Display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Touch
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(1);

    // LVGL
    lv_init();

    lv_display_t *disp = lv_display_create(320, 240);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_default(disp);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read);
    lv_indev_set_display(indev, disp);

    // Load LVGL Pro UI
    lvgl_ui_project_init("");
    lv_screen_load(main_screen_create());

    // Attach callback to the button
    lv_obj_t *btn = lv_obj_find_by_name(lv_screen_active(), "my_btn");
    if (btn) {
        lv_obj_add_event_cb(btn, my_custom_print, LV_EVENT_CLICKED, NULL);
        Serial.println("Callback attached to my_btn");
    } else {
        Serial.println("ERROR: my_btn not found!");
    }
}

void loop() {
    lv_timer_handler();
    delay(5);
}
```

---

## How It Works

### The full path from click to screen update

1. **User taps the screen** → XPT2046 reports coordinates via `my_touch_read`.
2. **LVGL processes the tap** → finds which widget was hit.
3. **The button's callback fires** → `my_custom_print()` runs on the next `lv_timer_handler()` cycle.
4. **Callback runs your code** → prints to Serial, increments a counter.
5. **Callback updates the label** → `lv_label_set_text_fmt()` changes the text.
6. **LVGL marks the label dirty** → on the next `lv_timer_handler()`, it redraws.
7. **`my_disp_flush()` pushes pixels** → the new text appears on the TFT.

The whole cycle is cooperative — nothing blocks. `loop()` just calls `lv_timer_handler()` repeatedly.

### Why `lv_obj_find_by_name` works

When you give a widget a `name` in XML:

- LVGL Pro generates a `lv_obj_set_name_static(obj, "my_btn")` call inside `main_screen_create()`.
- LVGL stores the name in the widget's metadata.
- `lv_obj_find_by_name(parent, "my_btn")` walks the tree and returns the matching widget.

Without the name attribute in XML, the widget still exists — it just can't be found by name.

---

## Alternative: Reactive Binding with Subjects

The manual approach above is explicit and always works. LVGL also offers **subjects** — reactive variables that widgets can bind to. When the subject changes, every bound widget updates automatically.

### In LVGL Pro XML

Declare a subject and bind the label:

```xml
<screen>
    <subjects>
        <subject name="click_count" type="int" value="0" />
    </subjects>
    <view extends="lv_obj">
        <lv_button name="my_btn" x="100" y="80" width="120" height="50">
            <lv_label text="Click Me" align="center" />
        </lv_button>
        <lv_label x="80" y="160" bind_text="click_count" />
    </view>
</screen>
```

### In C

```c
extern lv_subject_t click_count;   // declared by generated code

static void my_custom_print(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        int v = lv_subject_get_int(&click_count);
        lv_subject_set_int(&click_count, v + 1);
    }
}
```

The label updates itself. No `lv_label_set_text` call needed.

**Trade-offs:**

| Approach | Pros | Cons |
|----------|------|------|
| Manual (`lv_label_set_text`) | Explicit; works anywhere | You must remember to update every dependent widget |
| Subjects (`lv_subject_set_int`) | Declarative; updates everything bound at once | Requires setting up subjects in XML; slightly more setup |

---

## Common Pitfalls

| Symptom | Cause | Fix |
|---------|-------|-----|
| `lv_obj_find_by_name` returns `NULL` | Widget not named in XML, or object IDs disabled | Add `name="..."` in XML; set `LV_USE_OBJ_ID 1` and `LV_USE_OBJ_NAME 1` |
| Callback never fires | Wrong event code, or widget covered | Verify `LV_EVENT_CLICKED`; check z-order |
| Text doesn't update | Label pointer was `NULL` | Null-check before calling `lv_label_set_text_fmt` |
| Text flickers or disappears | Passing a temporary buffer to `lv_label_set_text_static` | Use `lv_label_set_text` or `lv_label_set_text_fmt` for non-constant strings |
| `undefined reference to lv_obj_set_name_static` | `LV_USE_OBJ_ID` off | Enable it in `lv_conf.h` |
| `undefined reference to lv_translation_add_static` | `LV_USE_TRANSLATION` off | Enable it in `lv_conf.h` (or remove translations from `translations.xml`) |
| Screen appears blank | `lv_screen_load` not called, or called before `lvgl_ui_project_init` | Always init the project before loading screens |

---

## References

- LVGL 9 widgets — https://docs.lvgl.io/master/widgets/index.html
- Event system — https://docs.lvgl.io/master/common-widget-features/events.html
- Observers / Subjects — https://docs.lvgl.io/master/others/observer.html
- LVGL Pro — https://lvgl.io/pro

---

## Change Log

| Date | Change |
|------|--------|
| 2026-09-29 | Initial button + label + callback + dynamic text example |