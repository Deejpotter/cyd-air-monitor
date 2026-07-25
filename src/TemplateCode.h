/**
 * Last updated: 2nd Feb 2025
 * By: Daniel Potter
 * Description: This file contains the template code for setting up the screen and touch interface.
 */

#ifndef TEMPLATE_CODE_H
#define TEMPLATE_CODE_H

#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#if defined(MODEL_JC2432W328R)
#include <XPT2046_Touchscreen.h>
#elif defined(MODEL_JC2432W328C)
#include <bb_captouch.h>
#elif defined(MODEL_2432S028R)
#include <XPT2046_Touchscreen.h>
#endif
#include "RGBledDriver.h"
#include "DisplayConfig.h"
#include "SettingsStore.h"
#if defined(MODEL_JC2432W328R)
// Resistive (XPT2046) pins
#ifndef XPT2046_CS
#ifdef TOUCH_CS
#define XPT2046_CS TOUCH_CS
#else
#define XPT2046_CS 33
#endif
#endif
#ifndef XPT2046_IRQ
#ifdef TOUCH_IRQ
#define XPT2046_IRQ TOUCH_IRQ
#else
#define XPT2046_IRQ 36
#endif
#endif
#ifndef XPT2046_CLK
#ifdef TFT_SCLK
#define XPT2046_CLK TFT_SCLK
#else
#define XPT2046_CLK 14
#endif
#endif
#ifndef XPT2046_MISO
#ifdef TFT_MISO
#define XPT2046_MISO TFT_MISO
#else
#define XPT2046_MISO 12
#endif
#endif
#ifndef XPT2046_MOSI
#ifdef TFT_MOSI
#define XPT2046_MOSI TFT_MOSI
#else
#define XPT2046_MOSI 13
#endif
#endif
#endif // MODEL_JC2432W328R

// 2432S028R has separate touch SPI bus (different from display SPI)
#if defined(MODEL_2432S028R)
// Resistive (XPT2046) pins - separate SPI bus
#ifndef XPT2046_CS
#ifdef TOUCH_CS
#define XPT2046_CS TOUCH_CS
#else
#define XPT2046_CS 33
#endif
#endif
#ifndef XPT2046_IRQ
#ifdef TOUCH_IRQ
#define XPT2046_IRQ TOUCH_IRQ
#else
#define XPT2046_IRQ 36
#endif
#endif
#ifndef XPT2046_CLK
#ifdef TOUCH_CLK
#define XPT2046_CLK TOUCH_CLK
#else
#define XPT2046_CLK 25
#endif
#endif
#ifndef XPT2046_MISO
#ifdef TOUCH_MISO
#define XPT2046_MISO TOUCH_MISO
#else
#define XPT2046_MISO 39
#endif
#endif
#ifndef XPT2046_MOSI
#ifdef TOUCH_MOSI
#define XPT2046_MOSI TOUCH_MOSI
#else
#define XPT2046_MOSI 32
#endif
#endif
#endif // MODEL_2432S028R

// Pin mapping via PlatformIO build flags (prefer PIO-defined macros)
#if defined(MODEL_JC2432W328R)
// Touch Calibration Values (overridable via PlatformIO build flags)
// Define TOUCH_X_MIN, TOUCH_X_MAX, TOUCH_Y_MIN, TOUCH_Y_MAX in platformio.ini to customize.
// Defaults match common JC2432W328R panels.
#ifndef TOUCH_X_MIN
#define TOUCH_X_MIN 200
#endif
#ifndef TOUCH_X_MAX
#define TOUCH_X_MAX 3700
#endif
#ifndef TOUCH_Y_MIN
#define TOUCH_Y_MIN 240
#endif
#ifndef TOUCH_Y_MAX
#define TOUCH_Y_MAX 3800
#endif
#endif

#if defined(MODEL_2432S028R)
// Touch Calibration Values for 2432S028R (overridable via PlatformIO build flags)
// These defaults should work for most 2432S028R panels - adjust if touch is off
#ifndef TOUCH_X_MIN
#define TOUCH_X_MIN 200
#endif
#ifndef TOUCH_X_MAX
#define TOUCH_X_MAX 3700
#endif
#ifndef TOUCH_Y_MIN
#define TOUCH_Y_MIN 240
#endif
#ifndef TOUCH_Y_MAX
#define TOUCH_Y_MAX 3800
#endif
#endif

#if defined(MODEL_JC2432W328C)
#ifndef CST820_SDA
#ifdef I2C_SDA
#define CST820_SDA I2C_SDA
#else
#define CST820_SDA 33
#endif
#endif
#ifndef CST820_SCL
#ifdef I2C_SCL
#define CST820_SCL I2C_SCL
#else
#define CST820_SCL 32
#endif
#endif
#ifndef CST820_RST
#define CST820_RST 25
#endif
#ifndef CST820_INT
#define CST820_INT 21
#endif
#endif

struct TouchSample
{
  bool pressed;
  int32_t rawX;
  int32_t rawY;
  int32_t mappedX;
  int32_t mappedY;
};

class TemplateCode
{
private:
  // Hardware Instances
#if defined(MODEL_JC2432W328R)
  SPIClass mySpi; // Reference to avoid copy
  XPT2046_Touchscreen ts;
#endif
#if defined(MODEL_JC2432W328C)
  BBCapTouch ts;
#endif
#if defined(MODEL_2432S028R)
  SPIClass mySpi; // Reference to avoid copy
  XPT2046_Touchscreen ts;
#endif
  TFT_eSPI tft;

  // LVGL Buffer
  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf[SCREEN_WIDTH * SCREEN_HEIGHT / 10];

  // Singleton instance
  static TemplateCode *instance;

  TouchCalibration touchCal;

  // Private constructor for singleton
  TemplateCode();

  // Helper initialization methods
  void initializeHardware();
  void initializeLVGL();
  void setupTouchscreen();
  static void readTouchpad(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
  void setupDisplay();

public:
  // Delete copy constructor and assignment operator
  TemplateCode(const TemplateCode &) = delete;
  TemplateCode operator=(const TemplateCode &) = delete;

  // Singleton access
  static TemplateCode &getInstance();

  // Main initialization
  bool begin();
  void applyTouchCalibration(const TouchCalibration &cal);
  TouchCalibration touchCalibration() const { return touchCal; }
  bool sampleTouch(TouchSample &sample);
  bool isResistiveTouch() const;
  const char *touchTypeName() const;

  // LVGL callback handlers
  static void flushDisplay(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
  // Overload for resistive/capacitive handled in .cpp

// Debug functionality
#if LV_USE_LOG != 0
  static void debugPrint(const char *buf);
#endif

  // Periodic tasks
  void update();
};

#endif // TEMPLATE_CODE_H
