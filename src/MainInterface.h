// MainInterface.h
/**
 * MainInterface.h
 * Last Updated: Nov 29, 2025
 * Author: Daniel Potter
 *
 * Generic display and touch test interface for CYD.
 * - Shows color/gradient test bars
 * - Displays touch coordinates; optional touch trail
 * - Simple bouncing square animation and FPS indicator
 */

#ifndef MAIN_INTERFACE_H
#define MAIN_INTERFACE_H

#include <lvgl.h>
#include <stdint.h>
#include "EnvSensor.h"

class MainInterface
{

private:
  // UI root
  lv_obj_t *mainScreen;

  // Header and info
  lv_obj_t *headerContainer;
  lv_obj_t *headerLabel;
  lv_obj_t *infoLabel;
  lv_obj_t *fpsLabel;

  // Test areas
  lv_obj_t *colorRow;
  lv_obj_t *gradientBox;
  lv_obj_t *bouncer;

  // Sensor labels
  lv_obj_t *tempLabel;
  lv_obj_t *humLabel;

  // Animation state
  int16_t bx = 10, by = 10;
  int8_t bdx = 2, bdy = 2;

  // Helpers
  void createHeader();
  void createTests();
  void createColorBars();
  void createSensorsArea();
  void updateFPS();
  void updateSensorDisplay();


public:
  MainInterface();
  ~MainInterface();

  void init();
  void update();
};

#endif // MAIN_INTERFACE_H
