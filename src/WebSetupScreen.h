#pragma once

#include <functional>
#include <lvgl.h>
#include <Arduino.h>

class WiFiConnectionManager;

class WebSetupScreen
{
public:
  using NavCallback = std::function<void()>;

  WebSetupScreen();
  ~WebSetupScreen();

  void init(WiFiConnectionManager *wifi);
  void show();
  void update();
  void setBackCallback(NavCallback cb);

  lv_obj_t *screen() const { return root; }

private:
  WiFiConnectionManager *wifi;
  NavCallback backCb;
  lv_obj_t *root;
  lv_obj_t *statusLabel;

  static void onBackClicked(lv_event_t *e);
  static void onStartPortalClicked(lv_event_t *e);
  static void onStopPortalClicked(lv_event_t *e);

  void createUi();
  void refreshStatus();
};
