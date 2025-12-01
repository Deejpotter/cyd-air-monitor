// MainInterface.h
/**
 * MainInterface.h
 * Last Updated: Dec 1, 2025
 * Author: Daniel Potter
 *
 * Air quality monitor interface for CYD.
 * - Temperature and humidity display (DHT11 on GPIO21)
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

  // Sensor cards and labels
  lv_obj_t *tempCard;
  lv_obj_t *tempLabelHeader;
  lv_obj_t *tempLabel;

  lv_obj_t *humCard;
  lv_obj_t *humLabelHeader;
  lv_obj_t *humLabel;

  // Helpers
  void createHeader();
  void createSensorsArea();
  void updateSensorDisplay();

public:
  MainInterface();
  ~MainInterface();

  void init();
  void update();
};

#endif // MAIN_INTERFACE_H
