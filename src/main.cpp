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
#include "WebServerManager.h"
#include "WiFiConnectionManager.h"

TemplateCode &templateCode = TemplateCode::getInstance();
MainInterface mainInterface = MainInterface();

PeriodicScheduler scheduler;
SensorManager sensorManager(3000);
SettingsStore settingsStore;
WiFiConnectionManager wifiManager;
WebServerManager webServer;

static String webHintText;

static void refreshWebHint()
{
  if (wifiManager.isSetupPortalActive())
  {
    webHintText = "Browser: join CYD-Setup -> 192.168.4.1";
    mainInterface.setWebAccessHint(webHintText.c_str());
    return;
  }

  if (wifiManager.status() == WiFiConnStatus::Connected)
  {
    String ip = wifiManager.ipAddress();
    if (ip.length())
      webHintText = String("Browser: http://") + ip;
    else
      webHintText = "Browser: cydmon.local";
    mainInterface.setWebAccessHint(webHintText.c_str());
    return;
  }

  webHintText = "WiFi: Settings -> WiFi or Browser Setup";
  mainInterface.setWebAccessHint(webHintText.c_str());
}

void setup()
{
  Serial.begin(115200);
#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
  // ESP32-S3 USB CDC: early prints are lost until the host opens the port
  unsigned long usbWaitStart = millis();
  while (!Serial && (millis() - usbWaitStart) < 5000)
    delay(10);
#endif
  Serial.println();
  Serial.println("CYD Air Monitor boot");

  settingsStore.begin();
  if (!settingsStore.isReady())
    Serial.println("Warning: NVS settings unavailable");

  sensorManager.begin();
  Serial.printf("Sensor poll interval: %u ms\n", 3000U);

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

  webServer.begin(&settingsStore, &wifiManager, &sensorManager, &templateCode, []() {
    mainInterface.refreshFromSettings();
    refreshWebHint();
  });

  // Log sensor wiring after USB/display init (boot lines are often lost on ESP32-S3 CDC)
  sensorManager.logStartupDiagnostics();

  sensorManager.onChange([&](float t, float h)
                         {
    if (!isnan(t)) mainInterface.setTemperature(t);
    if (!isnan(h)) mainInterface.setHumidity(h); });

  // Prime dashboard (scheduler first tick is 2s later)
  sensorManager.update();

  scheduler.addTask(std::bind(&SensorManager::update, &sensorManager), 3000);
  scheduler.addTask(std::bind(&WiFiConnectionManager::update, &wifiManager), 500);
  scheduler.addTask(std::bind(&WebServerManager::update, &webServer), 50);
  scheduler.addTask(std::bind(&MainInterface::update, &mainInterface), 100);
  scheduler.addTask(refreshWebHint, 2000);
}

void loop()
{
  templateCode.update();
  scheduler.update();

#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
  static bool usbLogReady = false;
  if (!usbLogReady && Serial)
  {
    usbLogReady = true;
    Serial.println();
    Serial.println("--- USB serial connected ---");
    sensorManager.logStartupDiagnostics();
  }
#endif

  delay(10);
}
