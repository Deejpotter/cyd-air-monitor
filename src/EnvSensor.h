// EnvSensor.h
// Minimal DHT11 reader (no external libs) for GPIO pin (DHT11)
#ifndef ENV_SENSOR_H
#define ENV_SENSOR_H

#include <Arduino.h>

class EnvSensor
{
public:
  EnvSensor();
  bool begin(uint8_t pin);
  // read() updates cached values; returns true if a valid reading is available
  bool read();
  float getTemperature() const { return temperature; }
  float getHumidity() const { return humidity; }
  unsigned long lastReadMillis() const { return lastRead; }

private:
  uint8_t pin = 21;
  float temperature = NAN;
  float humidity = NAN;
  unsigned long lastRead = 0;
  const unsigned long minInterval = 2000; // DHT11 min sample interval

  bool performRead();
};

extern EnvSensor envSensor;

#endif // ENV_SENSOR_H
