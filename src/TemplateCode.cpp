/**
 * Last updated: 2nd Feb 2025
 * By: Daniel Potter
 * Description: This file contains the template code for setting up the screen and touch interface.
 */

#include "TemplateCode.h"

namespace {

#if defined(MODEL_JC2432W328R) || defined(MODEL_2432S028R) || defined(MODEL_JC4827W543R)
void mapResistivePoint(int32_t rawX, int32_t rawY, const TouchCalibration &cal, int32_t &outX, int32_t &outY)
{
  int32_t mx = map(rawX, cal.xMin, cal.xMax, 0, SCREEN_WIDTH - 1);
  int32_t my = map(rawY, cal.yMin, cal.yMax, 0, SCREEN_HEIGHT - 1);
#if TOUCH_MIRROR_X
  mx = (SCREEN_WIDTH - 1) - mx;
#endif
#if TOUCH_MIRROR_Y
  my = (SCREEN_HEIGHT - 1) - my;
#endif
  outX = mx;
  outY = my;
}
#endif

void clampTouchPoint(int32_t &x, int32_t &y)
{
  if (x < 0)
    x = 0;
  if (x > (int32_t)SCREEN_WIDTH - 1)
    x = SCREEN_WIDTH - 1;
  if (y < 0)
    y = 0;
  if (y > (int32_t)SCREEN_HEIGHT - 1)
    y = SCREEN_HEIGHT - 1;
}

} // namespace

// Initialize static members
TemplateCode *TemplateCode::instance = nullptr;
lv_disp_draw_buf_t TemplateCode::draw_buf;
lv_color_t TemplateCode::buf[SCREEN_WIDTH * SCREEN_HEIGHT / 10];

TemplateCode::TemplateCode()
#if defined(MODEL_JC4827W543R)
    : displayBus(nullptr),
      displayPanel(nullptr),
      gfx(nullptr),
      touchSpi(HSPI),
      ts(XPT2046_CS, XPT2046_IRQ)
#elif defined(MODEL_JC2432W328R)
    : mySpi(VSPI),
      ts(XPT2046_CS, XPT2046_IRQ),
      tft(SCREEN_WIDTH, SCREEN_HEIGHT)
#elif defined(MODEL_JC2432W328C)
    : ts(),
      tft(SCREEN_WIDTH, SCREEN_HEIGHT)
#elif defined(MODEL_2432S028R)
    : mySpi(VSPI),
      ts(XPT2046_CS, XPT2046_IRQ),
      tft(SCREEN_WIDTH, SCREEN_HEIGHT)
#else
    : tft(SCREEN_WIDTH, SCREEN_HEIGHT)
#endif
{
  SettingsStore defaults;
  touchCal = defaults.defaultTouchCal();
}

TemplateCode &TemplateCode::getInstance()
{
  if (instance == nullptr)
  {
    instance = new TemplateCode();
  }
  return *instance;
}

bool TemplateCode::begin()
{
#if LV_USE_LOG != 0
  lv_log_register_print_cb(debugPrint);
#endif

  initializeHardware();
  initializeLVGL();
  setupTouchscreen();
  setupDisplay();

  return true;
}

void TemplateCode::applyTouchCalibration(const TouchCalibration &cal)
{
  touchCal = cal;
}

bool TemplateCode::isResistiveTouch() const
{
#if defined(MODEL_JC2432W328R) || defined(MODEL_2432S028R) || defined(MODEL_JC4827W543R)
  return true;
#else
  return false;
#endif
}

const char *TemplateCode::touchTypeName() const
{
#if defined(MODEL_JC2432W328C)
  return "Capacitive (CST820)";
#elif defined(MODEL_JC4827W543R)
  return "Resistive (XPT2046)";
#elif defined(MODEL_JC2432W328R)
  return "Resistive (XPT2046)";
#elif defined(MODEL_2432S028R)
  return "Resistive (XPT2046)";
#else
  return "Unknown";
#endif
}

bool TemplateCode::sampleTouch(TouchSample &sample)
{
  sample = {};

#if defined(MODEL_JC2432W328R) || defined(MODEL_2432S028R) || defined(MODEL_JC4827W543R)
  if (!(ts.tirqTouched() && ts.touched()))
    return false;

  TS_Point p = ts.getPoint();
  sample.pressed = true;
  sample.rawX = p.x;
  sample.rawY = p.y;
  mapResistivePoint(p.x, p.y, touchCal, sample.mappedX, sample.mappedY);
  clampTouchPoint(sample.mappedX, sample.mappedY);
  return true;
#endif

#if defined(MODEL_JC2432W328C)
  TOUCHINFO ti = {};
  if (!ts.getSamples(&ti) || ti.count == 0)
    return false;

  sample.pressed = true;
  sample.rawX = ti.x[0];
  sample.rawY = ti.y[0];

#if CAP_TOUCH_NATIVE_LANDSCAPE
  sample.mappedX = (int32_t)ti.y[0];
  sample.mappedY = (int32_t)(PANEL_WIDTH - 1) - (int32_t)ti.x[0];
#else
  sample.mappedX = (int32_t)ti.x[0];
  sample.mappedY = (int32_t)ti.y[0];
#endif
#if TOUCH_MIRROR_X
  sample.mappedX = (SCREEN_WIDTH - 1) - sample.mappedX;
#endif
#if TOUCH_MIRROR_Y
  sample.mappedY = (SCREEN_HEIGHT - 1) - sample.mappedY;
#endif
  clampTouchPoint(sample.mappedX, sample.mappedY);
  return true;
#endif

  return false;
}

