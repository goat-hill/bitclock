#pragma once

#include <stdint.h>

typedef enum {
  AQI_ALERT_NONE = 0,
  AQI_ALERT_TEMP_HIGH,
  AQI_ALERT_TEMP_LOW,
  AQI_ALERT_HUMDIITY_HIGH,
  AQI_ALERT_HUMDIITY_LOW,
  AQI_ALERT_CO2_HIGH,
  AQI_ALERT_NOX_HIGH,
  AQI_ALERT_VOC_HIGH,
  AQI_ALERT_COUNT
} aqi_alert_reason_t;

typedef struct {
  float temp_celsius;
  float humidity;
  uint16_t co2_ppm;
  int32_t nox_index;
  int32_t voc_index;
} aqi_data_t;

aqi_alert_reason_t aqi_alerts_check(aqi_data_t *aqi_data);
