#include "WebServerManager.h"
#include "DisplayConfig.h"
#include "SensorManager.h"
#include "SettingsStore.h"
#include "TemplateCode.h"
#include "WiFiConnectionManager.h"
#include "web/WebUi.h"

#include <ESPmDNS.h>
#include <WiFi.h>

namespace
{
constexpr const char *MDNS_HOST = "cydmon";

const char *boardName()
{
#if defined(MODEL_JC4827W543R)
  return "JC4827W543R";
#elif defined(MODEL_JC2432W328C)
  return "JC2432W328C";
#elif defined(MODEL_JC2432W328R)
  return "JC2432W328R";
#elif defined(MODEL_2432S028R)
  return "ESP32-2432S028R";
#else
  return "CYD";
#endif
}

const char *envName()
{
#if defined(MODEL_JC4827W543R)
  return "jc4827w543r";
#elif defined(MODEL_JC2432W328C)
  return "jc2432w328c";
#elif defined(MODEL_JC2432W328R)
  return "jc2432w328r";
#elif defined(MODEL_2432S028R)
  return "2432s028r";
#else
  return "unknown";
#endif
}

const char *wifiStatusKey(WiFiConnStatus s)
{
  switch (s)
  {
  case WiFiConnStatus::Connected:
    return "connected";
  case WiFiConnStatus::Connecting:
    return "connecting";
  case WiFiConnStatus::Failed:
    return "failed";
  default:
    return "disconnected";
  }
}

const char *scanStatusKey(WiFiScanStatus s)
{
  switch (s)
  {
  case WiFiScanStatus::Scanning:
    return "scanning";
  case WiFiScanStatus::Ready:
    return "ready";
  case WiFiScanStatus::Failed:
    return "failed";
  default:
    return "idle";
  }
}
} // namespace

void WebServerManager::begin(SettingsStore *store,
                             WiFiConnectionManager *wifiMgr,
                             SensorManager *sensorMgr,
                             TemplateCode *displayCode,
                             SettingsChangedCallback onSettingsChanged)
{
  settings = store;
  wifi = wifiMgr;
  sensors = sensorMgr;
  display = displayCode;
  settingsChangedCb = std::move(onSettingsChanged);

  registerRoutes();
  server.begin();
  serverStarted = true;
  Serial.println("WebServer: HTTP on port 80 (setup hotspot or WiFi)");
}

bool WebServerManager::isActive() const
{
  if (!serverStarted || !wifi)
    return false;
  return wifi->isSetupPortalActive() || wifi->status() == WiFiConnStatus::Connected;
}

void WebServerManager::update()
{
  if (!serverStarted)
    return;

  server.handleClient();

  if (wifi && wifi->status() == WiFiConnStatus::Connected)
    ensureMdns();
  else
    stopMdns();
}

String WebServerManager::accessUrl() const
{
  if (wifi && wifi->isSetupPortalActive())
    return WiFiConnectionManager::SETUP_PORTAL_URL;

  if (!mdnsActive)
  {
    if (wifi && wifi->ipAddress().length())
      return String("http://") + wifi->ipAddress();
    return "";
  }
  String url = String("http://") + MDNS_HOST + ".local";
  if (wifi && wifi->ipAddress().length())
    url += " (" + wifi->ipAddress() + ")";
  return url;
}

void WebServerManager::ensureMdns()
{
  if (mdnsActive)
    return;
  if (!MDNS.begin(MDNS_HOST))
  {
    Serial.println("WebServer: mDNS failed");
    return;
  }
  MDNS.addService("http", "tcp", 80);
  mdnsActive = true;
  Serial.printf("WebServer: http://%s.local\n", MDNS_HOST);
}

void WebServerManager::stopMdns()
{
  if (!mdnsActive)
    return;
  MDNS.end();
  mdnsActive = false;
}

