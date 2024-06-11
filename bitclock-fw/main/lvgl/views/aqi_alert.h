#include <stdbool.h>

#include "libs/aqi_alert.h"

typedef struct {
  aqi_alert_reason_t alert_reason;
} lv_helper_aqi_alert_data_t;
extern lv_helper_aqi_alert_data_t lv_helper_aqi_alert_data;

void lv_helper_aqi_alert_create(bool align_right);
void lv_helper_aqi_alert_update(lv_helper_aqi_alert_data_t *data);
