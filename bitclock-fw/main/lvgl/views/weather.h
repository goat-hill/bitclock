#include <stdint.h>

#include "libs/weather_api.h"
#include "lv_helper.h"

typedef struct {
  weather_icon_t icon;
  int16_t temp_high;
  int16_t temp_low;
  time_t curtime;
  bool hour24;
} lv_helper_view_mode_weather_data_t;
extern lv_helper_view_mode_weather_data_t lv_helper_view_mode_weather_data;

void lv_helper_weather_create();
void lv_helper_weather_update(lv_helper_view_mode_weather_data_t *data);
