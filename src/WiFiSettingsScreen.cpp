#include "WiFiSettingsScreen.h"
#include "SettingsStore.h"
#include "WiFiConnectionManager.h"
#include "UiLayout.h"
#include <stdio.h>

namespace
{
constexpr int SCREEN_W = UI_SCREEN_W;
constexpr int SCREEN_H = UI_SCREEN_H;
constexpr int HEADER_H = UI_HEADER_H;

lv_obj_t *createHeaderBar(lv_obj_t *parent, const char *title, lv_event_cb_t backCb, void *userData)
{
  lv_obj_t *header = lv_obj_create(parent);
  lv_obj_set_size(header, SCREEN_W, HEADER_H);
  lv_obj_set_style_bg_color(header, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
  lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(header, 4, LV_PART_MAIN);
  lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *backBtn = lv_btn_create(header);
  lv_obj_set_size(backBtn, 56, 28);
  lv_obj_align(backBtn, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_add_event_cb(backBtn, backCb, LV_EVENT_CLICKED, userData);
  lv_obj_t *backLbl = lv_label_create(backBtn);
  lv_label_set_text(backLbl, "Back");
  lv_obj_center(backLbl);

  lv_obj_t *titleLbl = lv_label_create(header);
  lv_label_set_text(titleLbl, title);
  lv_obj_set_style_text_color(titleLbl, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_align(titleLbl, LV_ALIGN_CENTER, 0, 0);

  return header;
}

lv_obj_t *createActionButton(lv_obj_t *parent, const char *text, lv_coord_t width, lv_event_cb_t cb, void *userData)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, width, 32);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, userData);
  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, text);
  lv_obj_center(lbl);
  return btn;
}
} // namespace

WiFiSettingsScreen::WiFiSettingsScreen()
    : settings(nullptr), wifi(nullptr), root(nullptr), statusLabel(nullptr),
      ssidDropdown(nullptr), passArea(nullptr), keyboard(nullptr)
{
}

WiFiSettingsScreen::~WiFiSettingsScreen() = default;

void WiFiSettingsScreen::init(SettingsStore *store, WiFiConnectionManager *wifiMgr)
{
  settings = store;
  wifi = wifiMgr;
  createUi();
  loadSavedCredentials();

  if (wifi)
  {
    wifi->onScanComplete([this]()
                         {
      if (wifi->scanStatus() == WiFiScanStatus::Failed)
      {
        setStatusText("Scan failed");
        return;
      }

      refreshSsidDropdown();
      if (ssidOptions.empty())
        setStatusText("No networks found");
      else
        setStatusText("Select a network"); });
  }
}

