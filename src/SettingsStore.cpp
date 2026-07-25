#include "SettingsStore.h"
#include <Preferences.h>

namespace
{
constexpr const char *NVS_NAMESPACE = "cydmon";
constexpr const char *KEY_SCHEMA = "schema";
constexpr const char *KEY_SSID = "wifi_ssid";
constexpr const char *KEY_PASS = "wifi_pass";
constexpr const char *KEY_AUTO_CONNECT = "wifi_auto";
constexpr const char *KEY_X_MIN = "tx_min";
constexpr const char *KEY_X_MAX = "tx_max";
constexpr const char *KEY_Y_MIN = "ty_min";
constexpr const char *KEY_Y_MAX = "ty_max";
constexpr const char *KEY_CAL_SAVED = "cal_ok";
constexpr const char *KEY_DISP_ROT = "disp_rot";
constexpr uint8_t SCHEMA_VERSION = 1;
} // namespace

bool SettingsStore::openRead(Preferences &prefs) const
{
  if (!ready)
    return false;
  return prefs.begin(NVS_NAMESPACE, true);
}

bool SettingsStore::openWrite(Preferences &prefs) const
{
  if (!ready)
    return false;
  return prefs.begin(NVS_NAMESPACE, false);
}

void SettingsStore::begin()
{
  Preferences prefs;
  if (!prefs.begin(NVS_NAMESPACE, false))
  {
    Serial.println("SettingsStore: failed to open NVS namespace");
    ready = false;
    return;
  }

  uint8_t schema = prefs.getUChar(KEY_SCHEMA, 0);
  if (schema == 0)
  {
    prefs.putUChar(KEY_SCHEMA, SCHEMA_VERSION);
    prefs.putBool(KEY_AUTO_CONNECT, true);
    Serial.println("SettingsStore: initialized new NVS namespace");
  }
  else if (schema != SCHEMA_VERSION)
  {
    Serial.printf("SettingsStore: schema %u (expected %u)\n", schema, SCHEMA_VERSION);
  }

  prefs.end();
  ready = true;

  Serial.printf("SettingsStore: WiFi saved=%s, touch cal saved=%s, auto-connect=%s\n",
                hasWifi() ? "yes" : "no",
                hasTouchCal() ? "yes" : "no",
                loadWifiAutoConnect() ? "yes" : "no");
}

bool SettingsStore::loadWifi(String &ssid, String &pass) const
{
  Preferences prefs;
  if (!openRead(prefs))
    return false;

  ssid = prefs.getString(KEY_SSID, "");
  pass = prefs.getString(KEY_PASS, "");
  prefs.end();
  return ssid.length() > 0;
}

void SettingsStore::saveWifi(const String &ssid, const String &pass)
{
  Preferences prefs;
  if (!openWrite(prefs))
    return;

  prefs.putString(KEY_SSID, ssid);
  prefs.putString(KEY_PASS, pass);
  prefs.end();
  Serial.printf("SettingsStore: saved WiFi credentials for \"%s\"\n", ssid.c_str());
}

bool SettingsStore::hasWifi() const
{
  Preferences prefs;
  if (!openRead(prefs))
    return false;

  bool saved = prefs.isKey(KEY_SSID);
  prefs.end();
  return saved;
}

void SettingsStore::clearWifi()
{
  Preferences prefs;
  if (!openWrite(prefs))
    return;

  prefs.remove(KEY_SSID);
  prefs.remove(KEY_PASS);
  prefs.end();
  Serial.println("SettingsStore: cleared WiFi credentials");
}

bool SettingsStore::loadWifiAutoConnect() const
{
  Preferences prefs;
  if (!openRead(prefs))
    return true;

  bool enabled = prefs.getBool(KEY_AUTO_CONNECT, true);
  prefs.end();
  return enabled;
}

void SettingsStore::saveWifiAutoConnect(bool enabled)
{
  Preferences prefs;
  if (!openWrite(prefs))
    return;

  prefs.putBool(KEY_AUTO_CONNECT, enabled);
  prefs.end();
}

