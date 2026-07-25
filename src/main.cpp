/**
 * Last updated: July 2026
 * By: Daniel Potter
 * Description: Air quality monitor for CYD (Cheap Yellow Display) boards.
 * Supports multiple CYD variants: JC2432W328R, JC2432W328C, and ESP32-2432S028R.
 */

#include "TemplateCode.h"
#include "MainInterface.h"
#include "PeriodicScheduler.h"
#include "SensorManager.h"
#include "SettingsStore.h"
#include "WiFiConnectionManager.h"
#include <DHT.h>

TemplateCode &templateCode = TemplateCode::getInstance();
MainInterface mainInterface = MainInterface();

#ifdef MODEL_2432S028R
#define DHTPIN 27
#elif defined(MODEL_JC2432W328C)
#define DHTPIN 22
#else
#define DHTPIN 21
#endif
#define DHTTYPE DHT11

PeriodicScheduler scheduler;
SensorManager sensorManager(DHTPIN, DHTTYPE, 2000);
SettingsStore settingsStore;
WiFiConnectionManager wifiManager;

void setup()
{
  Serial.begin(115200);
  settingsStore.begin();
  if (!settingsStore.isReady())
    Serial.println("Warning: NVS settings unavailable");

  sensorManager.begin();

  if (!templateCode.begin())
  {
    Serial.println("Failed to initialize template code.!");
    while (1)
    {
      delay(1000);
    }
  }

  templateCode.applyTouchCalibration(settingsStore.loadTouchCal());

  wifiManager.begin(&settingsStore);
  mainInterface.init(&settingsStore, &templateCode, &wifiManager);

  sensorManager.onChange([&](float t, float h)
                         {
    if (!isnan(t)) mainInterface.setTemperature(t);
    if (!isnan(h)) mainInterface.setHumidity(h); });

  scheduler.addTask(std::bind(&SensorManager::update, &sensorManager), 2000);
  scheduler.addTask(std::bind(&WiFiConnectionManager::update, &wifiManager), 500);
  scheduler.addTask(std::bind(&MainInterface::update, &mainInterface), 100);
}

void loop()
{
  templateCode.update();
  scheduler.update();
  delay(10);
}
