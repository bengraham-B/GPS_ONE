# LVGL 9 Widget Reference

A practical reference of the most commonly used LVGL 9 widgets with C code examples. All examples assume LVGL 9.x with the `lv_screen_active()` API and a display + input device already registered.

---

## Table of Contents

- [General Pattern](#general-pattern)
- [Base Object (lv_obj)](#base-object-lv_obj)
- [Label (lv_label)](#label-lv_label)
- [Button (lv_button)](#button-lv_button)
- [Image (lv_image)](#image-lv_image)
- [Bar (lv_bar)](#bar-lv_bar)
- [Slider (lv_slider)](#slider-lv_slider)
- [Arc (lv_arc)](#arc-lv_arc)
- [Switch (lv_switch)](#switch-lv_switch)
- [Checkbox (lv_checkbox)](#checkbox-lv_checkbox)
- [Dropdown (lv_dropdown)](#dropdown-lv_dropdown)
- [Roller (lv_roller)](#roller-lv_roller)
- [Text Area (lv_textarea)](#text-area-lv_textarea)
- [Spinbox (lv_spinbox)](#spinbox-lv_spinbox)
- [LED (lv_led)](#led-lv_led)
- [Table (lv_table)](#table-lv_table)
- [Chart (lv_chart)](#chart-lv_chart)
- [Calendar (lv_calendar)](#calendar-lv_calendar)
- [Tabview (lv_tabview)](#tabview-lv_tabview)
- [Message Box (lv_msgbox)](#message-box-lv_msgbox)
- [Spinner (lv_spinner)](#spinner-lv_spinner)
- [Keyboard (lv_keyboard)](#keyboard-lv_keyboard)
- [Common Styles Cheat Sheet](#common-styles-cheat-sheet)
- [Common Event Codes](#common-event-codes)
- [Memory & Performance Tips](#memory--performance-tips)

---

## General Pattern

Every widget follows the same lifecycle:

1. **Create** on a parent (screen or container): `lv_xxx_create(parent)`
2. **Configure** size, position, content: `lv_obj_set_size()`, `lv_xxx_set_value()`, etc.
3. **Style** appearance: `lv_obj_set_style_*()`
4. **Attach behavior** with event callbacks: `lv_obj_add_event_cb()`
5. **Optionally find later** by name: `lv_obj_find_by_name(parent, "name")`

The `parent` is usually `lv_screen_active()` for a top-level widget, or another widget for a nested one.

---

## Base Object (lv_obj)

The container widget that all other widgets inherit from. Use it as a panel, card, or custom drawing surface.

**Functions:**
- `lv_obj_create(parent)` — create
- `lv_obj_set_size(obj, w, h)` — dimensions (use `LV_SIZE_CONTENT` for auto)
- `lv_obj_set_pos(obj, x, y)` / `lv_obj_set_x()` / `lv_obj_set_y()`
- `lv_obj_align(obj, align, x, y)` — align relative to parent
- `lv_obj_align_to(obj, base, align, x, y)` — align relative to another object
- `lv_obj_center(obj)` — shorthand for `LV_ALIGN_CENTER, 0, 0`
- `lv_obj_add_flag(obj, flag)` — e.g. `LV_OBJ_FLAG_CLICKABLE`, `LV_OBJ_FLAG_HIDDEN`
- `lv_obj_clear_flag(obj, flag)` — remove a flag
- `lv_obj_set_style_*()` — apply styles to any part

**Example — styled panel with child:**

```c
lv_obj_t *panel = lv_obj_create(lv_screen_active());
lv_obj_set_size(panel, 200, 150);
lv_obj_center(panel);
lv_obj_set_style_bg_color(panel, lv_color_hex(0x2A2A2A), LV_PART_MAIN);
lv_obj_set_style_radius(panel, 12, LV_PART_MAIN);
lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN);
lv_obj_set_style_pad_all(panel, 10, LV_PART_MAIN);

lv_obj_t *child = lv_obj_create(panel);
lv_obj_set_size(child, 100, 50);
lv_obj_center(child);
```

**Example — hide/show dynamically:**

```c
lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);    // hide
lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);  // show
```

---

## Label (lv_label)

Text display. Supports formatting, alignment within a fixed width, and scrolling for long text.

**Functions:**
- `lv_label_create(parent)`
- `lv_label_set_text(label, "text")`
- `lv_label_set_text_fmt(label, "Value: %d", 42)`
- `lv_label_set_long_mode(label, mode)` — `LV_LABEL_LONG_WRAP`, `_SCROLL`, `_SCROLL_CIRCULAR`, `_DOT`, `_CLIP`
- `lv_label_set_text_static(label, "text")` — avoids heap allocation if text is a constant

**Example — basic label:**

```c
lv_obj_t *label = lv_label_create(lv_screen_active());
lv_label_set_text(label, "Hello, LVGL!");
lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);
```

**Example — formatted text:**

```c
int count = 0;
lv_obj_t *label = lv_label_create(lv_screen_active());
lv_label_set_text_fmt(label, "Count: %d", count);
lv_obj_center(label);
```

**Example — long scrolling text:**

```c
lv_obj_t *scroll = lv_label_create(lv_screen_active());
lv_obj_set_width(scroll, 150);
lv_label_set_long_mode(scroll, LV_LABEL_LONG_SCROLL_CIRCULAR);
lv_label_set_text(scroll, "This is a very long text that will scroll.");
lv_obj_align(scroll, LV_ALIGN_BOTTOM_MID, 0, -20);
```

**Example — recoloring part of the text (recolor):**

```c
lv_obj_t *label = lv_label_create(lv_screen_active());
lv_label_set_recolor(label, true);
lv_label_set_text(label, "Temp: #ff0000 45# #000000 C");
lv_obj_center(label);
```

---

## Button (lv_button)

Clickable control. Wraps a label or any other widget.

**Functions:**
- `lv_button_create(parent)`
- `lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL)`
- `lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE)` — toggle behavior

**Example — basic button:**

```c
static void btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        lv_label_set_text(label, "Clicked");
    }
}

lv_obj_t *btn = lv_button_create(lv_screen_active());
lv_obj_set_size(btn, 120, 50);
lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

lv_obj_t *label = lv_label_create(btn);
lv_label_set_text(label, "Press");
lv_obj_center(label);
```

**Example — styled button:**

```c
lv_obj_set_style_bg_color(btn, lv_color_hex(0x0066CC), LV_PART_MAIN);
lv_obj_set_style_bg_color(btn, lv_color_hex(0x004499), LV_PART_MAIN | LV_STATE_PRESSED);
lv_obj_set_style_radius(btn, 8, LV_PART_MAIN);
lv_obj_set_style_shadow_width(btn, 10, LV_PART_MAIN);
lv_obj_set_style_shadow_opa(btn, LV_OPA_40, LV_PART_MAIN);
```

**Example — toggle button:**

```c
lv_obj_t *tgl = lv_button_create(lv_screen_active());
lv_obj_add_flag(tgl, LV_OBJ_FLAG_CHECKABLE);
lv_obj_set_height(tgl, LV_SIZE_CONTENT);
lv_obj_align(tgl, LV_ALIGN_CENTER, 0, 40);

lv_obj_t *lbl = lv_label_create(tgl);
lv_label_set_text(lbl, "Toggle");
lv_obj_center(lbl);
```

---

## Image (lv_image)

Displays an image from a C array, file, or symbol.

**Functions:**
- `lv_image_create(parent)`
- `lv_image_set_src(img, src)` — `&my_img` for arrays, `"S:/path.png"` for files
- `lv_image_set_rotation(img, angle)` — degrees × 10
- `lv_image_set_scale(img, scale)` — 256 = 1.0×
- `lv_image_set_offset_x/y()` — pan within the widget

**Example — image from a C array:**

```c
LV_IMAGE_DECLARE(my_logo);   // defined elsewhere, e.g. via LVGL image converter

lv_obj_t *img = lv_image_create(lv_screen_active());
lv_image_set_src(img, &my_logo);
lv_obj_center(img);
```

**Example — image with rotation and scale:**

```c
lv_obj_t *img = lv_image_create(lv_screen_active());
lv_image_set_src(img, &my_icon);
lv_image_set_rotation(img, 450);   // 45.0 degrees
lv_image_set_scale(img, 384);      // 1.5x
lv_obj_center(img);
```

---

## Bar (lv_bar)

Horizontal or vertical progress/level indicator.

**Functions:**
- `lv_bar_create(parent)`
- `lv_bar_set_range(bar, min, max)`
- `lv_bar_set_value(bar, value, LV_ANIM_ON/OFF)`
- `lv_bar_set_mode(bar, LV_BAR_MODE_NORMAL/`SYMMETRICAL`/`RANGE`)`

**Example — progress bar:**

```c
lv_obj_t *bar = lv_bar_create(lv_screen_active());
lv_obj_set_size(bar, 200, 20);
lv_obj_center(bar);
lv_bar_set_range(bar, 0, 100);
lv_bar_set_value(bar, 65, LV_ANIM_OFF);
lv_obj_set_style_bg_color(bar, lv_color_hex(0x0066CC), LV_PART_INDICATOR);
```

**Example — vertical bar:**

```c
lv_obj_t *vbar = lv_bar_create(lv_screen_active());
lv_obj_set_size(vbar, 20, 150);
lv_obj_align(vbar, LV_ALIGN_LEFT_MID, 20, 0);
lv_bar_set_range(vbar, 0, 100);
lv_bar_set_value(vbar, 40, LV_ANIM_OFF);
```

**Example — symmetrical bar (centered at zero):**

```c
lv_bar_set_mode(bar, LV_BAR_MODE_SYMMETRICAL);
lv_bar_set_range(bar, -100, 100);
lv_bar_set_value(bar, 30, LV_ANIM_OFF);
```

---

## Slider (lv_slider)

Interactive value selector. Extends `lv_bar` with a draggable knob.

**Functions:**
- `lv_slider_create(parent)`
- `lv_slider_set_range(slider, min, max)`
- `lv_slider_set_value(slider, value, anim)`
- `lv_slider_get_value(slider)`
- Event: `LV_EVENT_VALUE_CHANGED`

**Example — slider with live label:**

```c
static void slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    lv_label_set_text_fmt(label, "%d", (int)lv_slider_get_value(slider));
}

lv_obj_t *slider = lv_slider_create(lv_screen_active());
lv_obj_set_width(slider, 200);
lv_obj_center(slider);
lv_slider_set_range(slider, 0, 100);
lv_slider_set_value(slider, 50, LV_ANIM_OFF);

lv_obj_t *value_label = lv_label_create(lv_screen_active());
lv_label_set_text(value_label, "50");
lv_obj_align_to(value_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, value_label);
```

---

## Arc (lv_arc)

Circular gauge or knob. Used for dials, gauges, and radial progress.

**Functions:**
- `lv_arc_create(parent)`
- `lv_arc_set_range(arc, min, max)`
- `lv_arc_set_value(arc, value)`
- `lv_arc_set_bg_angles(arc, start, end)` — degrees
- `lv_arc_set_rotation(arc, angle)`
- `lv_arc_set_mode(arc, LV_ARC_MODE_NORMAL/`REVERSE`/`SYMMETRICAL`)`

**Example — gauge arc:**

```c
lv_obj_t *arc = lv_arc_create(lv_screen_active());
lv_obj_set_size(arc, 150, 150);
lv_obj_center(arc);
lv_arc_set_rotation(arc, 135);
lv_arc_set_bg_angles(arc, 0, 270);
lv_arc_set_range(arc, 0, 100);
lv_arc_set_value(arc, 60);
```

**Example — knob-only arc (no indicator):**

```c
lv_obj_remove_style(arc, NULL, LV_PART_INDICATOR);
lv_arc_set_mode(arc, LV_ARC_MODE_REVERSE);
```

---

## Switch (lv_switch)

On/off toggle. Cleaner than a checkbox for binary settings.

**Functions:**
- `lv_switch_create(parent)`
- `lv_obj_add_state(sw, LV_STATE_CHECKED)` — set on
- `lv_obj_remove_state(sw, LV_STATE_CHECKED)` — set off
- `lv_obj_has_state(sw, LV_STATE_CHECKED)` — query
- Event: `LV_EVENT_VALUE_CHANGED`

**Example — switch with state callback:**

```c
static void switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    Serial.printf("Switch: %s\n", on ? "ON" : "OFF");
}

lv_obj_t *sw = lv_switch_create(lv_screen_active());
lv_obj_center(sw);
lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

---

## Checkbox (lv_checkbox)

Text with a check indicator. The label is built in — you set the text directly.

**Functions:**
- `lv_checkbox_create(parent)`
- `lv_checkbox_set_text(cb, "text")`
- Same state API as switch (`LV_STATE_CHECKED`)

**Example:**

```c
static void cb_event_cb(lv_event_t *e) {
    lv_obj_t *cb = lv_event_get_target(e);
    bool on = lv_obj_has_state(cb, LV_STATE_CHECKED);
    Serial.printf("Checkbox: %d\n", on);
}

lv_obj_t *cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Enable feature");
lv_obj_center(cb);
lv_obj_add_event_cb(cb, cb_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

---

## Dropdown (lv_dropdown)

Collapsible list of options. Shows the selected option when closed.

**Functions:**
- `lv_dropdown_create(parent)`
- `lv_dropdown_set_options(dd, "Option 1\nOption 2\nOption 3")`
- `lv_dropdown_set_selected(dd, idx)`
- `lv_dropdown_get_selected(dd)`
- `lv_dropdown_get_selected_str(dd, buf, buf_size)`
- Event: `LV_EVENT_VALUE_CHANGED`

**Example:**

```c
static void dd_event_cb(lv_event_t *e) {
    lv_obj_t *dd = lv_event_get_target(e);
    char buf[32];
    lv_dropdown_get_selected_str(dd, buf, sizeof(buf));
    Serial.printf("Selected: %s\n", buf);
}

lv_obj_t *dd = lv_dropdown_create(lv_screen_active());
lv_dropdown_set_options(dd, "Red\nGreen\nBlue\nYellow");
lv_obj_align(dd, LV_ALIGN_TOP_MID, 0, 20);
lv_obj_add_event_cb(dd, dd_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

---

## Roller (lv_roller)

Scrolling wheel selector. Good for compact numeric or list selection.

**Functions:**
- `lv_roller_create(parent)`
- `lv_roller_set_options(roller, "Item 1\nItem 2", LV_ROLLER_MODE_NORMAL)`
- `lv_roller_set_visible_row_count(roller, n)`
- `lv_roller_set_selected(roller, idx, LV_ANIM_ON)`
- Event: `LV_EVENT_VALUE_CHANGED`

**Example:**

```c
lv_obj_t *roller = lv_roller_create(lv_screen_active());
lv_roller_set_options(roller,
    "Monday\nTuesday\nWednesday\nThursday\nFriday",
    LV_ROLLER_MODE_NORMAL);
lv_roller_set_visible_row_count(roller, 3);
lv_obj_center(roller);
```

---

## Text Area (lv_textarea)

Multi-line text input with cursor and keyboard support.

**Functions:**
- `lv_textarea_create(parent)`
- `lv_textarea_set_text(ta, "text")`
- `lv_textarea_set_placeholder_text(ta, "hint")`
- `lv_textarea_set_one_line(ta, true)` — single-line mode
- `lv_textarea_set_password_mode(ta, true)` — mask characters
- `lv_textarea_get_text(ta)`
- Event: `LV_EVENT_VALUE_CHANGED`, `LV_EVENT_READY`

**Example — text field with keyboard:**

```c
static void ta_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = (lv_obj_t *)lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

lv_obj_t *ta = lv_textarea_create(lv_screen_active());
lv_obj_set_size(ta, 240, 50);
lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 20);
lv_textarea_set_placeholder_text(ta, "Type here...");
lv_textarea_set_one_line(ta, true);

lv_obj_t *kb = lv_keyboard_create(lv_screen_active());
lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
```

---

## Spinbox (lv_spinbox)

Numeric input with increment/decrement or keyboard entry. Extends `lv_textarea`.

**Functions:**
- `lv_spinbox_create(parent)`
- `lv_spinbox_set_range(sb, min, max)`
- `lv_spinbox_set_value(sb, value)`
- `lv_spinbox_set_digit_format(sb, digits, separator_pos)`
- `lv_spinbox_increment(sb)` / `lv_spinbox_decrement(sb)`

**Example:**

```c
lv_obj_t *sb = lv_spinbox_create(lv_screen_active());
lv_spinbox_set_range(sb, -1000, 1000);
lv_spinbox_set_digit_format(sb, 4, 2);
lv_spinbox_set_value(sb, 50);
lv_obj_set_width(sb, 100);
lv_obj_center(sb);
```

---

## LED (lv_led)

Simple glowing indicator. Cheaper than a styled `lv_obj` when you only need on/off state.

**Functions:**
- `lv_led_create(parent)`
- `lv_led_set_color(led, color)`
- `lv_led_on(led)` / `lv_led_off(led)`
- `lv_led_toggle(led)`

**Example:**

```c
lv_obj_t *led = lv_led_create(lv_screen_active());
lv_obj_set_size(led, 30, 30);
lv_led_set_color(led, lv_color_hex(0x00FF00));
lv_led_on(led);
lv_obj_center(led);
```

---

## Table (lv_table)

Grid of cells. Good for data display.

**Functions:**
- `lv_table_create(parent)`
- `lv_table_set_cell_value(table, row, col, "text")`
- `lv_table_set_row_cnt(table, n)` / `lv_table_set_col_cnt(table, n)`
- `lv_table_set_col_width(table, col, width)`

**Example:**

```c
lv_obj_t *table = lv_table_create(lv_screen_active());
lv_table_set_col_cnt(table, 3);
lv_table_set_row_cnt(table, 4);
lv_obj_set_size(table, 280, 180);
lv_obj_center(table);

lv_table_set_cell_value(table, 0, 0, "Name");
lv_table_set_cell_value(table, 0, 1, "Value");
lv_table_set_cell_value(table, 0, 2, "Unit");
lv_table_set_cell_value(table, 1, 0, "Temp");
lv_table_set_cell_value(table, 1, 1, "23");
lv_table_set_cell_value(table, 1, 2, "C");
```

---

## Chart (lv_chart)

Line, bar, or scatter chart.

**Functions:**
- `lv_chart_create(parent)`
- `lv_chart_set_type(chart, LV_CHART_TYPE_LINE/BAR/SCATTER)`
- `lv_chart_set_point_count(chart, n)`
- `lv_chart_add_series(chart, color, axis)` — returns `lv_chart_series_t *`
- `lv_chart_set_next_value(chart, series, value)`
- `lv_chart_set_range(chart, axis, min, max)`

**Example — line chart:**

```c
lv_obj_t *chart = lv_chart_create(lv_screen_active());
lv_obj_set_size(chart, 240, 150);
lv_obj_center(chart);
lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
lv_chart_set_point_count(chart, 20);
lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);

lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_hex(0x00AAFF),
                                             LV_CHART_AXIS_PRIMARY_Y);

// Push some sample values
for (int i = 0; i < 20; i++) {
    lv_chart_set_next_value(chart, ser, (i * 5) % 100);
}
```

---

## Calendar (lv_calendar)

Month-view calendar with selectable dates.

**Functions:**
- `lv_calendar_create(parent)`
- `lv_calendar_set_today_date(cal, year, month, day)`
- `lv_calendar_set_showed_date(cal, year, month)`
- `lv_calendar_get_pressed_date(cal, &date)`
- `lv_calendar_header_arrow_create(cal)` — add header with arrows

**Example:**

```c
lv_obj_t *cal = lv_calendar_create(lv_screen_active());
lv_obj_set_size(cal, 240, 240);
lv_obj_center(cal);
lv_calendar_set_today_date(cal, 2026, 9, 29);
lv_calendar_set_showed_date(cal, 2026, 9);
lv_calendar_header_arrow_create(cal);
```

---

## Tabview (lv_tabview)

Container with switchable tabs.

**Functions:**
- `lv_tabview_create(parent)`
- `lv_tabview_add_tab(tv, "Tab name")` — returns the tab's content object
- `lv_tabview_set_active(tv, idx, LV_ANIM_ON)`

**Example:**

```c
lv_obj_t *tv = lv_tabview_create(lv_screen_active());
lv_obj_set_size(tv, 300, 200);
lv_obj_center(tv);

lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Home");
lv_obj_t *tab2 = lv_tabview_add_tab(tv, "Settings");

lv_obj_t *lbl1 = lv_label_create(tab1);
lv_label_set_text(lbl1, "Home content");
lv_obj_center(lbl1);

lv_obj_t *lbl2 = lv_label_create(tab2);
lv_label_set_text(lbl2, "Settings content");
lv_obj_center(lbl2);
```

---

## Message Box (lv_msgbox)

Modal dialog with title, content, and buttons.

**Functions:**
- `lv_msgbox_create(parent)` — `NULL` parent = fullscreen modal
- `lv_msgbox_add_title(mbox, "Title")`
- `lv_msgbox_add_text(mbox, "Body text")`
- `lv_msgbox_add_footer_button(mbox, "OK")`
- `lv_msgbox_add_close_button(mbox)`

**Example:**

```c
lv_obj_t *mbox = lv_msgbox_create(NULL);
lv_msgbox_add_title(mbox, "Notice");
lv_msgbox_add_text(mbox, "Something happened.");
lv_msgbox_add_footer_button(mbox, "OK");
lv_msgbox_add_close_button(mbox);
```

---

## Spinner (lv_spinner)

Animated loading indicator.

**Functions:**
- `lv_spinner_create(parent)`
- `lv_spinner_set_anim_params(spinner, duration_ms, arc_length)`

**Example:**

```c
lv_obj_t *spinner = lv_spinner_create(lv_screen_active());
lv_obj_set_size(spinner, 60, 60);
lv_obj_center(spinner);
lv_spinner_set_anim_params(spinner, 1000, 60);
```

---

## Keyboard (lv_keyboard)

On-screen keyboard. Typically attached to a text area.

**Functions:**
- `lv_keyboard_create(parent)`
- `lv_keyboard_set_textarea(kb, ta)` — wire to a textarea
- `lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_LOWER/UPPER/NUMBER/SPECIAL)`

**Example:**

```c
lv_obj_t *ta = lv_textarea_create(lv_screen_active());
lv_obj_set_size(ta, 240, 40);
lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 10);

lv_obj_t *kb = lv_keyboard_create(lv_screen_active());
lv_keyboard_set_textarea(kb, ta);
```

---

## Common Styles Cheat Sheet

Applied with `lv_obj_set_style_<prop>(obj, value, selector)`.

| Property | Notes |
|----------|-------|
| `bg_color` | Background color |
| `bg_opa` | Background opacity (`LV_OPA_*`) |
| `border_width` / `border_color` | Border |
| `radius` | Corner rounding |
| `pad_all` / `pad_left` / `pad_top` / etc. | Inner padding |
| `text_color` | Text color |
| `text_font` | Font pointer |
| `text_align` | `LV_TEXT_ALIGN_LEFT/CENTER/RIGHT` |
| `shadow_width` / `shadow_opa` / `shadow_color` | Drop shadow |
| `outline_width` / `outline_color` | Outline (outside border) |
| `width` / `height` | Size (same as `lv_obj_set_size`) |
| `opacity` | Whole-object opacity |

**Parts (selectors):**

- `LV_PART_MAIN` — the widget's body
- `LV_PART_INDICATOR` — progress/selected portion (bar, slider, arc, switch)
- `LV_PART_KNOB` — the draggable handle (slider, arc, switch)
- `LV_PART_CURSOR` — text cursor
- `LV_PART_ITEMS` — table cells, dropdown options
- `LV_PART_SCROLLBAR` — scroll bar

**States:**

- `LV_STATE_DEFAULT`
- `LV_STATE_PRESSED`
- `LV_STATE_CHECKED`
- `LV_STATE_FOCUSED`
- `LV_STATE_DISABLED`

Combine with `|`: `LV_PART_MAIN | LV_STATE_PRESSED`.

---

## Common Event Codes

| Event | Fires when |
|-------|------------|
| `LV_EVENT_CLICKED` | Widget clicked (press + release inside) |
| `LV_EVENT_PRESSED` | Press started |
| `LV_EVENT_RELEASED` | Press ended |
| `LV_EVENT_VALUE_CHANGED` | Value changed (slider, switch, dropdown) |
| `LV_EVENT_FOCUSED` / `LV_EVENT_DEFOCUSED` | Focus gained/lost |
| `LV_EVENT_READY` | Text area "Enter" pressed |
| `LV_EVENT_CANCEL` | "Esc" pressed |
| `LV_EVENT_LONG_PRESSED` | Long press detected |
| `LV_EVENT_SCREEN_LOADED` / `LV_EVENT_SCREEN_UNLOADED` | Screen transitions |

**Handler signature:**

```c
static void my_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    void *user_data = lv_event_get_user_data(e);
    // ...
}
```

Register with:

```c
lv_obj_add_event_cb(widget, my_event_cb, LV_EVENT_CLICKED, NULL);
```

Multiple event codes can be watched with `LV_EVENT_ALL`, then checked inside the handler.

---

## Memory & Performance Tips

- **Reduce buffer sizes** before anything else. One partial display buffer of ~5–10 KB is usually enough.
- **Use `LV_LABEL_LONG_MODE_DOT`** instead of scrolling for long strings you don't need to read fully.
- **Prefer `lv_obj_set_style_*` on creation** and then don't touch it — each style change triggers a redraw.
- **Use `LV_OBJ_FLAG_HIDDEN`** instead of destroying and recreating widgets.
- **Reuse screens** — create once, load/unload as needed, rather than destroying them.
- **Disable unused widgets in `lv_conf.h`** (e.g. `LV_USE_CALENDAR 0`, `LV_USE_CHART 0`) to save flash.
- **Use static text** with `lv_label_set_text_static()` when the string is a literal — avoids heap churn.
- **`LV_USE_LOG 0`** in production builds saves both flash and CPU.
- **`LV_TICK_CUSTOM`** avoids needing a timer interrupt for LVGL ticks.

---

## References

- LVGL 9 widgets overview — https://docs.lvgl.io/master/widgets/index.html
- Style properties — https://docs.lvgl.io/master/common-widget-features/styles/style-properties.html
- Events — https://docs.lvgl.io/master/common-widget-features/events.html
- `lv_conf.h` reference — https://docs.lvgl.io/master/configuration/index.html

---

## Change Log

| Date | Change |
|------|--------|
| 2026-09-29 | Initial widget reference for LVGL 9.x |