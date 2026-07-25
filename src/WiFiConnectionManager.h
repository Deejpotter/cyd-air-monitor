#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>

class DNSServer;
class SettingsStore;

enum class WiFiConnStatus
{
  Disconnected,
  Connecting,
  Connected,
  Failed
};

enum class WiFiScanStatus
{
  Idle,
  Scanning,
  Ready,
  Failed
};

class WiFiConnectionManager
{
public:
  using StatusCallback = std::function<void(WiFiConnStatus status, const String &detail)>;
  using ScanCallback = std::function<void()>;

  static constexpr const char *SETUP_AP_SSID = "CYD-Setup";
  static constexpr const char *SETUP_PORTAL_URL = "http://192.168.4.1";

  void begin(SettingsStore *store);
  void update();

  void connect(const String &ssid, const String &pass);
  void connectSaved();
  void disconnect();

  void startScan();
  WiFiScanStatus scanStatus() const { return scanState; }
  int scanCount() const { return (int)scanSsids.size(); }
  String scanSsid(int index) const;
  int scanRssi(int index) const;

  WiFiConnStatus status() const { return connStatus; }
  String statusText() const;
  String ipAddress() const;

  /** Soft AP + captive DNS for browser-based WiFi setup at 192.168.4.1 */
  bool isSetupPortalActive() const { return setupPortalActive; }
  void startSetupPortal();
  void stopSetupPortal();

  void onStatusChange(StatusCallback cb);
  void onScanComplete(ScanCallback cb);

private:
  SettingsStore *settings;
  StatusCallback statusCb;
  ScanCallback scanCb;
  WiFiConnStatus connStatus;
  WiFiScanStatus scanState;
  std::vector<String> scanSsids;
  std::vector<int> scanRssis;
  unsigned long connectStartedMs;
  String pendingSsid;
  String pendingPass;
  bool pendingConnect;
  bool pendingScan;
  bool setupPortalActive;
  DNSServer *dnsServer;

  void setStatus(WiFiConnStatus next, const String &detail = "");
  void finishScan(int networkCount);
};