bool SettingsStore::hasTouchCal() const
{
  Preferences prefs;
  if (!openRead(prefs))
    return false;

  bool saved = prefs.getBool(KEY_CAL_SAVED, false);
  prefs.end();
  return saved;
}

TouchCalibration SettingsStore::defaultTouchCal() const
{
  TouchCalibration cal = {};
#if defined(TOUCH_X_MIN)
  cal.xMin = TOUCH_X_MIN;
#else
  cal.xMin = 200;
#endif
#if defined(TOUCH_X_MAX)
  cal.xMax = TOUCH_X_MAX;
#else
  cal.xMax = 3700;
#endif
#if defined(TOUCH_Y_MIN)
  cal.yMin = TOUCH_Y_MIN;
#else
  cal.yMin = 240;
#endif
#if defined(TOUCH_Y_MAX)
  cal.yMax = TOUCH_Y_MAX;
#else
  cal.yMax = 3800;
#endif
  return cal;
}

TouchCalibration SettingsStore::loadTouchCal() const
{
  TouchCalibration cal = defaultTouchCal();
  Preferences prefs;
  if (!openRead(prefs))
    return cal;

  if (!prefs.getBool(KEY_CAL_SAVED, false))
  {
    prefs.end();
    return cal;
  }

  cal.xMin = prefs.getInt(KEY_X_MIN, cal.xMin);
  cal.xMax = prefs.getInt(KEY_X_MAX, cal.xMax);
  cal.yMin = prefs.getInt(KEY_Y_MIN, cal.yMin);
  cal.yMax = prefs.getInt(KEY_Y_MAX, cal.yMax);
  prefs.end();
  return cal;
}

void SettingsStore::saveTouchCal(const TouchCalibration &cal)
{
  Preferences prefs;
  if (!openWrite(prefs))
    return;

  prefs.putInt(KEY_X_MIN, cal.xMin);
  prefs.putInt(KEY_X_MAX, cal.xMax);
  prefs.putInt(KEY_Y_MIN, cal.yMin);
  prefs.putInt(KEY_Y_MAX, cal.yMax);
  prefs.putBool(KEY_CAL_SAVED, true);
  prefs.end();
  Serial.printf("SettingsStore: saved touch cal X:%d..%d Y:%d..%d\n",
                cal.xMin, cal.xMax, cal.yMin, cal.yMax);
}

uint8_t SettingsStore::loadDisplayRotation() const
{
  Preferences prefs;
  if (!openRead(prefs))
    return 0;

  uint8_t rotation = prefs.getUChar(KEY_DISP_ROT, 0);
  prefs.end();
  return rotation & 0x3;
}

void SettingsStore::saveDisplayRotation(uint8_t rotation)
{
  Preferences prefs;
  if (!openWrite(prefs))
    return;

  prefs.putUChar(KEY_DISP_ROT, rotation & 0x3);
  prefs.end();
  Serial.printf("SettingsStore: saved display rotation %u\n", rotation & 0x3);
}

void SettingsStore::resetTouchCal()
{
  Preferences prefs;
  if (!openWrite(prefs))
    return;

  prefs.remove(KEY_X_MIN);
  prefs.remove(KEY_X_MAX);
  prefs.remove(KEY_Y_MIN);
  prefs.remove(KEY_Y_MAX);
  prefs.remove(KEY_CAL_SAVED);
  prefs.end();
  Serial.println("SettingsStore: reset touch calibration to defaults");
}

void SettingsStore::clearAll()
{
  Preferences prefs;
  if (!openWrite(prefs))
    return;

  prefs.clear();
  prefs.putUChar(KEY_SCHEMA, SCHEMA_VERSION);
  prefs.putBool(KEY_AUTO_CONNECT, true);
  prefs.end();
  Serial.println("SettingsStore: cleared all persisted settings");
}
