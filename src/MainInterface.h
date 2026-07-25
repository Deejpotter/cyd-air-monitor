// MainInterface.h
/**
 * MainInterface.h
 * Last Updated: July 2026
 * Author: Daniel Potter
 *
 * Dashboard UI with navigation to settings screens (WiFi, touch test).
 */

#ifndef MAIN_INTERFACE_H
#define MAIN_INTERFACE_H

#include <functional>
#include <lvgl.h>

class SettingsStore;
class TemplateCode;
class WiFiConnectionManager;
class WiFiSettingsScreen;
class TouchConfigScreen;

class MainInterface
{
public:
  MainInterface();
  ~MainInterface();

  void init(SettingsStore *store, TemplateCode *display, WiFiConnectionManager *wifi);
  void update();

  void setTemperature(float tempC);
  void setHumidity(float humidity);

private:
  enum class ActiveScreen
  {
    Dashboard,
    SettingsMenu,
    WiFi,
    Touch
  };

  SettingsStore *settings;
  TemplateCode *display;
  WiFiConnectionManager *wifi;
  ActiveScreen activeScreen;

  lv_obj_t *dashboardScreen;
  lv_obj_t *settingsMenuScreen;
  lv_obj_t *headerContainer;
  lv_obj_t *headerLabel;
  lv_obj_t *tempLabel;
  lv_obj_t *humidityLabel;
  lv_obj_t *wifiStatusLabel;

  WiFiSettingsScreen *wifiScreen;
  TouchConfigScreen *touchScreen;

  void createDashboard();
  void createSettingsMenu();
  void createHeader(lv_obj_t *parent, const char *title, bool showSettingsBtn);
  void showScreen(ActiveScreen screen);

  static void onSettingsBtnClicked(lv_event_t *e);
  static void onMenuWifiClicked(lv_event_t *e);
  static void onMenuTouchClicked(lv_event_t *e);
  static void onMenuBackClicked(lv_event_t *e);
};

#endif // MAIN_INTERFACE_H
