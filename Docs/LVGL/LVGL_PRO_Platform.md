# Designing a UI in LVGL Pro and Moving It into PlatformIO

A practical, end-to-end guide for building a UI in **LVGL Pro**, exporting it, and embedding it in a **PlatformIO / ESP32** project (specifically the CYD ESP32-2432S028R, but the workflow is board-agnostic).

---

## Table of Contents

- [Overview](#overview)
- [Mental Model](#mental-model)
- [Part 1 — Set Up the LVGL Pro Project](#part-1--set-up-the-lvgl-pro-project)
- [Part 2 — Design the UI](#part-2--design-the-ui)
- [Part 3 — Export the Code](#part-3--export-the-code)
- [Part 4 — Move the Files into PlatformIO](#part-4--move-the-files-into-platformio)
- [Part 5 — Wire It Up in `main.cpp`](#part-5--wire-it-up-in-maincpp)
- [Part 6 — Re-Exporting After UI Changes](#part-6--re-exporting-after-ui-changes)
- [What Lives Where](#what-lives-where)
- [Rules to Live By](#rules-to-live-by)
- [Troubleshooting Quick Reference](#troubleshooting-quick-reference)

---

## Overview

LVGL Pro is a **designer** — you draw screens, drop widgets, and it generates C code. It does **not** know about your display, your ESP32, or your build system. That separation is deliberate and is what makes the workflow portable.

The three stages:
