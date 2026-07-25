#pragma once

#include <functional>
#include <lvgl.h>
#include <vector>
#include <Arduino.h>

class SettingsStore;
class WiFiConnectionManager;

class WiFiSettingsScreen
{
public:
  using NavCallback = std::function<void()>;

  WiFiSettingsScreen();
  ~WiFiSettingsScreen();

  void init(SettingsStore *store, WiFiConnectionManager *wifi);
  void show();
  void update();
  void setBackCallback(NavCallback cb);
  void setStatusText(const char *text);

  lv_obj_t *screen() const { return root; }

private:
  SettingsStore *settings;
  WiFiConnectionManager *wifi;
  NavCallback backCb;
  std::vector<String> ssidOptions;
  lv_obj_t *root;
  lv_obj_t *statusLabel;
  lv_obj_t *ssidDropdown;
  lv_obj_t *passArea;
  lv_obj_t *keyboard;

  static void onBackClicked(lv_event_t *e);
  static void onConnectClicked(lv_event_t *e);
  static void onForgetClicked(lv_event_t *e);
  static void onScanClicked(lv_event_t *e);
  static void onDropdownClicked(lv_event_t *e);
  static void onKeyboardEvent(lv_event_t *e);
  static void onTextareaFocused(lv_event_t *e);
  static void onTextareaDefocused(lv_event_t *e);

  void createUi();
  void hideKeyboard();
  void loadSavedCredentials();
  void refreshSsidDropdown();
  void startScan();
  String selectedSsid() const;
};
