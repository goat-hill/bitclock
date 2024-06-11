#include "clock.h"

#include "aqi_alert.h"
#include "lvgl/styles.h"
#include "lvgl/utils.h"
#include <time.h>

lv_helper_view_mode_clock_data_t lv_helper_view_mode_clock_data;

static lv_obj_t *time_label;
static lv_obj_t *time_sublabel;

void lv_helper_clock_create() {
  time_label = lv_label_create(lv_screen_active());

  lv_obj_add_style(time_label, &large_number_style, LV_PART_MAIN);
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

  time_sublabel = lv_label_create(lv_screen_active());

  lv_obj_add_style(time_sublabel, &sublabel_style, LV_PART_MAIN);
  lv_obj_align(time_sublabel, LV_ALIGN_CENTER, 0, 60);

  lv_helper_aqi_alert_create(true);
}

void lv_helper_clock_update(lv_helper_view_mode_clock_data_t *data) {
  static struct tm timeinfo;
  localtime_r(&data->curtime, &timeinfo);

  static char time_label_str[6];
  static char time_sublabel_str[30];

  static char hour_str[3];
  static char minute_str[3];
  static char weekday_str[10];
  static char month_str[4];
  static char day_str[3];

  if (data->hour24) {
    snprintf(hour_str, sizeof(hour_str), "%u", timeinfo.tm_hour);
  } else {
    snprintf(hour_str, sizeof(hour_str), "%u",
             timeinfo.tm_hour % 12 == 0 ? 12 : timeinfo.tm_hour % 12);
  }
  strftime(minute_str, sizeof(minute_str), "%M", &timeinfo);
  snprintf(time_label_str, sizeof(time_label_str), "%s:%s", hour_str,
           minute_str);

  strftime(weekday_str, sizeof(weekday_str), "%A", &timeinfo);
  strftime(month_str, sizeof(month_str), "%b", &timeinfo);
  snprintf(day_str, sizeof(day_str), "%u", timeinfo.tm_mday);
  snprintf(time_sublabel_str, sizeof(time_sublabel_str), "%s · %s %s",
           weekday_str, month_str, day_str);
  str_to_upper(time_sublabel_str);

  set_text_if_changed(time_label, time_label_str);
  set_text_if_changed(time_sublabel, time_sublabel_str);

  lv_helper_aqi_alert_update(&lv_helper_aqi_alert_data);
}
