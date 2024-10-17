#include "helper.h"

lv_obj_t* create_label(lv_obj_t *parent, const lv_style_t *style, lv_align_t align, int x_offset, int y_offset) {
  lv_obj_t *label = lv_label_create(parent);
  lv_obj_add_style(label, style, LV_PART_MAIN);
  lv_obj_align(label, align, x_offset, y_offset);
  return label;
}

void format_time_and_date(struct tm *timeinfo, bool hour24, char *time_label_str, size_t time_label_size, char *time_sublabel_str, size_t time_sublabel_size) {
  char hour_str[3];
  char minute_str[3];
  char weekday_str[10];
  char month_str[4];
  char day_str[3];

  if (hour24) {
    snprintf(hour_str, sizeof(hour_str), "%u", timeinfo->tm_hour);
  } else {
    snprintf(hour_str, sizeof(hour_str), "%u",
             timeinfo->tm_hour % 12 == 0 ? 12 : timeinfo->tm_hour % 12);
  }
  strftime(minute_str, sizeof(minute_str), "%M", timeinfo);
  snprintf(time_label_str, time_label_size, "%s:%s", hour_str, minute_str);

  strftime(weekday_str, sizeof(weekday_str), "%A", timeinfo);
  strftime(month_str, sizeof(month_str), "%b", timeinfo);
  snprintf(day_str, sizeof(day_str), "%u", timeinfo->tm_mday);
  snprintf(time_sublabel_str, time_sublabel_size, "%s · %s %s",
           weekday_str, month_str, day_str);
  str_to_upper(time_sublabel_str);
}
