#include "SensorManager.h"
#include "SensorConfig.h"

#include <Arduino.h>
#include <Wire.h>

#if SENSOR_USE_DHT && defined(MODEL_JC4827W543R)
#include <DHTesp.h>
static DHTesp dhtEsp;
#elif SENSOR_USE_DHT
#include <DHT.h>
static DHT *dhtDriver = nullptr;
#endif

#if SENSOR_USE_BME680
#include <Adafruit_BME680.h>
static Adafruit_BME680 bme680;
#endif

SensorManager::SensorManager(uint32_t intervalMs)
    : interval(intervalMs), lastRead(0), tmp(NAN), hum(NAN), bmePresent(false)
{
}

void SensorManager::begin()
{
#if SENSOR_USE_BME680
  Wire.begin(BME680_I2C_SDA, BME680_I2C_SCL);
  bmePresent = bme680.begin();
  if (bmePresent)
  {
    bme680.setTemperatureOversampling(BME680_OS_8X);
    bme680.setHumidityOversampling(BME680_OS_2X);
    bme680.setPressureOversampling(BME680_OS_4X);
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme680.setGasHeater(320, 150);
    Serial.printf("BME680 on I2C SDA=%u SCL=%u\n", BME680_I2C_SDA, BME680_I2C_SCL);
  }
  else
  {
    Serial.printf("BME680 not found on I2C SDA=%u SCL=%u\n", BME680_I2C_SDA, BME680_I2C_SCL);
  }
#endif

#if SENSOR_USE_DHT && defined(MODEL_JC4827W543R)
  dhtEsp.setup(DHT_DATA_PIN, DHTesp::DHT11);
  delay(2000); // DHT11 needs ~1–2 s after power-up before first read
#elif SENSOR_USE_DHT
  if (!dhtDriver)
  {
    dhtDriver = new DHT(DHT_DATA_PIN, DHT_SENSOR_TYPE);
    dhtDriver->begin();
    Serial.printf("DHT on GPIO %u\n", DHT_DATA_PIN);
  }
#endif
}

void SensorManager::logStartupDiagnostics()
{
#if SENSOR_USE_DHT && defined(MODEL_JC4827W543R)
  pinMode(DHT_DATA_PIN, INPUT_PULLUP);
  const int idleLevel = digitalRead(DHT_DATA_PIN);
  Serial.printf("DHT on GPIO %u (idle level=%d, expect 1 with pull-up)\n", DHT_DATA_PIN, idleLevel);
  if (idleLevel == LOW)
  {
    Serial.println("DHT: data line stuck LOW — check P4 wiring (GND, 3.3V, IO17 data)");
  }
#elif SENSOR_USE_DHT
  Serial.printf("DHT on GPIO %u\n", DHT_DATA_PIN);
#endif
}

void SensorManager::readSensors(float &tempC, float &humidity)
{
  tempC = NAN;
  humidity = NAN;

#if SENSOR_USE_BME680
  if (bmePresent && bme680.performReading())
  {
    tempC = bme680.temperature;
    humidity = bme680.humidity;
  }
#elif SENSOR_USE_DHT && defined(MODEL_JC4827W543R)
  TempAndHumidity th = dhtEsp.getTempAndHumidity();
  if (dhtEsp.getStatus() == DHTesp::ERROR_NONE)
  {
    tempC = th.temperature;
    humidity = th.humidity;
  }
  else
  {
    static unsigned long lastDhtFailLog = 0;
    if (millis() - lastDhtFailLog > 10000)
    {
      lastDhtFailLog = millis();
      Serial.printf("DHT fail GPIO %u: %s (idle=%d)\n",
                    DHT_DATA_PIN, dhtEsp.getStatusString(), digitalRead(DHT_DATA_PIN));
    }
  }
#elif SENSOR_USE_DHT
  if (dhtDriver)
  {
    tempC = dhtDriver->readTemperature(true);
    humidity = dhtDriver->readHumidity(false);
    static unsigned long lastDhtFailLog = 0;
    if (isnan(tempC) && isnan(humidity) && (millis() - lastDhtFailLog) > 10000)
    {
      lastDhtFailLog = millis();
      Serial.printf("DHT read failed on GPIO %u (check wiring)\n", DHT_DATA_PIN);
    }
  }
#endif
}

void SensorManager::update()
{
  unsigned long now = millis();
  if ((uint32_t)(now - lastRead) < interval)
    return;
  lastRead = now;

  float prevT = tmp;
  float prevH = hum;
  float t = NAN;
  float h = NAN;
  readSensors(t, h);

  if (!isnan(t))
    tmp = t;
  if (!isnan(h))
    hum = h;

  Serial.printf("Sensor: %.1f C, %.1f %%RH\n", tmp, hum);

  if (cb)
  {
    bool changed = false;
    if (!isnan(t) && (isnan(prevT) || fabs(t - prevT) >= 0.1f))
      changed = true;
    if (!isnan(h) && (isnan(prevH) || fabs(h - prevH) >= 0.1f))
      changed = true;
    if (changed)
      cb(tmp, hum);
  }
}

void SensorManager::onChange(Callback c)
{
  cb = c;
}

float SensorManager::lastTemperature() const { return tmp; }
float SensorManager::lastHumidity() const { return hum; }

bool SensorManager::bme680Active() const
{
#if SENSOR_USE_BME680
  return bmePresent;
#else
  return false;
#endif
}