void TemplateCode::initializeHardware()
{
  initRGBled();
  ChangeRGBColor(RGB_COLOR_1);

#if defined(MODEL_JC2432W328R)
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
#endif
#if defined(MODEL_2432S028R)
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
#endif
#if defined(MODEL_JC4827W543R)
  touchSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
#endif
}

void TemplateCode::initializeLVGL()
{
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, nullptr, SCREEN_WIDTH * SCREEN_HEIGHT / 10);
}

void TemplateCode::setupTouchscreen()
{
#if defined(MODEL_JC4827W543R)
  ts.begin(touchSpi);
#elif defined(MODEL_JC2432W328R) || defined(MODEL_2432S028R)
  ts.begin(mySpi);
#endif
#if defined(MODEL_JC2432W328R) || defined(MODEL_2432S028R) || defined(MODEL_JC4827W543R)
  ts.setRotation(TOUCH_ROTATION);
#endif
#if defined(MODEL_JC2432W328C)
  ts.init(CST820_SDA, CST820_SCL, CST820_RST, CST820_INT);
#if !CAP_TOUCH_NATIVE_LANDSCAPE
  int oriDeg = 0;
  switch (LVGL_ROTATION & 0x3)
  {
  case 0:
    oriDeg = 0;
    break;
  case 1:
    oriDeg = 90;
    break;
  case 2:
    oriDeg = 180;
    break;
  case 3:
    oriDeg = 270;
    break;
  }
  ts.setOrientation(oriDeg, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
  Serial.printf("BBCapTouch init: sensor=%d native_landscape=%d %ux%u\n",
                ts.sensorType(), CAP_TOUCH_NATIVE_LANDSCAPE,
                (unsigned)SCREEN_WIDTH, (unsigned)SCREEN_HEIGHT);
  Serial.flush();
#endif
}

void TemplateCode::setupDisplay()
{
#if defined(MODEL_JC4827W543R)
  displayBus = new Arduino_ESP32QSPI(45, 47, 21, 48, 40, 39);
  displayPanel = new Arduino_NV3041A(displayBus, GFX_NOT_DEFINED, TFT_DISPLAY_ROTATION, true);
  gfx = new Arduino_Canvas(SCREEN_WIDTH, SCREEN_HEIGHT, displayPanel);
  if (!gfx->begin())
  {
    Serial.println("JC4827 display init failed");
  }
  gfx->fillScreen(BLACK);
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#else
  tft.begin();
  tft.setRotation(TFT_DISPLAY_ROTATION);
#ifdef TFT_MADCTL_OVERRIDE
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MADCTL_OVERRIDE);
#endif
#if TFT_INVERT_DISPLAY
  tft.invertDisplay(true);
#else
  tft.invertDisplay(false);
#endif
#endif

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = flushDisplay;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.sw_rotate = 1;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = readTouchpad;
  lv_indev_drv_register(&indev_drv);
}

void TemplateCode::flushDisplay(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
  auto &display = getInstance();
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if defined(MODEL_JC4827W543R)
  if (display.gfx)
    display.gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  display.tft.startWrite();
  display.tft.setAddrWindow(area->x1, area->y1, w, h);
  display.tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  display.tft.endWrite();
#endif

  lv_disp_flush_ready(disp_drv);
}

#if defined(MODEL_JC2432W328R) || defined(MODEL_2432S028R) || defined(MODEL_JC4827W543R)
void TemplateCode::readTouchpad(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
  auto &display = getInstance();
  TouchSample sample;
  if (!display.sampleTouch(sample))
  {
    data->state = LV_INDEV_STATE_REL;
    return;
  }

  data->state = LV_INDEV_STATE_PR;
  data->point.x = (lv_coord_t)sample.mappedX;
  data->point.y = (lv_coord_t)sample.mappedY;
}
#endif

#if defined(MODEL_JC2432W328C)
void TemplateCode::readTouchpad(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
  auto &display = getInstance();
  TouchSample sample;
  if (!display.sampleTouch(sample))
  {
    data->state = LV_INDEV_STATE_REL;
    return;
  }

  data->state = LV_INDEV_STATE_PR;
  data->point.x = (lv_coord_t)sample.mappedX;
  data->point.y = (lv_coord_t)sample.mappedY;
}
#endif

void TemplateCode::update()
{
  lv_timer_handler();
#if defined(MODEL_JC4827W543R)
  if (gfx)
    gfx->flush();
#endif
}

void TemplateCode::setDisplayRotation(uint8_t rotation)
{
  rotation &= 0x3;
  lv_disp_t *disp = lv_disp_get_default();
  if (!disp)
    return;

  lv_disp_set_rotation(disp, static_cast<lv_disp_rot_t>(rotation));
  lv_obj_invalidate(lv_scr_act());
  lv_refr_now(disp);
  Serial.printf("Display rotation set to %u (%s)\n", rotation, displayRotationLabel());
}

uint8_t TemplateCode::displayRotation() const
{
  lv_disp_t *disp = lv_disp_get_default();
  if (!disp)
    return 0;
  return static_cast<uint8_t>(lv_disp_get_rotation(disp));
}

const char *TemplateCode::displayRotationLabel() const
{
  switch (displayRotation())
  {
  case 1:
    return "90°";
  case 2:
    return "180°";
  case 3:
    return "270°";
  default:
    return "0°";
  }
}

#if LV_USE_LOG != 0
void TemplateCode::debugPrint(const char *buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif
