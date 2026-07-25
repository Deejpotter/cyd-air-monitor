#pragma once

/**
 * Display and touch orientation configuration.
 *
 * Set per-board values in platformio.ini. See docs/DISPLAY_CONFIG.md for the
 * full compatibility matrix and tuning guide.
 *
 * Key concepts:
 * - LVGL_ROTATION: logical UI frame (0/2 portrait 240x320, 1/3 landscape 320x240)
 * - TFT_DISPLAY_ROTATION: argument to tft.setRotation() — may differ from LVGL
 * - TFT_MADCTL_OVERRIDE: optional ST7789 MADCTL byte after setRotation (axis fix)
 * - TOUCH_ROTATION: XPT2046 setRotation() for resistive boards
 * - TOUCH_MIRROR_X/Y: apply after raw->screen mapping for resistive touch
 * - CAP_TOUCH_NATIVE_LANDSCAPE: CST816/CST820 portrait raw -> landscape transform
 */

// LVGL logical rotation (defaults to legacy TFT_ROTATION if set)
#ifndef LVGL_ROTATION
#ifdef TFT_ROTATION
#define LVGL_ROTATION TFT_ROTATION
#else
#define LVGL_ROTATION 1
#endif
#endif

#ifndef TFT_DISPLAY_ROTATION
#define TFT_DISPLAY_ROTATION LVGL_ROTATION
#endif

#ifndef TOUCH_ROTATION
#define TOUCH_ROTATION LVGL_ROTATION
#endif

#ifndef TOUCH_MIRROR_X
#define TOUCH_MIRROR_X 0
#endif

#ifndef TOUCH_MIRROR_Y
#define TOUCH_MIRROR_Y 0
#endif

#ifndef CAP_TOUCH_NATIVE_LANDSCAPE
#define CAP_TOUCH_NATIVE_LANDSCAPE 0
#endif

#ifndef TFT_INVERT_DISPLAY
#define TFT_INVERT_DISPLAY 0
#endif

// Native panel resolution (ST7789 CYD panels)
static constexpr uint16_t PANEL_WIDTH = 240;
static constexpr uint16_t PANEL_HEIGHT = 320;

#if (LVGL_ROTATION == 0) || (LVGL_ROTATION == 2)
static constexpr uint16_t SCREEN_WIDTH = PANEL_WIDTH;
static constexpr uint16_t SCREEN_HEIGHT = PANEL_HEIGHT;
#else
static constexpr uint16_t SCREEN_WIDTH = PANEL_HEIGHT;
static constexpr uint16_t SCREEN_HEIGHT = PANEL_WIDTH;
#endif
