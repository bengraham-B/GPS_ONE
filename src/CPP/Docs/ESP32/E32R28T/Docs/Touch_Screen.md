# Touchscreen (XPT2046) on ESP32-2432S028R — Reference Guide

> Board: **ESP32-2432S028R** ("Cheap Yellow Display" / CYD)
> Touch Controller: **XPT2046** (4-wire resistive) · Interface: SPI
> Framework: **PlatformIO + Arduino** (using `XPT2046_Touchscreen` by Paul Stoffregen)

---

## 🧠 How It Works

Your CYD uses a **4-wire resistive touchscreen**. It consists of two transparent conductive layers separated by a small gap. When you press the screen, the layers make contact at that point.

The **XPT2046** chip measures the resistance at the contact point to determine the X and Y coordinates. This is done by applying a voltage across one layer and reading the resulting voltage on the other layer, which is proportional to the position of the touch.

**Communication:** The XPT2046 talks to the ESP32 over **SPI** (Serial Peripheral Interface). It also has an **IRQ (Interrupt Request)** pin, which goes LOW when the screen is touched, allowing the ESP32 to react immediately.

**ADC Resolution:** The XPT2046 provides **12-bit** raw values, ranging from **0 to 4095**.

---

## 🔌 Pin Connections (CYD)

| Signal        | GPIO | Notes                              |
|---------------|------|------------------------------------|
| XPT2046_IRQ   | 36   | Touch interrupt (goes LOW on touch)|
| XPT2046_MOSI  | 32   | Data from ESP32 to XPT2046         |
| XPT2046_MISO  | 39   | Data from XPT2046 to ESP32         |
| XPT2046_CLK   | 25   | SPI Clock                          |
| XPT2046_CS    | 33   | Chip Select                        |

> ⚠️ The touchscreen uses a **separate SPI bus (VSPI)** from the display (HSPI) on the CYD. This is why the pins differ from the TFT pins.

---

## 📦 Library Setup (PlatformIO)

```ini
lib_deps =
    bodmer/TFT_eSPI@^2.5.43
    paulstoffregen/XPT2046_Touchscreen@^1.4
```

---

## 💻 Basic Example: Reading Touch Coordinates

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

// Touchscreen pins for ESP32-2432S028R
#define XPT2046_IRQ   36
#define XPT2046_MOSI  32
#define XPT2046_MISO  39
#define XPT2046_CLK   25
#define XPT2046_CS    33

// Dedicated SPI instance for the touchscreen (VSPI)
SPIClass touchscreenSPI = SPIClass(VSPI);

// Touchscreen object (CS pin, IRQ pin)
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("XPT2046 Touch Test");

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(1); // Match display orientation
}

void loop() {
  if (touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    Serial.printf("Raw: X = %d, Y = %d, Z = %d\n", p.x, p.y, p.z);
  }
  delay(50);
}
```

**Expected output** when touching the screen:
```
Raw: X = 1823, Y = 2745, Z = 612
```

> The **Z value** represents pressure. Values above ~400 typically indicate a valid touch.

---

## 🎯 Mapping Raw Values to Screen Coordinates

```cpp
// Display in landscape mode (320x240)
// These calibration values are typical for CYD but vary per unit
#define RAW_X_MIN  200
#define RAW_X_MAX  3700
#define RAW_Y_MIN  240
#define RAW_Y_MAX  3800

void loop() {
  if (touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();

    int screenX = map(p.x, RAW_X_MIN, RAW_X_MAX, 0, 319);
    int screenY = map(p.y, RAW_Y_MIN, RAW_Y_MAX, 0, 239);

    screenX = constrain(screenX, 0, 319);
    screenY = constrain(screenY, 0, 239);

    Serial.printf("Screen: X = %d, Y = %d\n", screenX, screenY);
  }
}
```

> 📝 The raw min/max values above are **starting points**. Run a calibration sketch to get accurate values for your unit — the CYD has unit-to-unit variation in the resistive panel.

---

## 🔧 Calibration

- **Repository:** `CF20852/ESP32-2432S028-Touchscreen-Calibration`
- Displays six crosshairs one at a time; tap each one and it computes transformation coefficients.
- Coefficients saved to **NVS** using the `Preferences` library (persist across reboots).
- Uses the **TI appnote** coordinate transformation equations.

---

## ⚠️ Common Issues

| Symptom                     | Cause                                    | Fix                                        |
|-----------------------------|------------------------------------------|--------------------------------------------|
| `touched()` always false    | IRQ pin not wired / wrong pin            | Verify `XPT2046_IRQ = 36`                  |
| Coordinates inverted        | Wrong rotation                           | Try `touchscreen.setRotation(0..3)`        |
| X and Y swapped             | Rotation mismatch with display           | Match `setRotation` with `tft.setRotation` |
| Jittery values              | SPI noise / long wires                   | Add `delay(30)` between reads              |
| SD card and touch conflict  | Shared SPI bus                           | Use separate SPI instances (HSPI / VSPI)   |
| Inaccurate but consistent   | Wrong raw min/max in `map()`             | Run calibration sketch                     |

---

## 🔗 References

- **TFT_eSPI** (Bodmer) — https://github.com/Bodmer/TFT_eSPI
- **XPT2046_Touchscreen** (Paul Stoffregen) — https://github.com/PaulStoffregen/XPT2046_Touchscreen
- **CYD Touch Calibration** — https://github.com/CF20852/ESP32-2432S028-Touchscreen-Calibration
- **XPT2046 Datasheet** — XPT2046 · 4-wire resistive touchscreen controller
- **TI Appnote** — "Calibrating Touch Screen Systems" (coordinate transformation equations)

---

## 💡 Golden Rules

1. **Touchscreen and TFT use different SPI buses** on the CYD — don't share `SPIClass`.
2. **Match rotations** between `tft.setRotation()` and `touchscreen.setRotation()`.
3. **Calibrate once per unit** — resistive panels vary.
4. **Store calibration in NVS** so it survives reboots.
5. **Use the IRQ pin** for responsive input instead of polling.