#pragma once

#include <Arduino.h>
#include <functional>
#include <WebServer.h>

class SettingsStore;
class WiFiConnectionManager;
class SensorManager;
class TemplateCode;

/**
 * Local HTTP dashboard and settings API (LAN only).
 * Serves a richer web UI when WiFi is connected; changes sync with NVS and the touchscreen.
 */
class WebServerManager
{
public:
  using SettingsChangedCallback = std::function<void()>;

  void begin(SettingsStore *store,
             WiFiConnectionManager *wifi,
             SensorManager *sensors,
             TemplateCode *display,
             SettingsChangedCallback onSettingsChanged = nullptr);

  void update();

  bool isActive() const;
  String accessUrl() const;

private:
  SettingsStore *settings = nullptr;
  WiFiConnectionManager *wifi = nullptr;
  SensorManager *sensors = nullptr;
  TemplateCode *display = nullptr;
  SettingsChangedCallback settingsChangedCb;
  WebServer server{80};
  bool mdnsActive = false;
  bool serverStarted = false;

  void ensureMdns();
  void stopMdns();
  void registerRoutes();

  void handleRoot();
  void handleApiStatus();
  void handleApiSettings();
  void handleWifiScan();
  void handleWifiNetworks();
  void handleWifiConnect();
  void handleWifiForget();
  void handleSetupPortal();
  void handleDisplayRotation();
  void handleTouchCalSave();
  void handleTouchCalReset();
  void handleApiTouchSample();

  static String jsonEscape(const String &s);
  static String readBody(WebServer &srv);
  static String jsonStringField(const String &body, const char *key);
  static int jsonIntField(const String &body, const char *key, int fallback);
};
