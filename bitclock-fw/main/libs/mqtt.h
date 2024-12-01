#pragma once

#include "aqi_alert.h"

void send_homeassistant_mqtt_sensor_data(const char *mqtt_url, aqi_data_t *aqi_data);
