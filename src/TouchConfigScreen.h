#pragma once

#include <functional>
#include <lvgl.h>
#include "SettingsStore.h"

class TemplateCode;

class TouchConfigScreen
{
public:
  using NavCallback = std::function<void()>;

  TouchConfigScreen();
  ~TouchConfigScreen();

  void init(SettingsStore *store, TemplateCode *display);
  void show();
  void update();
  void setBackCallback(NavCallback cb);

  lv_obj_t *screen() const { return root; }

private:
  SettingsStore *settings;
  TemplateCode *display;
  NavCallback backCb;
  TouchCalibration cal;

  lv_obj_t *root;
  lv_obj_t *typeLabel;
  lv_obj_t *stateLabel;
  lv_obj_t *rawLabel;
  lv_obj_t *mappedLabel;
  lv_obj_t *calLabel;
  lv_obj_t *touchPad;
  lv_obj_t *touchDot;
  lv_obj_t *hintLabel;

  struct CalAdjustPayload
  {
    TouchConfigScreen *screen;
    int field;
    int delta;
  };
  CalAdjustPayload calAdjustments[8];

  static void onBackClicked(lv_event_t *e);
  static void onSaveCalClicked(lv_event_t *e);
  static void onResetCalClicked(lv_event_t *e);
  static void onCalAdjustClicked(lv_event_t *e);

  void createUi();
  void refreshCalLabel();
  void applyCalibration();
  void adjustCal(int fieldIndex, int delta);
};
