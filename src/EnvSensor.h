// EnvSensor.h
// ESP32-optimized DHT11 reader based on abdellah2288/esp32-dht11
#ifndef ENV_SENSOR_H
#define ENV_SENSOR_H

#include <Arduino.h>
#include <driver/gpio.h>

class EnvSensor
{
public:
  EnvSensor();
  bool begin(uint8_t pin);
  bool read();
  float getTemperature() const { return temperature; }
  float getHumidity() const { return humidity; }
  unsigned long lastReadMillis() const { return lastRead; }

private:
  gpio_num_t pin;
  float temperature = NAN;
  float humidity = NAN;
  unsigned long lastRead = 0;
  const unsigned long minInterval = 2000;

  // ESP32 low-level DHT11 functions
  int waitForState(int state, int timeout);
  void holdLow(int holdTimeUs);
  int dht11Read(int connectionTimeout);
};

extern EnvSensor envSensor;

#endif // ENV_SENSOR_H
