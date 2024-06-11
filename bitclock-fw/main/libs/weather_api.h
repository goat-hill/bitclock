#pragma once

#include "esp_err.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef enum {
  WEATHER_ICON_NONE = 0,
  WEATHER_ICON_CLEAR_NIGHT,
  WEATHER_ICON_CLOUD,
  WEATHER_ICON_FOGGY,
  WEATHER_ICON_PARTLY_CLOUDY_DAY,
  WEATHER_ICON_PARTLY_CLOUDY_NIGHT,
  WEATHER_ICON_RAINY,
  WEATHER_ICON_SNOWY,
  WEATHER_ICON_SUNNY,
  WEATHER_ICON_THUNDERSTORM,
  WEATHER_ICON_MAX // Always keep this as the last element
} weather_icon_t;

typedef struct {
  time_t requested_at;
  uint16_t temp_min;
  uint16_t temp_max;
  bool temp_celsius;
  weather_icon_t icon;
} wifi_weather_t;

esp_err_t refresh_daily_weather(wifi_weather_t *weather, const char *path);
