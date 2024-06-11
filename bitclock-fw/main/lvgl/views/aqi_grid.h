#include <stdint.h>

typedef struct {
  float temp_celsius;
  uint8_t humidity_pct;
  uint16_t co2_ppm;
  int32_t voc_index;
  int32_t nox_index;
} lv_helper_view_mode_aqi_data_t;
extern lv_helper_view_mode_aqi_data_t lv_helper_view_mode_aqi_data;

void lv_helper_aqi_grid_create();
void lv_helper_aqi_grid_update(lv_helper_view_mode_aqi_data_t *data);
