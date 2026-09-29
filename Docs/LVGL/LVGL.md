# LVGL Pro → PlatformIO Setup Guide

This document explains how to design a UI in **LVGL Pro** and get it running on an **ESP32-CYD (ESP32-2432S028R)** — the 2.8" 240×320 resistive touch display module with ILI9341 + XPT2046.

The workflow is: **design in LVGL Pro → export C code → drop into PlatformIO → write glue code → flash.**

---

## Table of Contents

- [Hardware](#hardware)
- [Prerequisites](#prerequisites)
- [Project Layout](#project-layout)
- [Step 1 — Design the UI in LVGL Pro](#step-1--design-the-ui-in-lvgl-pro)
- [Step 2 — Export the C Code](#step-2--export-the-c-code)
- [Step 3 — Import into PlatformIO](#step-3--import-into-platformio)
- [Step 4 — Configure `platformio.ini`](#step-4--configure-platformioini)
- [Step 5 — Create `lv_conf.h`](#step-5--create-lv_confh)
- [Step 6 — Write `main.cpp`](#step-6--write-maincpp)
- [Step 7 — Build and Flash](#step-7--build-and-flash)
- [Common Pitfalls](#common-pitfalls)
- [What LVGL Pro Generates](#what-lvgl-pro-generates)
- [Why Some Choices Were Made](#why-some-choices-were-made)

---

## Hardware

| Component | Detail |
|-----------|--------|
| MCU | ESP32-WROOM-32E |
| Display | 2.8" ILI9341, 240×320, SPI |
| Touch | XPT2046 resistive, SPI |
| Backlight | GPIO 21, active HIGH |
| Toolchain | PlatformIO + Arduino framework |

**CYD pin map (typical):**

| Signal | GPIO |
|--------|------|
| TFT_MOSI | 13 |
| TFT_SCLK | 14 |
| TFT_CS   | 15 |
| TFT_DC   | 2  |
| TFT_RST  | -1 (not wired) |
| TFT_BL   | 21 |
| Touch IRQ  | 36 |
| Touch MOSI | 32 |
| Touch MISO | 39 |
| Touch CLK  | 25 |
| Touch CS   | 33 |

> Verify against your specific CYD board revision — some variants differ.

---

## Prerequisites

- **VS Code** with the **PlatformIO IDE** extension installed
- **LVGL Pro Editor** (standalone app or VS Code extension)
- PlatformIO CLI available (`pio --version`)

---

## Project Layout
