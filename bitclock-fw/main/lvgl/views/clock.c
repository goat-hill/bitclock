#include "clock.h"

#include "aqi_alert.h"
#include "lvgl/styles.h"
#include "lvgl/utils.h"
#include <time.h>
#include "helper.h"

lv_helper_view_mode_clock_data_t lv_helper_view_mode_clock_data;

static lv_obj_t *time_label;
static lv_obj_t *time_sublabel;


void lv_helper_clock_create() {
  time_label = create_label(lv_screen_active(), &large_number_style, LV_ALIGN_CENTER, 0, 0);
  time_sublabel = create_label(lv_screen_active(), &sublabel_style, LV_ALIGN_CENTER, 0, 60);
  lv_helper_aqi_alert_create(true);
}

void lv_helper_clock_update(lv_helper_view_mode_clock_data_t *data) {
  static struct tm timeinfo;
  localtime_r(&data->curtime, &timeinfo);

  static char time_label_str[6];
  static char time_sublabel_str[30];

  format_time_and_date(&timeinfo, data->hour24, time_label_str, sizeof(time_label_str), time_sublabel_str, sizeof(time_sublabel_str));

  set_text_if_changed(time_label, time_label_str);
  set_text_if_changed(time_sublabel, time_sublabel_str);

  lv_helper_aqi_alert_update(&lv_helper_aqi_alert_data);
}
