#include "WebSetupScreen.h"
#include "UiLayout.h"
#include "WiFiConnectionManager.h"

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

WebSetupScreen::WebSetupScreen()
    : wifi(nullptr), root(nullptr), statusLabel(nullptr)
{
}

WebSetupScreen::~WebSetupScreen() = default;

void WebSetupScreen::init(WiFiConnectionManager *wifiMgr)
{
  wifi = wifiMgr;
  createUi();
}

void WebSetupScreen::createUi()
{
  root = lv_obj_create(NULL);
  lv_obj_set_size(root, SCREEN_W, SCREEN_H);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

  createHeaderBar(root, "Browser Setup", onBackClicked, this);

  lv_obj_t *scroll = lv_obj_create(root);
  lv_obj_set_size(scroll, SCREEN_W, SCREEN_H - HEADER_H);
  lv_obj_align(scroll, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(scroll, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_border_width(scroll, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(scroll, 10, LV_PART_MAIN);
  lv_obj_set_flex_flow(scroll, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(scroll, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *intro = lv_label_create(scroll);
  lv_label_set_text(intro,
                    "Two ways to configure WiFi — pick whichever is easier.");
  lv_obj_set_width(intro, SCREEN_W - 28);
  lv_label_set_long_mode(intro, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(intro, lv_color_hex(0xCCCCCC), LV_PART_MAIN);

  lv_obj_t *touchTitle = lv_label_create(scroll);
  lv_label_set_text(touchTitle, "On this screen:");
  lv_obj_set_style_text_color(touchTitle, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_pad_top(touchTitle, 8, LV_PART_MAIN);

  lv_obj_t *touchSteps = lv_label_create(scroll);
  lv_label_set_text(touchSteps, "Settings -> WiFi\nScan, pick network, enter password.");
  lv_obj_set_width(touchSteps, SCREEN_W - 28);
  lv_label_set_long_mode(touchSteps, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(touchSteps, lv_color_hex(0xAAAAAA), LV_PART_MAIN);

  lv_obj_t *setupTitle = lv_label_create(scroll);
  lv_label_set_text(setupTitle, "From phone or PC browser:");
  lv_obj_set_style_text_color(setupTitle, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_pad_top(setupTitle, 10, LV_PART_MAIN);

  lv_obj_t *setupSteps = lv_label_create(scroll);
  lv_label_set_text(setupSteps,
                    "1. Tap Start Setup Hotspot below\n"
                    "2. Join WiFi: CYD-Setup on your device\n"
                    "3. Open http://192.168.4.1\n"
                    "4. WiFi tab -> scan -> connect\n\n"
                    "Tip: many phones open the page automatically.");
  lv_obj_set_width(setupSteps, SCREEN_W - 28);
  lv_label_set_long_mode(setupSteps, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(setupSteps, lv_color_hex(0xAAAAAA), LV_PART_MAIN);

  lv_obj_t *lanTitle = lv_label_create(scroll);
  lv_label_set_text(lanTitle, "Already on your WiFi:");
  lv_obj_set_style_text_color(lanTitle, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_pad_top(lanTitle, 10, LV_PART_MAIN);

  lv_obj_t *lanSteps = lv_label_create(scroll);
  lv_label_set_text(lanSteps,
                    "On the same network open:\n"
                    "http://<device-ip>\n"
                    "(shown on dashboard)\n\n"
                    "Optional: http://cydmon.local");
  lv_obj_set_width(lanSteps, SCREEN_W - 28);
  lv_label_set_long_mode(lanSteps, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(lanSteps, lv_color_hex(0xAAAAAA), LV_PART_MAIN);

  statusLabel = lv_label_create(scroll);
  lv_obj_set_width(statusLabel, SCREEN_W - 28);
  lv_label_set_long_mode(statusLabel, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x888888), LV_PART_MAIN);
  lv_obj_set_style_pad_top(statusLabel, 8, LV_PART_MAIN);
  refreshStatus();

  createActionButton(scroll, "Start Setup Hotspot", SCREEN_W - 32, onStartPortalClicked, this);
  createActionButton(scroll, "Stop Setup Hotspot", SCREEN_W - 32, onStopPortalClicked, this);
}

void WebSetupScreen::show()
{
  refreshStatus();
  lv_scr_load(root);
}

void WebSetupScreen::update()
{
  refreshStatus();
}

void WebSetupScreen::setBackCallback(NavCallback cb)
{
  backCb = std::move(cb);
}

void WebSetupScreen::refreshStatus()
{
  if (!statusLabel || !wifi)
    return;

  if (wifi->isSetupPortalActive())
  {
    lv_label_set_text(statusLabel, "Hotspot active: join CYD-Setup, open 192.168.4.1");
    return;
  }

  if (wifi->status() == WiFiConnStatus::Connected)
  {
    String msg = String("Connected — browser: http://") + wifi->ipAddress();
    lv_label_set_text(statusLabel, msg.c_str());
    return;
  }

  lv_label_set_text(statusLabel, "Setup hotspot is off. Start it to configure from a browser.");
}

void WebSetupScreen::onBackClicked(lv_event_t *e)
{
  auto *self = static_cast<WebSetupScreen *>(lv_event_get_user_data(e));
  if (self && self->backCb)
    self->backCb();
}

void WebSetupScreen::onStartPortalClicked(lv_event_t *e)
{
  auto *self = static_cast<WebSetupScreen *>(lv_event_get_user_data(e));
  if (self && self->wifi)
  {
    self->wifi->startSetupPortal();
    self->refreshStatus();
  }
}

void WebSetupScreen::onStopPortalClicked(lv_event_t *e)
{
  auto *self = static_cast<WebSetupScreen *>(lv_event_get_user_data(e));
  if (self && self->wifi)
  {
    self->wifi->stopSetupPortal();
    self->refreshStatus();
  }
}
