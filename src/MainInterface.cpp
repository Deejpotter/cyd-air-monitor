#include "MainInterface.h"
#include "SettingsStore.h"
#include "TemplateCode.h"
#include "TouchConfigScreen.h"
#include "WebSetupScreen.h"
#include "WiFiConnectionManager.h"
#include "WiFiSettingsScreen.h"
#include <stdio.h>

#include "DisplayConfig.h"
#include "UiLayout.h"

namespace
{
constexpr int SCREEN_W = UI_SCREEN_W;
constexpr int SCREEN_H = UI_SCREEN_H;
constexpr int HEADER_H = UI_HEADER_H;
} // namespace

MainInterface::MainInterface()
    : settings(nullptr), display(nullptr), wifi(nullptr),
      activeScreen(ActiveScreen::Dashboard), dashboardScreen(nullptr),
      settingsMenuScreen(nullptr), headerContainer(nullptr), headerLabel(nullptr),
      tempLabel(nullptr), humidityLabel(nullptr), wifiStatusLabel(nullptr),
      webHintLabel(nullptr),
      rotationLabel(nullptr),
      wifiScreen(nullptr), webSetupScreen(nullptr), touchScreen(nullptr)
{
}

MainInterface::~MainInterface()
{
  delete wifiScreen;
  delete webSetupScreen;
  delete touchScreen;
}

void MainInterface::init(SettingsStore *store, TemplateCode *displayCode, WiFiConnectionManager *wifiMgr)
{
  settings = store;
  display = displayCode;
  wifi = wifiMgr;

  if (display && settings)
  {
    display->applyTouchCalibration(settings->loadTouchCal());
    display->setDisplayRotation(settings->loadDisplayRotation());
  }

  wifiScreen = new WiFiSettingsScreen();
  webSetupScreen = new WebSetupScreen();
  touchScreen = new TouchConfigScreen();

  wifiScreen->init(settings, wifi);
  webSetupScreen->init(wifi);
  touchScreen->init(settings, display);

  wifiScreen->setBackCallback([this]() { showScreen(ActiveScreen::SettingsMenu); });
  webSetupScreen->setBackCallback([this]() { showScreen(ActiveScreen::SettingsMenu); });
  touchScreen->setBackCallback([this]() { showScreen(ActiveScreen::SettingsMenu); });

  if (wifi)
  {
    wifi->onStatusChange([this](WiFiConnStatus, const String &detail)
                         {
      if (wifiStatusLabel)
      {
        const char *text = detail.length() ? detail.c_str() : wifi->statusText().c_str();
        lv_label_set_text(wifiStatusLabel, text);
      }
      if (activeScreen == ActiveScreen::WiFi)
        wifiScreen->setStatusText(wifi->statusText().c_str()); });
  }

  createDashboard();
  createSettingsMenu();
  showScreen(ActiveScreen::Dashboard);
}

