#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>

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

  void setStatus(WiFiConnStatus next, const String &detail = "");
  void finishScan(int networkCount);
};
