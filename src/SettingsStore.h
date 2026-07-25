#pragma once

#include <Arduino.h>

class Preferences;

struct TouchCalibration
{
  int xMin;
  int xMax;
  int yMin;
  int yMax;
};

/**
 * Persistent settings backed by ESP32 NVS (Arduino Preferences library).
 * Survives power cycles and firmware updates within the same partition scheme.
 */
class SettingsStore
{
public:
  void begin();

  bool isReady() const { return ready; }

  bool loadWifi(String &ssid, String &pass) const;
  void saveWifi(const String &ssid, const String &pass);
  bool hasWifi() const;
  void clearWifi();

  bool loadWifiAutoConnect() const;
  void saveWifiAutoConnect(bool enabled);

  bool hasTouchCal() const;
  TouchCalibration loadTouchCal() const;
  void saveTouchCal(const TouchCalibration &cal);
  void resetTouchCal();
  TouchCalibration defaultTouchCal() const;

  /** LVGL display rotation index: 0=0°, 1=90°, 2=180°, 3=270°. */
  uint8_t loadDisplayRotation() const;
  void saveDisplayRotation(uint8_t rotation);

  void clearAll();

private:
  bool ready = false;

  bool openRead(Preferences &prefs) const;
  bool openWrite(Preferences &prefs) const;
};
