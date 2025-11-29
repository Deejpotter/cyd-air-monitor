// EnvSensor.cpp
#include "EnvSensor.h"

EnvSensor envSensor;

EnvSensor::EnvSensor() {}

bool EnvSensor::begin(uint8_t p)
{
  pin = p;
  // leave as INPUT_PULLUP by default; DHT line is pulled high by sensor
  pinMode(pin, INPUT_PULLUP);
  return true;
}

bool EnvSensor::read()
{
  unsigned long now = millis();
  if (now - lastRead < minInterval)
  {
    // still within caching window
    return isfinite(temperature) && isfinite(humidity);
  }

  bool ok = performRead();
  if (ok)
    lastRead = now;
  return ok;
}

// Minimal DHT11 read implementation using pulse timing
bool EnvSensor::performRead()
{
  // Send start signal
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(20); // >=18ms for DHT11
  // Pull high and switch to input
  digitalWrite(pin, HIGH);
  delayMicroseconds(30);
  pinMode(pin, INPUT_PULLUP);

  unsigned long timeout = 1000;

  // Wait for sensor response: LOW ~80us
  unsigned long t = pulseIn(pin, LOW, timeout);
  if (t == 0) return false;
  // Wait for HIGH ~80us
  t = pulseIn(pin, HIGH, timeout);
  if (t == 0) return false;

  uint8_t data[5] = {0, 0, 0, 0, 0};

  for (int i = 0; i < 40; ++i)
  {
    // low 50us
    t = pulseIn(pin, LOW, timeout);
    if (t == 0) return false;
    // high: length determines 0 or 1
    unsigned long len = pulseIn(pin, HIGH, timeout);
    if (len == 0) return false;
    int byteIndex = i / 8;
    data[byteIndex] <<= 1;
    if (len > 50) // threshold; 26-28us -> 0, ~70us -> 1. Use 50us cutoff
      data[byteIndex] |= 1;
  }

  uint8_t checksum = data[0] + data[1] + data[2] + data[3];
  if (checksum != data[4])
    return false;

  // DHT11 returns integer humidity and temperature in bytes 0 and 2
  humidity = (float)data[0] + ((float)data[1] / 10.0f);
  temperature = (float)data[2] + ((float)data[3] / 10.0f);
  return true;
}