void WebServerManager::registerRoutes()
{
  server.on("/", HTTP_GET, [this]() { handleRoot(); });
  server.on("/api/status", HTTP_GET, [this]() { handleApiStatus(); });
  server.on("/api/settings", HTTP_GET, [this]() { handleApiSettings(); });
  server.on("/api/wifi/scan", HTTP_POST, [this]() { handleWifiScan(); });
  server.on("/api/wifi/networks", HTTP_GET, [this]() { handleWifiNetworks(); });
  server.on("/api/wifi/connect", HTTP_POST, [this]() { handleWifiConnect(); });
  server.on("/api/wifi/forget", HTTP_POST, [this]() { handleWifiForget(); });
  server.on("/api/setup/portal", HTTP_POST, [this]() { handleSetupPortal(); });
  server.on("/api/display/rotation", HTTP_POST, [this]() { handleDisplayRotation(); });
  server.on("/api/touch/cal", HTTP_POST, [this]() { handleTouchCalSave(); });
  server.on("/api/touch/reset", HTTP_POST, [this]() { handleTouchCalReset(); });
  server.on("/api/touch/sample", HTTP_GET, [this]() { handleApiTouchSample(); });
  server.onNotFound([this]() {
    if (wifi && wifi->isSetupPortalActive())
    {
      server.sendHeader("Location", String(WiFiConnectionManager::SETUP_PORTAL_URL), true);
      server.send(302, "text/plain", "");
      return;
    }
    server.send(404, "application/json", "{\"error\":\"not found\"}");
  });
}

void WebServerManager::handleRoot()
{
  server.send_P(200, "text/html", WEB_UI_HTML);
}

void WebServerManager::handleApiStatus()
{
  float temp = sensors ? sensors->lastTemperature() : NAN;
  float hum = sensors ? sensors->lastHumidity() : NAN;

  String wifiState = "disconnected";
  String ip;
  int rssi = 0;
  if (wifi)
  {
    wifiState = wifiStatusKey(wifi->status());
    ip = wifi->ipAddress();
    if (WiFi.status() == WL_CONNECTED)
      rssi = WiFi.RSSI();
  }

  String url = accessUrl();
  String savedSsid;
  String passDummy;
  if (settings)
    settings->loadWifi(savedSsid, passDummy);

  String rotLabel = display ? String(display->displayRotationLabel()) : "0°";

  String json = "{";
  json += "\"board\":\"" + jsonEscape(String(boardName())) + "\",";
  json += "\"env\":\"" + String(envName()) + "\",";
  json += "\"resolution\":\"" + String(SCREEN_WIDTH) + "x" + String(SCREEN_HEIGHT) + "\",";
  json += "\"screen_w\":" + String(SCREEN_WIDTH) + ",\"screen_h\":" + String(SCREEN_HEIGHT) + ",";
  json += "\"uptime_ms\":" + String(millis()) + ",";
  json += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"temperature\":";
  json += isnan(temp) ? "null" : String(temp, 2);
  json += ",\"humidity\":";
  json += isnan(hum) ? "null" : String(hum, 2);
  json += ",\"wifi\":{\"status\":\"" + wifiState + "\",\"ip\":\"" + jsonEscape(ip) + "\",";
  json += "\"rssi\":";
  json += (WiFi.status() == WL_CONNECTED) ? String(rssi) : "null";
  json += ",\"saved_ssid\":\"" + jsonEscape(savedSsid) + "\"},";
  json += "\"setup_portal\":";
  json += (wifi && wifi->isSetupPortalActive()) ? "true" : "false";
  json += ",\"setup_url\":\"" + String(WiFiConnectionManager::SETUP_PORTAL_URL) + "\"";
  json += ",\"ap_ssid\":\"" + String(WiFiConnectionManager::SETUP_AP_SSID) + "\"";
  json += ",\"web_url\":\"" + jsonEscape(url) + "\",";
  json += "\"touch\":{\"type\":\"";
  json += display ? jsonEscape(String(display->touchTypeName())) : "unknown";
  json += "\",\"resistive\":";
  json += (display && display->isResistiveTouch()) ? "true" : "false";
  json += "},";
  json += "\"display_rotation\":";
  json += display ? String(display->displayRotation()) : "0";
  json += ",\"display_rotation_label\":\"" + jsonEscape(rotLabel) + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void WebServerManager::handleApiSettings()
{
  String ssidOnly;
  String passDummy;
  if (settings)
    settings->loadWifi(ssidOnly, passDummy);

  TouchCalibration cal = settings ? settings->loadTouchCal() : TouchCalibration{};
  uint8_t rot = settings ? settings->loadDisplayRotation() : 0;
  if (display)
    rot = display->displayRotation();

  String json = "{";
  json += "\"wifi\":{\"saved_ssid\":\"" + jsonEscape(ssidOnly) + "\",";
  json += "\"auto_connect\":";
  json += (settings && settings->loadWifiAutoConnect()) ? "true" : "false";
  json += "},";
  json += "\"display_rotation\":" + String(rot) + ",";
  json += "\"touch\":{\"resistive\":";
  json += (display && display->isResistiveTouch()) ? "true" : "false";
  json += ",\"cal\":{\"xMin\":" + String(cal.xMin) + ",\"xMax\":" + String(cal.xMax);
  json += ",\"yMin\":" + String(cal.yMin) + ",\"yMax\":" + String(cal.yMax);
  json += ",\"saved\":";
  json += (settings && settings->hasTouchCal()) ? "true" : "false";
  json += "}}";

  server.send(200, "application/json", json);
}

void WebServerManager::handleWifiScan()
{
  if (wifi)
    wifi->startScan();
  server.send(200, "application/json", "{\"ok\":true,\"message\":\"Scan started\"}");
}

void WebServerManager::handleWifiNetworks()
{
  if (!wifi)
  {
    server.send(200, "application/json", "{\"status\":\"failed\",\"networks\":[]}");
    return;
  }

  String json = "{\"status\":\"" + String(scanStatusKey(wifi->scanStatus())) + "\",\"networks\":[";
  if (wifi->scanStatus() == WiFiScanStatus::Ready)
  {
    for (int i = 0; i < wifi->scanCount(); ++i)
    {
      if (i)
        json += ',';
      json += "{\"ssid\":\"" + jsonEscape(wifi->scanSsid(i)) + "\",\"rssi\":" + String(wifi->scanRssi(i)) + "}";
    }
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void WebServerManager::handleWifiConnect()
{
  if (!settings || !wifi)
  {
    server.send(500, "application/json", "{\"ok\":false,\"message\":\"Not ready\"}");
    return;
  }

  String body = readBody(server);
  String ssid = jsonStringField(body, "ssid");
  String pass = jsonStringField(body, "password");
  if (!ssid.length())
  {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"SSID required\"}");
    return;
  }

  settings->saveWifi(ssid, pass);
  settings->saveWifiAutoConnect(true);
  wifi->connect(ssid, pass);

  server.send(200, "application/json", "{\"ok\":true,\"message\":\"Connecting — refresh dashboard in a few seconds\"}");
  if (settingsChangedCb)
    settingsChangedCb();
}

void WebServerManager::handleWifiForget()
{
  if (settings)
  {
    settings->clearWifi();
    settings->saveWifiAutoConnect(false);
  }
  if (wifi)
    wifi->disconnect();

  server.send(200, "application/json", "{\"ok\":true,\"message\":\"Saved WiFi cleared\"}");
  if (settingsChangedCb)
    settingsChangedCb();
}

void WebServerManager::handleSetupPortal()
{
  if (!wifi)
  {
    server.send(500, "application/json", "{\"ok\":false,\"message\":\"Not ready\"}");
    return;
  }

  String body = readBody(server);
  bool enable = !(body.indexOf("\"enable\":false") >= 0 || body.indexOf("\"enable\": false") >= 0);

  if (enable)
    wifi->startSetupPortal();
  else
    wifi->stopSetupPortal();

  server.send(200, "application/json", enable
                                         ? "{\"ok\":true,\"message\":\"Setup hotspot started\"}"
                                         : "{\"ok\":true,\"message\":\"Setup hotspot stopped\"}");
  if (settingsChangedCb)
    settingsChangedCb();
}

void WebServerManager::handleDisplayRotation()
{
  if (!settings || !display)
  {
    server.send(500, "application/json", "{\"ok\":false,\"message\":\"Not ready\"}");
    return;
  }

  String body = readBody(server);
  int rot = jsonIntField(body, "rotation", -1);
  if (rot < 0 || rot > 3)
  {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"Rotation must be 0–3\"}");
    return;
  }

  display->setDisplayRotation((uint8_t)rot);
  settings->saveDisplayRotation((uint8_t)rot);

  String msg = String("Rotation set to ") + display->displayRotationLabel();
  server.send(200, "application/json", "{\"ok\":true,\"message\":\"" + jsonEscape(msg) + "\"}");
  if (settingsChangedCb)
    settingsChangedCb();
}

void WebServerManager::handleTouchCalSave()
{
  if (!settings || !display)
  {
    server.send(500, "application/json", "{\"ok\":false,\"message\":\"Not ready\"}");
    return;
  }
  if (!display->isResistiveTouch())
  {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"Capacitive touch — cal fixed in firmware\"}");
    return;
  }

  String body = readBody(server);
  TouchCalibration cal{};
  cal.xMin = jsonIntField(body, "xMin", settings->defaultTouchCal().xMin);
  cal.xMax = jsonIntField(body, "xMax", settings->defaultTouchCal().xMax);
  cal.yMin = jsonIntField(body, "yMin", settings->defaultTouchCal().yMin);
  cal.yMax = jsonIntField(body, "yMax", settings->defaultTouchCal().yMax);

  settings->saveTouchCal(cal);
  display->applyTouchCalibration(cal);

  server.send(200, "application/json", "{\"ok\":true,\"message\":\"Touch calibration saved\"}");
  if (settingsChangedCb)
    settingsChangedCb();
}

