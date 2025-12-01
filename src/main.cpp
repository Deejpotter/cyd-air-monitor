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
  Serial.println("\n\n=== CYD Air Monitor Starting ===");

  // Initialize display, LVGL, and touch via template
  Serial.println("[Setup] Initializing display...");
  if (!templateCode.begin())
  {
    Serial.println("Failed to initialize template code");
    while (true)
      delay(1000);
  }
  Serial.println("[Setup] Display initialized");

  // Initialize DHT11 on pin 21 (GPIO21)
  Serial.println("[Setup] Initializing DHT11 sensor on GPIO21...");
  envSensor.begin(21);
  // prime a read
  Serial.println("[Setup] Performing initial sensor read...");
  bool success = envSensor.read();
  Serial.printf("[Setup] Initial read: %s\n", success ? "SUCCESS" : "FAILED");
  Serial.printf("[Setup] Initial values - Temp: %.1f°C, Humidity: %.1f%%\n",
                envSensor.getTemperature(), envSensor.getHumidity());

  // Initialize UI
  Serial.println("[Setup] Initializing UI...");
  mainInterface.init();
  Serial.println("[Setup] Setup complete!\n");
}

void loop()
{
  // LVGL tick/flush is handled inside update
  templateCode.update();
  mainInterface.update();
  delay(5);
}
