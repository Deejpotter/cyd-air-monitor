#include "TouchConfigScreen.h"
#include "SettingsStore.h"
#include "TemplateCode.h"
#include <stdio.h>

namespace
{
constexpr int SCREEN_W = 320;
constexpr int SCREEN_H = 240;
constexpr int HEADER_H = 40;

enum CalField
{
  CAL_X_MIN = 0,
  CAL_X_MAX = 1,
  CAL_Y_MIN = 2,
  CAL_Y_MAX = 3
};

struct CalButtonDef
{
  const char *label;
  int field;
  int delta;
};

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

lv_obj_t *createSmallButton(lv_obj_t *parent, const char *text, lv_event_cb_t cb, void *userData)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, 36, 24);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, userData);
  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, text);
  lv_obj_center(lbl);
  return btn;
}
} // namespace

TouchConfigScreen::TouchConfigScreen()
    : settings(nullptr), display(nullptr), root(nullptr), typeLabel(nullptr),
      stateLabel(nullptr), rawLabel(nullptr), mappedLabel(nullptr), calLabel(nullptr),
      touchPad(nullptr), touchDot(nullptr), hintLabel(nullptr)
{
  for (auto &adj : calAdjustments)
    adj = {nullptr, 0, 0};
}

TouchConfigScreen::~TouchConfigScreen() = default;

void TouchConfigScreen::init(SettingsStore *store, TemplateCode *displayCode)
{
  settings = store;
  display = displayCode;
  if (settings)
    cal = settings->loadTouchCal();
  createUi();
  applyCalibration();
}

