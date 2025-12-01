// EnvSensor.cpp
// ESP32-optimized DHT11 reader based on abdellah2288/esp32-dht11
#include "EnvSensor.h"
#include <rom/ets_sys.h>

EnvSensor envSensor;

EnvSensor::EnvSensor() {}

bool EnvSensor::begin(uint8_t p)
{
  pin = (gpio_num_t)p;
  return true;
}

bool EnvSensor::read()
{
  unsigned long now = millis();
  if (now - lastRead < minInterval)
  {
    return isfinite(temperature) && isfinite(humidity);
  }

  int result = dht11Read(5); // 5 connection attempts
  if (result == 0)
  {
    lastRead = now;
    Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
    return true;
  }
  return false;
}

int EnvSensor::waitForState(int state, int timeout)
{
  gpio_set_direction(pin, GPIO_MODE_INPUT);
  int count = 0;

  while (gpio_get_level(pin) != state)
  {
    if (count >= timeout)
      return -1;
    count += 2;
    ets_delay_us(2);
  }

  return count;
}

void EnvSensor::holdLow(int holdTimeUs)
{
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_set_level(pin, 0);
  ets_delay_us(holdTimeUs);
  gpio_set_level(pin, 1);
}

int EnvSensor::dht11Read(int connectionTimeout)
{
  int waited = 0;
  int oneDuration = 0;
  int zeroDuration = 0;
  int timeoutCounter = 0;

  uint8_t receivedData[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

  while (timeoutCounter < connectionTimeout)
  {
    timeoutCounter++;
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    holdLow(18000);

    waited = waitForState(0, 40);
    if (waited == -1)
    {
      Serial.println("DHT11: Failed at phase 1");
      ets_delay_us(20000);
      continue;
    }

    waited = waitForState(1, 90);
    if (waited == -1)
    {
      Serial.println("DHT11: Failed at phase 2");
      ets_delay_us(20000);
      continue;
    }

    waited = waitForState(0, 90);
    if (waited == -1)
    {
      Serial.println("DHT11: Failed at phase 3");
      ets_delay_us(20000);
      continue;
    }

    break;
  }

  if (timeoutCounter == connectionTimeout)
    return -1;

  // Read 40 bits (5 bytes)
  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      zeroDuration = waitForState(1, 58);
      oneDuration = waitForState(0, 74);
      receivedData[i] |= (oneDuration > zeroDuration) << (7 - j);
    }
  }

  // Validate checksum
  int crc = receivedData[0] + receivedData[1] + receivedData[2] + receivedData[3];
  crc = crc & 0xff;

  if (crc == receivedData[4])
  {
    humidity = receivedData[0] + receivedData[1] / 10.0;
    temperature = receivedData[2] + receivedData[3] / 10.0;
    return 0;
  }
  else
  {
    Serial.println("DHT11: Wrong checksum");
    return -1;
  }
}
