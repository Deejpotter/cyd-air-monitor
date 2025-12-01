/**
 * Last Updated: Nov 29, 2025
 * Author: Daniel Potter
 *
 * Generic display and simple DHT11 UI:
 * - Header with resolution and model
 * - Color bars + gradient box
 * - Simple bouncing square + FPS indicator
 * - Temperature and humidity display (EnvSensor, DHT11 on GPIO21)
 */

#include "MainInterface.h"
#include <stdio.h>
#include "EnvSensor.h"

/**
 * Constructor: Initializes all UI element pointers to nullptr
 */
MainInterface::MainInterface()
{
  mainScreen = nullptr;
  headerContainer = nullptr;
  headerLabel = nullptr;
  infoLabel = nullptr;
  fpsLabel = nullptr;
  colorRow = nullptr;
  gradientBox = nullptr;
  bouncer = nullptr;
  tempLabel = nullptr;
  humLabel = nullptr;
}

/**
 * Destructor
 */
MainInterface::~MainInterface() {}

void MainInterface::init()
{
  // Create main screen
  mainScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(mainScreen, lv_color_hex(0x000000), 0);
  lv_obj_set_style_pad_all(mainScreen, 4, 0);

  // Flex column layout
  lv_obj_set_layout(mainScreen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(mainScreen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(mainScreen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

  // Header + tests
  createHeader();
  createTests();
  createSensorsArea();

  // Load screen
  lv_scr_load(mainScreen);
}

void MainInterface::createHeader()
{
  uint16_t w = lv_disp_get_hor_res(NULL);
  uint16_t h = lv_disp_get_ver_res(NULL);

  headerContainer = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(headerContainer);
  lv_obj_set_size(headerContainer, w, 42);
  lv_obj_set_style_bg_color(headerContainer, lv_color_hex(0x1E1E1E), 0);
  lv_obj_set_style_pad_all(headerContainer, 6, 0);

  headerLabel = lv_label_create(headerContainer);
  lv_obj_set_style_text_color(headerLabel, lv_color_hex(0xFFFFFF), 0);

#if defined(MODEL_JC2432W328R)
  const char *model = "JC2432W328R (Resistive)";
#elif defined(MODEL_JC2432W328C)
  const char *model = "JC2432W328C (Capacitive)";
#else
  const char *model = "CYD";
#endif
  char buf[96];
  lv_snprintf(buf, sizeof(buf), "%s  |  %ux%u", model, (unsigned)w, (unsigned)h);
  lv_label_set_text(headerLabel, buf);
  lv_obj_align(headerLabel, LV_ALIGN_LEFT_MID, 0, 0);

  // Info + FPS labels below header
  infoLabel = lv_label_create(mainScreen);
  lv_obj_set_style_text_color(infoLabel, lv_color_hex(0xA0A0A0), 0);
  lv_label_set_text(infoLabel, "Temp: --°C  Hum: --%");
  lv_obj_align(infoLabel, LV_ALIGN_TOP_LEFT, 4, 48);

  fpsLabel = lv_label_create(mainScreen);
  lv_obj_set_style_text_color(fpsLabel, lv_color_hex(0xA0FFA0), 0);
  lv_label_set_text(fpsLabel, "FPS: --");
  lv_obj_align(fpsLabel, LV_ALIGN_TOP_RIGHT, -4, 48);
}

void MainInterface::createSensorsArea()
{
  // Create temperature and humidity labels below header
  tempLabel = lv_label_create(mainScreen);
  lv_obj_set_style_text_color(tempLabel, lv_color_hex(0xFFD0D0), 0);
  lv_label_set_text(tempLabel, "Temp: --°C");
  lv_obj_align(tempLabel, LV_ALIGN_TOP_LEFT, 4, 70);

  humLabel = lv_label_create(mainScreen);
  lv_obj_set_style_text_color(humLabel, lv_color_hex(0xD0D0FF), 0);
  lv_label_set_text(humLabel, "Hum: --%");
  lv_obj_align(humLabel, LV_ALIGN_TOP_LEFT, 4, 90);
}

void MainInterface::createColorBars()
{
  colorRow = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(colorRow);
  lv_obj_set_width(colorRow, lv_pct(100));
  lv_obj_set_height(colorRow, 36);
  lv_obj_set_layout(colorRow, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(colorRow, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(colorRow, 4, 0);

  auto make_box = [&](lv_color_t c)
  {
    lv_obj_t *b = lv_obj_create(colorRow);
    lv_obj_remove_style_all(b);
    lv_obj_set_flex_grow(b, 1);
    lv_obj_set_style_bg_color(b, c, 0);
    return b;
  };

  make_box(lv_color_hex(0xFF0000)); // R
  make_box(lv_color_hex(0x00FF00)); // G
  make_box(lv_color_hex(0x0000FF)); // B
  make_box(lv_color_hex(0xFFFFFF)); // W
  make_box(lv_color_hex(0x000000)); // K

  // Gradient box
  gradientBox = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(gradientBox);
  lv_obj_set_width(gradientBox, lv_pct(100));
  lv_obj_set_height(gradientBox, 24);
  static lv_style_t grad;
  static bool inited = false;
  if (!inited)
  {
    inited = true;
    lv_style_init(&grad);
    lv_style_set_bg_grad_dir(&grad, LV_GRAD_DIR_HOR);
    lv_style_set_bg_color(&grad, lv_color_hex(0x000000));
    lv_style_set_bg_grad_color(&grad, lv_color_hex(0xFFFFFF));
  }
  lv_obj_add_style(gradientBox, &grad, 0);
}

void MainInterface::createTests()
{
  createColorBars();

  // Bouncing square
  bouncer = lv_obj_create(mainScreen);
  lv_obj_remove_style_all(bouncer);
  lv_obj_set_size(bouncer, 20, 20);
  lv_obj_set_style_bg_color(bouncer, lv_color_hex(0x00A0FF), 0);
  lv_obj_set_style_radius(bouncer, 4, 0);
  lv_obj_align(bouncer, LV_ALIGN_TOP_LEFT, bx, by);
}

void MainInterface::update()
{
  // Bouncer animation within screen bounds (below header and info rows)
  uint16_t w = lv_disp_get_hor_res(NULL);
  uint16_t h = lv_disp_get_ver_res(NULL);

  // Compute available area: margin below fps/info ~ 80px
  int top_margin = 80;
  int maxx = (int)w - 20 - 4;
  int maxy = (int)h - 20 - 4;
  if (bx <= 0 || bx >= maxx)
    bdx = -bdx;
  if (by <= top_margin || by >= maxy)
    bdy = -bdy;
  bx += bdx;
  by += bdy;
  if (by < top_margin)
    by = top_margin;
  lv_obj_set_pos(bouncer, bx, by);

  updateFPS();

  // Update environment sensor display every second (reads are cached by EnvSensor)
  static unsigned long lastSensorUpdate = 0;
  unsigned long now = millis();
  if (now - lastSensorUpdate >= 1000)
  {
    envSensor.read();
    updateSensorDisplay();
    lastSensorUpdate = now;
  }
}

void MainInterface::updateSensorDisplay()
{
  char buf[80];
  float t = envSensor.getTemperature();
  float h = envSensor.getHumidity();
  if (!isfinite(t) || !isfinite(h))
    lv_snprintf(buf, sizeof(buf), "Temp: --°C  Hum: --%%");
  else
    lv_snprintf(buf, sizeof(buf), "Temp: %.1f°C  Hum: %.1f%%", t, h);
  lv_label_set_text(infoLabel, buf);

  // Also update individual labels
  if (isfinite(t) && tempLabel)
  {
    char tbuf[32];
    lv_snprintf(tbuf, sizeof(tbuf), "Temp: %.1f°C", t);
    lv_label_set_text(tempLabel, tbuf);
  }
  if (isfinite(h) && humLabel)
  {
    char hbuf[32];
    lv_snprintf(hbuf, sizeof(hbuf), "Hum: %.1f%%", h);
    lv_label_set_text(humLabel, hbuf);
  }
}

void MainInterface::updateFPS()
{
  static uint32_t last = 0;
  static uint16_t frames = 0;
  frames++;
  uint32_t now = lv_tick_get();
  if (now - last >= 1000)
  {
    char buf[24];
    lv_snprintf(buf, sizeof(buf), "FPS: %u", (unsigned)frames);
    lv_label_set_text(fpsLabel, buf);
    frames = 0;
    last = now;
  }
}

void MainInterface::screenTouchEvent(lv_event_t *e)
{
  (void)e; // Touch removed for simple DHT interface
}