void WebServerManager::handleTouchCalReset()
{
  if (!settings || !display)
  {
    server.send(500, "application/json", "{\"ok\":false,\"message\":\"Not ready\"}");
    return;
  }
  if (!display->isResistiveTouch())
  {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"Capacitive touch — nothing to reset\"}");
    return;
  }

  settings->resetTouchCal();
  TouchCalibration cal = settings->defaultTouchCal();
  display->applyTouchCalibration(cal);

  server.send(200, "application/json", "{\"ok\":true,\"message\":\"Touch calibration reset to defaults\"}");
  if (settingsChangedCb)
    settingsChangedCb();
}

void WebServerManager::handleApiTouchSample()
{
  if (!display)
  {
    server.send(500, "application/json", "{\"pressed\":false}");
    return;
  }

  TouchSample sample{};
  bool ok = display->sampleTouch(sample);
  String json = "{";
  json += "\"pressed\":" + String(ok && sample.pressed ? "true" : "false");
  if (ok && sample.pressed)
  {
    json += ",\"rawX\":" + String(sample.rawX) + ",\"rawY\":" + String(sample.rawY);
    json += ",\"mappedX\":" + String(sample.mappedX) + ",\"mappedY\":" + String(sample.mappedY);
  }
  json += "}";
  server.send(200, "application/json", json);
}

String WebServerManager::jsonEscape(const String &s)
{
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); ++i)
  {
    char c = s[i];
    if (c == '\\' || c == '"')
    {
      out += '\\';
      out += c;
    }
    else if (c == '\n')
      out += "\\n";
    else
      out += c;
  }
  return out;
}

String WebServerManager::readBody(WebServer &srv)
{
  if (!srv.hasArg("plain"))
    return "";
  return srv.arg("plain");
}

String WebServerManager::jsonStringField(const String &body, const char *key)
{
  String needle = String("\"") + key + "\":\"";
  int start = body.indexOf(needle);
  if (start < 0)
    return "";
  start += needle.length();
  String out;
  for (int i = start; i < (int)body.length(); ++i)
  {
    char c = body[i];
    if (c == '\\' && i + 1 < (int)body.length())
    {
      out += body[i + 1];
      ++i;
      continue;
    }
    if (c == '"')
      break;
    out += c;
  }
  return out;
}

int WebServerManager::jsonIntField(const String &body, const char *key, int fallback)
{
  String needle = String("\"") + key + "\":";
  int start = body.indexOf(needle);
  if (start < 0)
    return fallback;
  start += needle.length();
  while (start < (int)body.length() && body[start] == ' ')
    ++start;
  return body.substring(start).toInt();
}