void WiFiSettingsScreen::createUi()
{
  root = lv_obj_create(NULL);
  lv_obj_set_size(root, SCREEN_W, SCREEN_H);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

  createHeaderBar(root, "WiFi", onBackClicked, this);

  lv_obj_t *content = lv_obj_create(root);
  lv_obj_set_size(content, SCREEN_W, SCREEN_H - HEADER_H);
  lv_obj_align(content, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(content, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_border_width(content, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(content, 8, LV_PART_MAIN);
  lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_add_flag(content, LV_OBJ_FLAG_SCROLLABLE);

  statusLabel = lv_label_create(content);
  lv_label_set_text(statusLabel, "Disconnected");
  lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
  lv_obj_set_width(statusLabel, SCREEN_W - 24);
  lv_label_set_long_mode(statusLabel, LV_LABEL_LONG_WRAP);

  lv_obj_t *altHint = lv_label_create(content);
  lv_label_set_text(altHint, "Or use Settings -> Browser Setup from a phone/PC.");
  lv_obj_set_style_text_color(altHint, lv_color_hex(0x666666), LV_PART_MAIN);
  lv_obj_set_style_text_font(altHint, &lv_font_montserrat_14, 0);
  lv_obj_set_width(altHint, SCREEN_W - 24);
  lv_label_set_long_mode(altHint, LV_LABEL_LONG_WRAP);

  lv_obj_t *scanRow = lv_obj_create(content);
  lv_obj_set_size(scanRow, SCREEN_W - 24, 36);
  lv_obj_set_style_bg_opa(scanRow, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(scanRow, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(scanRow, 0, LV_PART_MAIN);
  lv_obj_set_flex_flow(scanRow, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(scanRow, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  createActionButton(scanRow, "Scan", 64, onScanClicked, this);

  ssidDropdown = lv_dropdown_create(scanRow);
  lv_obj_set_width(ssidDropdown, SCREEN_W - 24 - 72);
  lv_dropdown_set_options(ssidDropdown, "Tap Scan");
  lv_obj_set_style_pad_ver(ssidDropdown, 6, LV_PART_MAIN);
  lv_obj_add_event_cb(ssidDropdown, onDropdownClicked, LV_EVENT_CLICKED, this);
  lv_obj_add_event_cb(ssidDropdown, onDropdownClicked, LV_EVENT_VALUE_CHANGED, this);

  passArea = lv_textarea_create(content);
  lv_textarea_set_one_line(passArea, true);
  lv_textarea_set_password_mode(passArea, true);
  lv_textarea_set_placeholder_text(passArea, "Password");
  lv_obj_set_width(passArea, SCREEN_W - 24);
  lv_obj_add_event_cb(passArea, onTextareaFocused, LV_EVENT_FOCUSED, this);
  lv_obj_add_event_cb(passArea, onTextareaDefocused, LV_EVENT_DEFOCUSED, this);

  createActionButton(content, "Connect", SCREEN_W - 24, onConnectClicked, this);

  lv_obj_t *btnRow = lv_obj_create(content);
  lv_obj_set_size(btnRow, SCREEN_W - 24, 36);
  lv_obj_set_style_bg_opa(btnRow, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(btnRow, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(btnRow, 0, LV_PART_MAIN);
  lv_obj_set_flex_flow(btnRow, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(btnRow, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  createActionButton(btnRow, "Forget WiFi", (SCREEN_W - 32) / 2, onForgetClicked, this);

  keyboard = lv_keyboard_create(root);
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_size(keyboard, SCREEN_W, UI_KEYBOARD_H);
  lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
  lv_obj_add_event_cb(keyboard, onKeyboardEvent, LV_EVENT_ALL, this);
}

void WiFiSettingsScreen::hideKeyboard()
{
  if (!keyboard)
    return;

  lv_keyboard_set_textarea(keyboard, NULL);
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  if (passArea)
    lv_obj_clear_state(passArea, LV_STATE_FOCUSED);
}

void WiFiSettingsScreen::loadSavedCredentials()
{
  if (!settings)
    return;

  String ssid;
  String pass;
  if (settings->loadWifi(ssid, pass))
  {
    ssidOptions.clear();
    ssidOptions.push_back(ssid);
    lv_dropdown_clear_options(ssidDropdown);
    lv_dropdown_add_option(ssidDropdown, ssid.c_str(), LV_DROPDOWN_POS_LAST);
    lv_dropdown_set_selected(ssidDropdown, 0);
    lv_textarea_set_text(passArea, pass.c_str());
  }
}

void WiFiSettingsScreen::refreshSsidDropdown()
{
  if (!wifi || !ssidDropdown)
    return;

  String savedSsid;
  String savedPass;
  if (settings)
    settings->loadWifi(savedSsid, savedPass);

  ssidOptions.clear();
  lv_dropdown_clear_options(ssidDropdown);

  const int count = wifi->scanCount();
  for (int i = 0; i < count; ++i)
  {
    String ssid = wifi->scanSsid(i);
    ssidOptions.push_back(ssid);

    char opt[64];
    snprintf(opt, sizeof(opt), "%s (%d)", ssid.c_str(), wifi->scanRssi(i));
    lv_dropdown_add_option(ssidDropdown, opt, LV_DROPDOWN_POS_LAST);
  }

  if (savedSsid.length())
  {
    bool found = false;
    for (const auto &option : ssidOptions)
    {
      if (option == savedSsid)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      ssidOptions.insert(ssidOptions.begin(), savedSsid);
      lv_dropdown_add_option(ssidDropdown, savedSsid.c_str(), 0);
    }
  }

  if (ssidOptions.empty())
  {
    lv_dropdown_add_option(ssidDropdown, "No networks found", LV_DROPDOWN_POS_LAST);
    lv_dropdown_set_selected(ssidDropdown, 0);
    return;
  }

  uint16_t selected = 0;
  if (savedSsid.length())
  {
    for (size_t i = 0; i < ssidOptions.size(); ++i)
    {
      if (ssidOptions[i] == savedSsid)
      {
        selected = (uint16_t)i;
        break;
      }
    }
  }
  lv_dropdown_set_selected(ssidDropdown, selected);
}

void WiFiSettingsScreen::startScan()
{
  if (!wifi)
    return;

  if (wifi->scanStatus() == WiFiScanStatus::Scanning)
    return;

  setStatusText("Scanning...");
  wifi->startScan();
}

String WiFiSettingsScreen::selectedSsid() const
{
  if (ssidOptions.empty() || !ssidDropdown)
    return "";

  uint16_t selected = lv_dropdown_get_selected(ssidDropdown);
  if (selected >= ssidOptions.size())
    return "";

  return ssidOptions[selected];
}

void WiFiSettingsScreen::show()
{
  hideKeyboard();
  if (wifi)
    setStatusText(wifi->statusText().c_str());

  lv_scr_load(root);
  startScan();
}

void WiFiSettingsScreen::update()
{
  if (!wifi || !statusLabel)
    return;

  if (wifi->scanStatus() == WiFiScanStatus::Scanning)
    setStatusText("Scanning...");
  else if (wifi->status() == WiFiConnStatus::Connecting)
    setStatusText(wifi->statusText().c_str());
  else if (wifi->status() == WiFiConnStatus::Connected)
    setStatusText(wifi->statusText().c_str());
}

void WiFiSettingsScreen::setStatusText(const char *text)
{
  if (statusLabel && text)
    lv_label_set_text(statusLabel, text);
}

void WiFiSettingsScreen::setBackCallback(NavCallback cb)
{
  backCb = cb;
}

void WiFiSettingsScreen::onBackClicked(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  if (self)
    self->hideKeyboard();
  if (self && self->backCb)
    self->backCb();
}

void WiFiSettingsScreen::onScanClicked(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  if (self)
  {
    self->hideKeyboard();
    self->startScan();
  }
}

void WiFiSettingsScreen::onDropdownClicked(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  if (self)
    self->hideKeyboard();
}

void WiFiSettingsScreen::onConnectClicked(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  if (!self || !self->settings || !self->wifi)
    return;

  String ssid = self->selectedSsid();
  if (!ssid.length())
  {
    self->setStatusText("Scan and select a network");
    return;
  }

  const char *pass = lv_textarea_get_text(self->passArea);
  self->settings->saveWifi(ssid, pass ? pass : "");
  self->settings->saveWifiAutoConnect(true);
  self->wifi->connect(ssid, pass ? pass : "");
  self->setStatusText("Connecting...");
  self->hideKeyboard();
}

void WiFiSettingsScreen::onForgetClicked(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  if (!self || !self->settings)
    return;

  self->hideKeyboard();
  self->settings->clearWifi();
  self->settings->saveWifiAutoConnect(false);
  if (self->wifi)
    self->wifi->disconnect();

  self->ssidOptions.clear();
  lv_dropdown_clear_options(self->ssidDropdown);
  lv_dropdown_add_option(self->ssidDropdown, "Tap Scan", LV_DROPDOWN_POS_LAST);
  lv_dropdown_set_selected(self->ssidDropdown, 0);
  lv_textarea_set_text(self->passArea, "");
  self->setStatusText("Saved WiFi cleared");
}

void WiFiSettingsScreen::onKeyboardEvent(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  if (!self)
    return;

  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL)
    self->hideKeyboard();
}

void WiFiSettingsScreen::onTextareaFocused(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  lv_obj_t *ta = static_cast<lv_obj_t *>(lv_event_get_target(e));
  if (!self || !self->keyboard)
    return;

  lv_keyboard_set_textarea(self->keyboard, ta);
  lv_obj_clear_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);
}

void WiFiSettingsScreen::onTextareaDefocused(lv_event_t *e)
{
  auto *self = static_cast<WiFiSettingsScreen *>(lv_event_get_user_data(e));
  if (self)
    self->hideKeyboard();
}
