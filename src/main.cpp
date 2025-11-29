/**
 * Minimal main for CYD template + LVGL
 * - Uses TemplateCode for display + touch init
 * - Uses MainInterface for simple UI
 * - No scheduler, no sensor manager
 */

#include "TemplateCode.h"
#include "MainInterface.h"
#include "EnvSensor.h"

TemplateCode &templateCode = TemplateCode::getInstance();
MainInterface mainInterface;

void setup()
{
  Serial.begin(115200);
  delay(200);

  // Initialize display, LVGL, and touch via template
  if (!templateCode.begin())
  {
    Serial.println("Failed to initialize template code");
    while (true)
      delay(1000);
  }

  // Initialize DHT11 on pin 21 (GPIO21)
  envSensor.begin(21);
  // prime a read
  envSensor.read();

  // Initialize UI
  mainInterface.init();
}

void loop()
{
  // LVGL tick/flush is handled inside update
  templateCode.update();
  mainInterface.update();
  delay(5);
}
