#include "WiFiConnectionManager.h"
#include "SettingsStore.h"
#include <DNSServer.h>
#include <WiFi.h>

namespace
{
constexpr unsigned long CONNECT_TIMEOUT_MS = 20000;
} // namespace

void WiFiConnectionManager::begin(SettingsStore *store)
{
  settings = store;
  connStatus = WiFiConnStatus::Disconnected;
  scanState = WiFiScanStatus::Idle;
  connectStartedMs = 0;
  pendingConnect = false;
  pendingScan = false;
  setupPortalActive = false;
  dnsServer = new DNSServer();

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  if (settings && settings->loadWifiAutoConnect() && settings->hasWifi())
  {
    connectSaved();
  }
}

void WiFiConnectionManager::update()
{
  if (setupPortalActive && dnsServer)
    dnsServer->processNextRequest();

  if (pendingScan)
  {
    pendingScan = false;
    scanSsids.clear();
    scanRssis.clear();
    WiFi.scanDelete();
    WiFi.scanNetworks(true);
    scanState = WiFiScanStatus::Scanning;
  }

  if (scanState == WiFiScanStatus::Scanning)
  {
    int n = WiFi.scanComplete();
    if (n == -1)
      return;
    if (n == -2)
    {
      scanState = WiFiScanStatus::Failed;
      if (scanCb)
        scanCb();
      return;
    }
    finishScan(n);
  }

  if (pendingConnect)
  {
    pendingConnect = false;
    if (setupPortalActive)
      stopSetupPortal();
    WiFi.disconnect(true);
    delay(100);
    WiFi.begin(pendingSsid.c_str(), pendingPass.c_str());
    connectStartedMs = millis();
    setStatus(WiFiConnStatus::Connecting, pendingSsid);
  }

  if (connStatus == WiFiConnStatus::Connecting)
  {
    wl_status_t wl = WiFi.status();
    if (wl == WL_CONNECTED)
    {
      if (setupPortalActive)
        stopSetupPortal();
      setStatus(WiFiConnStatus::Connected, WiFi.localIP().toString());
      return;
    }

    if ((millis() - connectStartedMs) > CONNECT_TIMEOUT_MS)
    {
      setStatus(WiFiConnStatus::Failed, "Timeout");
      return;
    }

    if (wl == WL_CONNECT_FAILED || wl == WL_NO_SSID_AVAIL)
    {
      setStatus(WiFiConnStatus::Failed, "Connect failed");
    }
    return;
  }

  if (connStatus == WiFiConnStatus::Connected && WiFi.status() != WL_CONNECTED)
  {
    setStatus(WiFiConnStatus::Disconnected);
    return;
  }

  if (connStatus == WiFiConnStatus::Disconnected && WiFi.status() == WL_CONNECTED)
    setStatus(WiFiConnStatus::Connected, WiFi.localIP().toString());
}

void WiFiConnectionManager::startSetupPortal()
{
  if (setupPortalActive)
    return;

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(SETUP_AP_SSID);
  if (dnsServer)
    dnsServer->start(53, "*", IPAddress(192, 168, 4, 1));

  setupPortalActive = true;
  setStatus(WiFiConnStatus::Disconnected, "Browser setup active");
  Serial.printf("Setup portal: join \"%s\" then open %s\n", SETUP_AP_SSID, SETUP_PORTAL_URL);
}

void WiFiConnectionManager::stopSetupPortal()
{
  if (!setupPortalActive)
    return;

  if (dnsServer)
    dnsServer->stop();
  WiFi.softAPdisconnect(true);
  setupPortalActive = false;
  WiFi.mode(WIFI_STA);
  Serial.println("Setup portal stopped");
}

void WiFiConnectionManager::startScan()
{
  if (scanState == WiFiScanStatus::Scanning)
    return;
  pendingScan = true;
}

void WiFiConnectionManager::finishScan(int networkCount)
{
  for (int i = 0; i < networkCount; ++i)
  {
    String ssid = WiFi.SSID(i);
    if (!ssid.length())
      continue;

    bool duplicate = false;
    for (const auto &existing : scanSsids)
    {
      if (existing == ssid)
      {
        duplicate = true;
        break;
      }
    }
    if (duplicate)
      continue;

    scanSsids.push_back(ssid);
    scanRssis.push_back(WiFi.RSSI(i));
  }

  WiFi.scanDelete();
  scanState = WiFiScanStatus::Ready;
  if (scanCb)
    scanCb();
}

String WiFiConnectionManager::scanSsid(int index) const
{
  if (index < 0 || index >= (int)scanSsids.size())
    return "";
  return scanSsids[index];
}

int WiFiConnectionManager::scanRssi(int index) const
{
  if (index < 0 || index >= (int)scanRssis.size())
    return 0;
  return scanRssis[index];
}

void WiFiConnectionManager::connect(const String &ssid, const String &pass)
{
  pendingSsid = ssid;
  pendingPass = pass;
  pendingConnect = true;
}

void WiFiConnectionManager::connectSaved()
{
  if (!settings)
    return;

  String ssid;
  String pass;
  if (!settings->loadWifi(ssid, pass))
    return;

  connect(ssid, pass);
}

void WiFiConnectionManager::disconnect()
{
  WiFi.disconnect(true);
  setStatus(WiFiConnStatus::Disconnected);
}

String WiFiConnectionManager::statusText() const
{
  if (setupPortalActive && connStatus != WiFiConnStatus::Connected)
    return "Setup: join CYD-Setup";

  switch (connStatus)
  {
  case WiFiConnStatus::Disconnected:
    return "Disconnected";
  case WiFiConnStatus::Connecting:
    return "Connecting...";
  case WiFiConnStatus::Connected:
    return "Connected: " + ipAddress();
  case WiFiConnStatus::Failed:
    return "Failed";
  }
  return "Unknown";
}

String WiFiConnectionManager::ipAddress() const
{
  if (setupPortalActive && WiFi.status() != WL_CONNECTED)
    return "192.168.4.1";
  if (WiFi.status() == WL_CONNECTED)
    return WiFi.localIP().toString();
  return "";
}

void WiFiConnectionManager::onStatusChange(StatusCallback cb)
{
  statusCb = cb;
}

void WiFiConnectionManager::onScanComplete(ScanCallback cb)
{
  scanCb = cb;
}

void WiFiConnectionManager::setStatus(WiFiConnStatus next, const String &detail)
{
  connStatus = next;
  if (statusCb)
    statusCb(next, detail.length() ? detail : statusText());
}
