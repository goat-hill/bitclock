#include "aqi_alert.h"

aqi_alert_reason_t aqi_alerts_check(aqi_data_t *aqi_data) {
  // TODO: Avoid flickering by some mechansim if oscillating around a border
  // value
  // TODO: Make these configurable

  // Extremely high
  if (aqi_data->nox_index >= 300) {
    return AQI_ALERT_NOX_HIGH;
  }
  if (aqi_data->voc_index >= 400) {
    return AQI_ALERT_VOC_HIGH;
  }
  if (aqi_data->co2_ppm >= 2000) {
    return AQI_ALERT_CO2_HIGH;
  }

  // High
  if (aqi_data->nox_index >= 150) {
    return AQI_ALERT_NOX_HIGH;
  }
  if (aqi_data->voc_index >= 325) {
    return AQI_ALERT_VOC_HIGH;
  }
  if (aqi_data->co2_ppm >= 1000) {
    return AQI_ALERT_CO2_HIGH;
  }

  // Elevated
  if (aqi_data->nox_index >= 20) {
    return AQI_ALERT_NOX_HIGH;
  }
  if (aqi_data->voc_index >= 250) {
    return AQI_ALERT_VOC_HIGH;
  }
  if (aqi_data->co2_ppm >= 700) {
    return AQI_ALERT_CO2_HIGH;
  }

  // Humidity
  if (aqi_data->humidity >= 70) {
    return AQI_ALERT_HUMDIITY_HIGH;
  }
  if (aqi_data->humidity <= 30) {
    return AQI_ALERT_HUMDIITY_LOW;
  }

  // No temp alerts for now

  return AQI_ALERT_NONE;
}
