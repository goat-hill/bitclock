#include "lv_helper.h"

#include "esp_timer.h"
#include "lvgl/lvgl.h"

#include "views/aqi_grid.h"
#include "views/clock.h"
#include "views/logo.h"
#include "views/passkey.h"
#include "views/weather.h"

static const char *TAG = "lv_helper";

view_mode_t active_view_mode = VIEW_MODE_NONE;

uint32_t lv_tick_cb() { return esp_timer_get_time() / 1000; }

void lv_helper_set_view_mode(view_mode_t view_mode) {
  if (view_mode != active_view_mode) {
    lv_obj_t *screen = lv_scr_act();
    if (view_mode != VIEW_MODE_NONE) {
      lv_obj_clean(screen);
    }

    switch (view_mode) {
    case VIEW_MODE_NONE:
      break;
    case VIEW_MODE_PASSKEY:
      lv_helper_passkey_create();
      break;
    case VIEW_MODE_LOGO:
      lv_helper_logo_create();
      break;
    case VIEW_MODE_CLOCK:
      lv_helper_clock_create();
      break;
    case VIEW_MODE_AQI_GRID:
      lv_helper_aqi_grid_create();
      break;
    case VIEW_MODE_WEATHER:
      lv_helper_weather_create();
      break;
    case VIEW_MODE_MAX:
      ESP_ERROR_CHECK(ESP_FAIL);
      break;
    }

    active_view_mode = view_mode;
  }
}

void lv_helper_update() {
  switch (active_view_mode) {
  case VIEW_MODE_NONE:
    break;
  case VIEW_MODE_PASSKEY:
    lv_helper_passkey_update(&lv_helper_view_mode_passkey_data);
    break;
  case VIEW_MODE_LOGO:
    lv_helper_logo_update();
    break;
  case VIEW_MODE_CLOCK:
    lv_helper_clock_update(&lv_helper_view_mode_clock_data);
    break;
  case VIEW_MODE_AQI_GRID:
    lv_helper_aqi_grid_update(&lv_helper_view_mode_aqi_data);
    break;
  case VIEW_MODE_WEATHER:
    lv_helper_weather_update(&lv_helper_view_mode_weather_data);
    break;
  case VIEW_MODE_MAX:
    ESP_ERROR_CHECK(ESP_FAIL);
    break;
  }
}

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