void TouchConfigScreen::createUi()
{
  root = lv_obj_create(NULL);
  lv_obj_set_size(root, SCREEN_W, SCREEN_H);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

  createHeaderBar(root, "Touch Test", onBackClicked, this);

  lv_obj_t *content = lv_obj_create(root);
  lv_obj_set_size(content, SCREEN_W, SCREEN_H - HEADER_H);
  lv_obj_align(content, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(content, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_border_width(content, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(content, 6, LV_PART_MAIN);
  lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_add_flag(content, LV_OBJ_FLAG_SCROLLABLE);

  typeLabel = lv_label_create(content);
  lv_label_set_text(typeLabel, display ? display->touchTypeName() : "Touch");
  lv_obj_set_style_text_color(typeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

  stateLabel = lv_label_create(content);
  lv_label_set_text(stateLabel, "State: released");
  lv_obj_set_style_text_color(stateLabel, lv_color_hex(0xAAAAAA), LV_PART_MAIN);

  rawLabel = lv_label_create(content);
  lv_label_set_text(rawLabel, "Raw: --, --");
  lv_obj_set_style_text_color(rawLabel, lv_color_hex(0xAAAAAA), LV_PART_MAIN);

  mappedLabel = lv_label_create(content);
  lv_label_set_text(mappedLabel, "Mapped: --, --");
  lv_obj_set_style_text_color(mappedLabel, lv_color_hex(0xAAAAAA), LV_PART_MAIN);

  touchPad = lv_obj_create(content);
  lv_obj_set_size(touchPad, SCREEN_W - 24, 72);
  lv_obj_set_style_bg_color(touchPad, lv_color_hex(0x101820), LV_PART_MAIN);
  lv_obj_set_style_border_color(touchPad, lv_color_hex(0x336699), LV_PART_MAIN);
  lv_obj_set_style_border_width(touchPad, 1, LV_PART_MAIN);
  lv_obj_clear_flag(touchPad, LV_OBJ_FLAG_SCROLLABLE);

  touchDot = lv_obj_create(touchPad);
  lv_obj_set_size(touchDot, 10, 10);
  lv_obj_set_style_radius(touchDot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_bg_color(touchDot, lv_color_hex(0x00FF88), LV_PART_MAIN);
  lv_obj_set_style_border_width(touchDot, 0, LV_PART_MAIN);
  lv_obj_add_flag(touchDot, LV_OBJ_FLAG_HIDDEN);

  if (display && display->isResistiveTouch())
  {
    hintLabel = lv_label_create(content);
    lv_label_set_text(hintLabel,
                      "Resistive: map raw ADC to screen. "
                      "If axes flip, tune platformio TOUCH_MIRROR_X/Y.");
    lv_obj_set_style_text_color(hintLabel, lv_color_hex(0x777777), LV_PART_MAIN);
    lv_obj_set_width(hintLabel, SCREEN_W - 24);
    lv_label_set_long_mode(hintLabel, LV_LABEL_LONG_WRAP);

    calLabel = lv_label_create(content);
    lv_obj_set_style_text_color(calLabel, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_width(calLabel, SCREEN_W - 24);
    lv_label_set_long_mode(calLabel, LV_LABEL_LONG_WRAP);
    refreshCalLabel();

    static const CalButtonDef calButtons[] = {
        {"Xmin-", CAL_X_MIN, -50},
        {"Xmin+", CAL_X_MIN, 50},
        {"Xmax-", CAL_X_MAX, -50},
        {"Xmax+", CAL_X_MAX, 50},
    };

    lv_obj_t *calRow = lv_obj_create(content);
    lv_obj_set_size(calRow, SCREEN_W - 24, 28);
    lv_obj_set_style_bg_opa(calRow, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(calRow, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(calRow, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(calRow, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(calRow, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (size_t i = 0; i < sizeof(calButtons) / sizeof(calButtons[0]); ++i)
    {
      calAdjustments[i] = CalAdjustPayload{this, calButtons[i].field, calButtons[i].delta};
      createSmallButton(calRow, calButtons[i].label, onCalAdjustClicked, &calAdjustments[i]);
    }

    static const CalButtonDef calButtonsY[] = {
        {"Ymin-", CAL_Y_MIN, -50},
        {"Ymin+", CAL_Y_MIN, 50},
        {"Ymax-", CAL_Y_MAX, -50},
        {"Ymax+", CAL_Y_MAX, 50},
    };

    lv_obj_t *calRowY = lv_obj_create(content);
    lv_obj_set_size(calRowY, SCREEN_W - 24, 28);
    lv_obj_set_style_bg_opa(calRowY, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(calRowY, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(calRowY, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(calRowY, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(calRowY, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (size_t i = 0; i < sizeof(calButtonsY) / sizeof(calButtonsY[0]); ++i)
    {
      calAdjustments[i + 4] = CalAdjustPayload{this, calButtonsY[i].field, calButtonsY[i].delta};
      createSmallButton(calRowY, calButtonsY[i].label, onCalAdjustClicked, &calAdjustments[i + 4]);
    }

    lv_obj_t *actionRow = lv_obj_create(content);
    lv_obj_set_size(actionRow, SCREEN_W - 24, 32);
    lv_obj_set_style_bg_opa(actionRow, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(actionRow, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(actionRow, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(actionRow, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(actionRow, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *saveBtn = lv_btn_create(actionRow);
    lv_obj_set_size(saveBtn, 100, 28);
    lv_obj_add_event_cb(saveBtn, onSaveCalClicked, LV_EVENT_CLICKED, this);
    lv_obj_t *saveLbl = lv_label_create(saveBtn);
    lv_label_set_text(saveLbl, "Save Cal");
    lv_obj_center(saveLbl);

    lv_obj_t *resetBtn = lv_btn_create(actionRow);
    lv_obj_set_size(resetBtn, 100, 28);
    lv_obj_add_event_cb(resetBtn, onResetCalClicked, LV_EVENT_CLICKED, this);
    lv_obj_t *resetLbl = lv_label_create(resetBtn);
    lv_label_set_text(resetLbl, "Reset");
    lv_obj_center(resetLbl);
  }
  else
  {
    hintLabel = lv_label_create(content);
    lv_label_set_text(hintLabel,
                      "Capacitive: coords pre-calibrated. "
                      "If mirrored, set TOUCH_MIRROR_X/Y in platformio.ini. "
                      "No touch? try CST820_INT=36.");
    lv_obj_set_style_text_color(hintLabel, lv_color_hex(0x777777), LV_PART_MAIN);
    lv_obj_set_width(hintLabel, SCREEN_W - 24);
    lv_label_set_long_mode(hintLabel, LV_LABEL_LONG_WRAP);
  }
}

void TouchConfigScreen::show()
{
  lv_scr_load(root);
}

void TouchConfigScreen::update()
{
  if (!display)
    return;

  TouchSample sample;
  char buf[64];
  if (display->sampleTouch(sample))
  {
    lv_label_set_text(stateLabel, "State: pressed");

    if (display->isResistiveTouch())
      snprintf(buf, sizeof(buf), "Raw: %ld, %ld", (long)sample.rawX, (long)sample.rawY);
    else
      snprintf(buf, sizeof(buf), "Panel: %ld, %ld", (long)sample.rawX, (long)sample.rawY);
    lv_label_set_text(rawLabel, buf);

    snprintf(buf, sizeof(buf), "Mapped: %ld, %ld", (long)sample.mappedX, (long)sample.mappedY);
    lv_label_set_text(mappedLabel, buf);

    lv_coord_t padW = lv_obj_get_width(touchPad) - 10;
    lv_coord_t padH = lv_obj_get_height(touchPad) - 10;
    lv_coord_t dotX = (padW * sample.mappedX) / (SCREEN_W - 1);
    lv_coord_t dotY = (padH * sample.mappedY) / (SCREEN_H - 1);
    lv_obj_set_pos(touchDot, dotX, dotY);
    lv_obj_clear_flag(touchDot, LV_OBJ_FLAG_HIDDEN);
  }
  else
  {
    lv_label_set_text(stateLabel, "State: released");
  }
}

void TouchConfigScreen::setBackCallback(NavCallback cb)
{
  backCb = cb;
}

void TouchConfigScreen::refreshCalLabel()
{
  if (!calLabel)
    return;

  char buf[96];
  snprintf(buf, sizeof(buf), "Cal X:%d..%d Y:%d..%d", cal.xMin, cal.xMax, cal.yMin, cal.yMax);
  lv_label_set_text(calLabel, buf);
}

void TouchConfigScreen::applyCalibration()
{
  if (display)
    display->applyTouchCalibration(cal);
}

void TouchConfigScreen::adjustCal(int fieldIndex, int delta)
{
  switch (fieldIndex)
  {
  case CAL_X_MIN:
    cal.xMin += delta;
    break;
  case CAL_X_MAX:
    cal.xMax += delta;
    break;
  case CAL_Y_MIN:
    cal.yMin += delta;
    break;
  case CAL_Y_MAX:
    cal.yMax += delta;
    break;
  }

  if (cal.xMin >= cal.xMax)
    cal.xMax = cal.xMin + 100;
  if (cal.yMin >= cal.yMax)
    cal.yMax = cal.yMin + 100;

  applyCalibration();
  refreshCalLabel();
}

void TouchConfigScreen::onBackClicked(lv_event_t *e)
{
  auto *self = static_cast<TouchConfigScreen *>(lv_event_get_user_data(e));
  if (self && self->backCb)
    self->backCb();
}

void TouchConfigScreen::onSaveCalClicked(lv_event_t *e)
{
  auto *self = static_cast<TouchConfigScreen *>(lv_event_get_user_data(e));
  if (!self || !self->settings)
    return;

  self->settings->saveTouchCal(self->cal);
  self->applyCalibration();
}

void TouchConfigScreen::onResetCalClicked(lv_event_t *e)
{
  auto *self = static_cast<TouchConfigScreen *>(lv_event_get_user_data(e));
  if (!self || !self->settings)
    return;

  self->settings->resetTouchCal();
  self->cal = self->settings->defaultTouchCal();
  self->applyCalibration();
  self->refreshCalLabel();
}

void TouchConfigScreen::onCalAdjustClicked(lv_event_t *e)
{
  auto *payload = static_cast<CalAdjustPayload *>(lv_event_get_user_data(e));
  if (!payload || !payload->screen)
    return;

  payload->screen->adjustCal(payload->field, payload->delta);
}