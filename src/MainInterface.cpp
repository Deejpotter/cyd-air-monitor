/**
 * Last Updated: Dec 1, 2025
 * Author: Daniel Potter
 *
 * Air quality monitor interface:
 * - Header with device info
 * - Temperature and humidity display (EnvSensor, DHT11 on GPIO21)
 */

#include "MainInterface.h"
#include <stdio.h>
#include "EnvSensor.h"

/**
 * Constructor: Initializes all UI element pointers to nullptr
 */
MainInterface::MainInterface()
{
  mainScreen = nullptr;
  headerContainer = nullptr;
  headerLabel = nullptr;
  tempCard = nullptr;
  tempLabelHeader = nullptr;
  tempLabel = nullptr;
  humCard = nullptr;
  humLabelHeader = nullptr;
  humLabel = nullptr;
}

/**
 * Destructor
 */
MainInterface::~MainInterface() {}

void MainInterface::init()
{
  // Create main screen with dark blue-gray background
  mainScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(mainScreen, lv_color_hex(0x1a1d29), 0);
  lv_obj_set_style_pad_all(mainScreen, 8, 0);

  // Flex column layout
  lv_obj_set_layout(mainScreen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(mainScreen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(mainScreen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_gap(mainScreen, 12, 0);

  // Header + sensors
  createHeader();
  createSensorsArea();

  // Load screen
  lv_scr_load(mainScreen);
}

void MainInterface::createHeader()
{
  uint16_t w = lv_disp_get_hor_res(NULL);

  headerContainer = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(headerContainer);
  lv_obj_set_size(headerContainer, w - 16, 38);
  lv_obj_set_style_bg_color(headerContainer, lv_color_hex(0x2a2d3a), 0);
  lv_obj_set_style_bg_opa(headerContainer, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(headerContainer, 8, 0);
  lv_obj_set_style_pad_all(headerContainer, 8, 0);
  lv_obj_set_style_border_width(headerContainer, 1, 0);
  lv_obj_set_style_border_color(headerContainer, lv_color_hex(0x3a3d4a), 0);

  headerLabel = lv_label_create(headerContainer);
  lv_obj_set_style_text_color(headerLabel, lv_color_hex(0xE0E0E0), 0);
  lv_label_set_text(headerLabel, "🏠 Air Monitor");
  lv_obj_align(headerLabel, LV_ALIGN_CENTER, 0, 0);
}

void MainInterface::createSensorsArea()
{
  uint16_t w = lv_disp_get_hor_res(NULL);
  uint16_t cardWidth = w - 16;
  uint16_t cardHeight = 75;

  // Temperature Card (warm orange tones)
  tempCard = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(tempCard);
  lv_obj_set_size(tempCard, cardWidth, cardHeight);
  lv_obj_set_style_bg_color(tempCard, lv_color_hex(0x2d2420), 0);
  lv_obj_set_style_bg_opa(tempCard, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(tempCard, 12, 0);
  lv_obj_set_style_pad_all(tempCard, 12, 0);
  lv_obj_set_style_border_width(tempCard, 2, 0);
  lv_obj_set_style_border_color(tempCard, lv_color_hex(0xFF6B35), 0);
  lv_obj_set_style_shadow_width(tempCard, 8, 0);
  lv_obj_set_style_shadow_color(tempCard, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(tempCard, LV_OPA_30, 0);

  // Temperature header label
  tempLabelHeader = lv_label_create(tempCard);
  lv_obj_set_style_text_color(tempLabelHeader, lv_color_hex(0xFF8C61), 0);
  lv_label_set_text(tempLabelHeader, "🌡️ TEMPERATURE");
  lv_obj_align(tempLabelHeader, LV_ALIGN_TOP_LEFT, 0, 0);

  // Temperature value label
  tempLabel = lv_label_create(tempCard);
  lv_obj_set_style_text_color(tempLabel, lv_color_hex(0xFFFFFF), 0);
  static lv_style_t style_temp_val;
  lv_style_init(&style_temp_val);
  lv_style_set_text_font(&style_temp_val, &lv_font_montserrat_28);
  lv_obj_add_style(tempLabel, &style_temp_val, 0);
  lv_label_set_text(tempLabel, "--°C");
  lv_obj_align(tempLabel, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

  // Humidity Card (cool blue tones)
  humCard = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(humCard);
  lv_obj_set_size(humCard, cardWidth, cardHeight);
  lv_obj_set_style_bg_color(humCard, lv_color_hex(0x1a2332), 0);
  lv_obj_set_style_bg_opa(humCard, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(humCard, 12, 0);
  lv_obj_set_style_pad_all(humCard, 12, 0);
  lv_obj_set_style_border_width(humCard, 2, 0);
  lv_obj_set_style_border_color(humCard, lv_color_hex(0x4A90E2), 0);
  lv_obj_set_style_shadow_width(humCard, 8, 0);
  lv_obj_set_style_shadow_color(humCard, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(humCard, LV_OPA_30, 0);

  // Humidity header label
  humLabelHeader = lv_label_create(humCard);
  lv_obj_set_style_text_color(humLabelHeader, lv_color_hex(0x6BB6FF), 0);
  lv_label_set_text(humLabelHeader, "💧 HUMIDITY");
  lv_obj_align(humLabelHeader, LV_ALIGN_TOP_LEFT, 0, 0);

  // Humidity value label
  humLabel = lv_label_create(humCard);
  lv_obj_set_style_text_color(humLabel, lv_color_hex(0xFFFFFF), 0);
  static lv_style_t style_hum_val;
  lv_style_init(&style_hum_val);
  lv_style_set_text_font(&style_hum_val, &lv_font_montserrat_28);
  lv_obj_add_style(humLabel, &style_hum_val, 0);
  lv_label_set_text(humLabel, "--%");
  lv_obj_align(humLabel, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
}

void MainInterface::update()
{
  // Update environment sensor display every 2 seconds
  static unsigned long lastSensorUpdate = 0;
  unsigned long now = millis();
  if (now - lastSensorUpdate >= 2000)
  {
    envSensor.read();
    updateSensorDisplay();
    lastSensorUpdate = now;
  }
}

void MainInterface::updateSensorDisplay()
{
  float t = envSensor.getTemperature();
  float h = envSensor.getHumidity();

  // Update temperature label
  if (isfinite(t) && tempLabel)
  {
    char tbuf[32];
    snprintf(tbuf, sizeof(tbuf), "%.0f°C", t);
    lv_label_set_text(tempLabel, tbuf);
  }
  else if (tempLabel)
  {
    lv_label_set_text(tempLabel, "--°C");
  }

  // Update humidity label
  if (isfinite(h) && humLabel)
  {
    char hbuf[32];
    snprintf(hbuf, sizeof(hbuf), "%.0f%%", h);
    lv_label_set_text(humLabel, hbuf);
  }
  else if (humLabel)
  {
    lv_label_set_text(humLabel, "--%");
  }
}
