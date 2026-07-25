#pragma once

#include <functional>
#include <stdint.h>

class SensorManager
{
public:
  using Callback = std::function<void(float tempC, float humidity)>;

  explicit SensorManager(uint32_t intervalMs = 2000);

  void begin();
  void logStartupDiagnostics();
  void update();

  void onChange(Callback cb);

  float lastTemperature() const;
  float lastHumidity() const;

  /** True when a BME680 responded on the configured I2C bus (jc4827 only). */
  bool bme680Active() const;

private:
  uint32_t interval;
  uint32_t lastRead;
  float tmp;
  float hum;
  Callback cb;
  bool bmePresent;

  void readSensors(float &tempC, float &humidity);
};
