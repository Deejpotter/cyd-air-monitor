#pragma once

#include <stdint.h>

/** Board-specific environmental sensor wiring — edit per env / harness. */

#if defined(MODEL_JC4827W543R)

// Exactly one sensor — set one to 1, the other to 0, then reflash.
#define SENSOR_USE_DHT 1
#define SENSOR_USE_BME680 0

// DHT11 on P4: GND, 3.3V, IO17 (data)
static constexpr uint8_t DHT_DATA_PIN = 17;
static constexpr uint8_t DHT_SENSOR_TYPE = 11; // DHT11

// BME680 on P4 (swap harness): GND, 3.3V, IO17 (SDA), IO18 (SCL)
static constexpr uint8_t BME680_I2C_SDA = 17;
static constexpr uint8_t BME680_I2C_SCL = 18;

#if (SENSOR_USE_DHT && SENSOR_USE_BME680) || (!SENSOR_USE_DHT && !SENSOR_USE_BME680)
#error "JC4827: enable exactly one of SENSOR_USE_DHT or SENSOR_USE_BME680"
#endif

#elif defined(MODEL_2432S028R)

static constexpr uint8_t DHT_DATA_PIN = 27;
static constexpr uint8_t DHT_SENSOR_TYPE = 11;
#define SENSOR_USE_DHT 1
#define SENSOR_USE_BME680 0

#elif defined(MODEL_JC2432W328C)

static constexpr uint8_t DHT_DATA_PIN = 22;
static constexpr uint8_t DHT_SENSOR_TYPE = 11;
#define SENSOR_USE_DHT 1
#define SENSOR_USE_BME680 0

#else // jc2432w328r and fallback

static constexpr uint8_t DHT_DATA_PIN = 21;
static constexpr uint8_t DHT_SENSOR_TYPE = 11;
#define SENSOR_USE_DHT 1
#define SENSOR_USE_BME680 0

#endif
