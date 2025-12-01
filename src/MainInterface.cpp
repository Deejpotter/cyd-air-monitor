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
  tempLabel = nullptr;
  humLabel = nullptr;
}

/**
 * Destructor
 */
MainInterface::~MainInterface() {}

void MainInterface::init()
{
  // Create main screen
  mainScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(mainScreen, lv_color_hex(0x000000), 0);
  lv_obj_set_style_pad_all(mainScreen, 4, 0);

  // Flex column layout
  lv_obj_set_layout(mainScreen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(mainScreen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(mainScreen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

  // Header + sensors
  createHeader();
  createSensorsArea();

  // Load screen
  lv_scr_load(mainScreen);
}

void MainInterface::createHeader()
{
  uint16_t w = lv_disp_get_hor_res(NULL);
  uint16_t h = lv_disp_get_ver_res(NULL);

  headerContainer = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(headerContainer);
  lv_obj_set_size(headerContainer, w, 42);
  lv_obj_set_style_bg_color(headerContainer, lv_color_hex(0x1E1E1E), 0);
  lv_obj_set_style_pad_all(headerContainer, 6, 0);

  headerLabel = lv_label_create(headerContainer);
  lv_obj_set_style_text_color(headerLabel, lv_color_hex(0xFFFFFF), 0);

#if defined(MODEL_JC2432W328R)
  const char *model = "JC2432W328R (Resistive)";
#elif defined(MODEL_JC2432W328C)
  const char *model = "JC2432W328C (Capacitive)";
#else
  const char *model = "CYD";
#endif
  lv_label_set_text(headerLabel, "Air Quality Monitor");
  lv_obj_align(headerLabel, LV_ALIGN_CENTER, 0, 0);
}

void MainInterface::createSensorsArea()
{
  // Create temperature label
  tempLabel = lv_label_create(mainScreen);
  lv_obj_set_style_text_color(tempLabel, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(tempLabel, &lv_font_montserrat_32, 0);
  lv_label_set_text(tempLabel, "--°C");
  lv_obj_align(tempLabel, LV_ALIGN_CENTER, 0, -30);

  // Create humidity label
  humLabel = lv_label_create(mainScreen);
  lv_obj_set_style_text_color(humLabel, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(humLabel, &lv_font_montserrat_32, 0);
  lv_label_set_text(humLabel, "--%");
  lv_obj_align(humLabel, LV_ALIGN_CENTER, 0, 30);
}

void MainInterface::update()
{

  // Update environment sensor display every second (reads are cached by EnvSensor)
  static unsigned long lastSensorUpdate = 0;
  unsigned long now = millis();
  if (now - lastSensorUpdate >= 1000)
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
    lv_snprintf(tbuf, sizeof(tbuf), "%.1f°C", t);
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
    lv_snprintf(hbuf, sizeof(hbuf), "%.1f%%", h);
    lv_label_set_text(humLabel, hbuf);
  }
  else if (humLabel)
  {
    lv_label_set_text(humLabel, "--%");
  }
}