void MainInterface::createHeader(lv_obj_t *parent, const char *title, bool showSettingsBtn)
{
  lv_obj_t *header = lv_obj_create(parent);
  lv_obj_set_size(header, SCREEN_W, HEADER_H);
  lv_obj_set_style_bg_color(header, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
  lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(header, 4, LV_PART_MAIN);
  lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

  if (showSettingsBtn)
  {
    lv_obj_t *settingsBtn = lv_btn_create(header);
    lv_obj_set_size(settingsBtn, 56, 28);
    lv_obj_align(settingsBtn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(settingsBtn, onSettingsBtnClicked, LV_EVENT_CLICKED, this);
    lv_obj_t *settingsLbl = lv_label_create(settingsBtn);
    lv_label_set_text(settingsLbl, LV_SYMBOL_SETTINGS);
    lv_obj_center(settingsLbl);
  }

  lv_obj_t *titleLbl = lv_label_create(header);
  lv_label_set_text(titleLbl, title);
  lv_obj_set_style_text_color(titleLbl, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_align(titleLbl, LV_ALIGN_CENTER, 0, 0);
}

void MainInterface::createDashboard()
{
  dashboardScreen = lv_obj_create(NULL);
  lv_obj_set_size(dashboardScreen, SCREEN_W, SCREEN_H);
  lv_obj_set_style_bg_color(dashboardScreen, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_pad_all(dashboardScreen, 0, LV_PART_MAIN);
  lv_obj_clear_flag(dashboardScreen, LV_OBJ_FLAG_SCROLLABLE);

  headerContainer = lv_obj_create(dashboardScreen);
  lv_obj_set_size(headerContainer, SCREEN_W, HEADER_H);
  lv_obj_set_style_bg_color(headerContainer, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
  lv_obj_set_style_border_width(headerContainer, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(headerContainer, 4, LV_PART_MAIN);
  lv_obj_clear_flag(headerContainer, LV_OBJ_FLAG_SCROLLABLE);

  headerLabel = lv_label_create(headerContainer);
  lv_label_set_text(headerLabel, "Air Monitor");
  lv_obj_set_style_text_color(headerLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_align(headerLabel, LV_ALIGN_LEFT_MID, 4, 0);

  lv_obj_t *settingsBtn = lv_btn_create(headerContainer);
  lv_obj_set_size(settingsBtn, 56, 28);
  lv_obj_align(settingsBtn, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_add_event_cb(settingsBtn, onSettingsBtnClicked, LV_EVENT_CLICKED, this);
  lv_obj_t *settingsLbl = lv_label_create(settingsBtn);
  lv_label_set_text(settingsLbl, LV_SYMBOL_SETTINGS);
  lv_obj_center(settingsLbl);

  tempLabel = lv_label_create(dashboardScreen);
  lv_obj_set_style_text_font(tempLabel, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(tempLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_label_set_text(tempLabel, "Temperature:\n--.-°C");
  lv_obj_align(tempLabel, LV_ALIGN_TOP_MID, 0, 56);

  humidityLabel = lv_label_create(dashboardScreen);
  lv_obj_set_style_text_font(humidityLabel, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(humidityLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_label_set_text(humidityLabel, "Humidity:\n--.-%");
  lv_obj_align_to(humidityLabel, tempLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

  wifiStatusLabel = lv_label_create(dashboardScreen);
  lv_obj_set_style_text_color(wifiStatusLabel, lv_color_hex(0x666666), LV_PART_MAIN);
  lv_label_set_text(wifiStatusLabel, wifi ? wifi->statusText().c_str() : "WiFi: --");
  lv_obj_align(wifiStatusLabel, LV_ALIGN_BOTTOM_MID, 0, -24);

  webHintLabel = lv_label_create(dashboardScreen);
  lv_obj_set_style_text_color(webHintLabel, lv_color_hex(0x888888), LV_PART_MAIN);
  lv_obj_set_style_text_font(webHintLabel, &lv_font_montserrat_14, 0);
  lv_label_set_text(webHintLabel, "");
  lv_obj_align(webHintLabel, LV_ALIGN_BOTTOM_MID, 0, -6);
}

void MainInterface::createSettingsMenu()
{
  settingsMenuScreen = lv_obj_create(NULL);
  lv_obj_set_size(settingsMenuScreen, SCREEN_W, SCREEN_H);
  lv_obj_set_style_bg_color(settingsMenuScreen, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_pad_all(settingsMenuScreen, 0, LV_PART_MAIN);
  lv_obj_clear_flag(settingsMenuScreen, LV_OBJ_FLAG_SCROLLABLE);

  createHeader(settingsMenuScreen, "Settings", false);

  lv_obj_t *content = lv_obj_create(settingsMenuScreen);
  lv_obj_set_size(content, SCREEN_W, SCREEN_H - HEADER_H);
  lv_obj_align(content, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(content, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_border_width(content, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(content, 12, LV_PART_MAIN);
  lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lv_obj_t *wifiBtn = lv_btn_create(content);
  lv_obj_set_size(wifiBtn, SCREEN_W - 32, 40);
  lv_obj_add_event_cb(wifiBtn, onMenuWifiClicked, LV_EVENT_CLICKED, this);
  lv_obj_t *wifiLbl = lv_label_create(wifiBtn);
  lv_label_set_text(wifiLbl, LV_SYMBOL_WIFI "  WiFi");
  lv_obj_center(wifiLbl);

  lv_obj_t *webBtn = lv_btn_create(content);
  lv_obj_set_size(webBtn, SCREEN_W - 32, 40);
  lv_obj_add_event_cb(webBtn, onMenuWebSetupClicked, LV_EVENT_CLICKED, this);
  lv_obj_t *webLbl = lv_label_create(webBtn);
  lv_label_set_text(webLbl, LV_SYMBOL_HOME "  Browser Setup");
  lv_obj_center(webLbl);

  lv_obj_t *touchBtn = lv_btn_create(content);
  lv_obj_set_size(touchBtn, SCREEN_W - 32, 40);
  lv_obj_add_event_cb(touchBtn, onMenuTouchClicked, LV_EVENT_CLICKED, this);
  lv_obj_t *touchLbl = lv_label_create(touchBtn);
  lv_label_set_text(touchLbl, LV_SYMBOL_EDIT "  Touch Test");
  lv_obj_center(touchLbl);

  lv_obj_t *rotateBtn = lv_btn_create(content);
  lv_obj_set_size(rotateBtn, SCREEN_W - 32, 40);
  lv_obj_add_event_cb(rotateBtn, onMenuRotateClicked, LV_EVENT_CLICKED, this);
  lv_obj_t *rotateLbl = lv_label_create(rotateBtn);
  lv_label_set_text(rotateLbl, LV_SYMBOL_REFRESH "  Rotate Display");
  lv_obj_center(rotateLbl);

  rotationLabel = lv_label_create(content);
  lv_obj_set_style_text_color(rotationLabel, lv_color_hex(0x888888), LV_PART_MAIN);
  updateRotationLabel();

  lv_obj_t *backBtn = lv_btn_create(content);
  lv_obj_set_size(backBtn, SCREEN_W - 32, 36);
  lv_obj_add_event_cb(backBtn, onMenuBackClicked, LV_EVENT_CLICKED, this);
  lv_obj_t *backLbl = lv_label_create(backBtn);
  lv_label_set_text(backLbl, "Back to Dashboard");
  lv_obj_center(backLbl);
}

void MainInterface::showScreen(ActiveScreen screen)
{
  activeScreen = screen;
  switch (screen)
  {
  case ActiveScreen::Dashboard:
    lv_scr_load(dashboardScreen);
    break;
  case ActiveScreen::SettingsMenu:
    lv_scr_load(settingsMenuScreen);
    break;
  case ActiveScreen::WiFi:
    wifiScreen->show();
    break;
  case ActiveScreen::WebSetup:
    webSetupScreen->show();
    break;
  case ActiveScreen::Touch:
    touchScreen->show();
    break;
  }
}

void MainInterface::refreshFromSettings()
{
  updateRotationLabel();
}

void MainInterface::setWebAccessHint(const char *hint)
{
  if (!webHintLabel)
    return;
  if (hint && hint[0])
    lv_label_set_text(webHintLabel, hint);
  else
    lv_label_set_text(webHintLabel, "");
}

void MainInterface::update()
{
  if (activeScreen == ActiveScreen::WiFi)
    wifiScreen->update();
  else if (activeScreen == ActiveScreen::WebSetup)
    webSetupScreen->update();
  else if (activeScreen == ActiveScreen::Touch)
    touchScreen->update();

  if (wifiStatusLabel && wifi && activeScreen == ActiveScreen::Dashboard)
    lv_label_set_text(wifiStatusLabel, wifi->statusText().c_str());
}

void MainInterface::setTemperature(float tempC)
{
  if (!tempLabel)
    return;
  char buf[32];
  snprintf(buf, sizeof(buf), "Temperature:\n%.1f°C", tempC);
  lv_label_set_text(tempLabel, buf);
}

void MainInterface::setHumidity(float humidity)
{
  if (!humidityLabel)
    return;
  char buf[32];
  snprintf(buf, sizeof(buf), "Humidity:\n%.1f%%", humidity);
  lv_label_set_text(humidityLabel, buf);
}

void MainInterface::onSettingsBtnClicked(lv_event_t *e)
{
  auto *self = static_cast<MainInterface *>(lv_event_get_user_data(e));
  if (self)
    self->showScreen(ActiveScreen::SettingsMenu);
}

void MainInterface::onMenuWifiClicked(lv_event_t *e)
{
  auto *self = static_cast<MainInterface *>(lv_event_get_user_data(e));
  if (self)
    self->showScreen(ActiveScreen::WiFi);
}

void MainInterface::onMenuWebSetupClicked(lv_event_t *e)
{
  auto *self = static_cast<MainInterface *>(lv_event_get_user_data(e));
  if (self)
    self->showScreen(ActiveScreen::WebSetup);
}

void MainInterface::onMenuTouchClicked(lv_event_t *e)
{
  auto *self = static_cast<MainInterface *>(lv_event_get_user_data(e));
  if (self)
    self->showScreen(ActiveScreen::Touch);
}

void MainInterface::updateRotationLabel()
{
  if (!rotationLabel || !display)
    return;

  char buf[48];
  snprintf(buf, sizeof(buf), "Rotation: %s", display->displayRotationLabel());
  lv_label_set_text(rotationLabel, buf);
}

void MainInterface::onMenuRotateClicked(lv_event_t *e)
{
  auto *self = static_cast<MainInterface *>(lv_event_get_user_data(e));
  if (!self || !self->display || !self->settings)
    return;

  uint8_t next = (self->display->displayRotation() + 1) & 0x3;
  self->display->setDisplayRotation(next);
  self->settings->saveDisplayRotation(next);
  self->updateRotationLabel();
}

void MainInterface::onMenuBackClicked(lv_event_t *e)
{
  auto *self = static_cast<MainInterface *>(lv_event_get_user_data(e));
  if (self)
    self->showScreen(ActiveScreen::Dashboard);
}
