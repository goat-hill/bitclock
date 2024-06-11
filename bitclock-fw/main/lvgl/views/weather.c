#include "weather.h"
#include "aqi_alert.h"
#include "lvgl/lvgl.h"
#include "lvgl/styles.h"
#include "lvgl/utils.h"

lv_helper_view_mode_weather_data_t lv_helper_view_mode_weather_data;

LV_IMAGE_DECLARE(material_clear_night);
LV_IMAGE_DECLARE(material_cloud);
LV_IMAGE_DECLARE(material_foggy);
LV_IMAGE_DECLARE(material_partly_cloudy_day);
LV_IMAGE_DECLARE(material_partly_cloudy_night);
LV_IMAGE_DECLARE(material_rainy);
LV_IMAGE_DECLARE(material_snowy);
LV_IMAGE_DECLARE(material_sunny);
LV_IMAGE_DECLARE(material_thunderstorm);

static lv_obj_t *temp_high_label;
static lv_obj_t *temp_low_label;

static lv_obj_t *weather_icon;

weather_icon_t active_icon = WEATHER_ICON_NONE;

static bool first_update = false;

void lv_helper_weather_create() {
  lv_obj_t *screen = lv_scr_act();

  // FIXME: This should be configurable or find a better way to mount displays
  int32_t y_offset = 5;

  weather_icon = lv_image_create(screen);
  lv_obj_align(weather_icon, LV_ALIGN_CENTER, -66 + 2, 0 + y_offset);
  lv_image_set_antialias(weather_icon, false);

  temp_high_label = lv_label_create(screen);
  lv_obj_add_style(temp_high_label, &medium_number_style, LV_PART_MAIN);
  lv_obj_align(temp_high_label, LV_ALIGN_LEFT_MID, 132 + 4, -30 + y_offset);

  temp_low_label = lv_label_create(screen);
  lv_obj_add_style(temp_low_label, &small_number_style, LV_PART_MAIN);
  lv_obj_align(temp_low_label, LV_ALIGN_LEFT_MID, 132 + 4, 30 + y_offset);

  lv_helper_aqi_alert_create(false);

  first_update = true;
}

void lv_helper_weather_update(lv_helper_view_mode_weather_data_t *data) {
  static char temp_high_str[6];
  static char temp_low_str[6];
  if (data->temp_high < 100 && data->temp_high > -10) {
    snprintf(temp_high_str, sizeof(temp_high_str), "%" PRId16 "°",
             data->temp_high);
  } else {
    // Temp is already 3 chars so omit the degrees symbol for this large font
    snprintf(temp_high_str, sizeof(temp_high_str), "%" PRId16, data->temp_high);
  }
  snprintf(temp_low_str, sizeof(temp_low_str), "%" PRId16 "°", data->temp_low);
  set_text_if_changed(temp_high_label, temp_high_str);
  set_text_if_changed(temp_low_label, temp_low_str);

  if (active_icon != data->icon || first_update) {
    switch (data->icon) {
    case WEATHER_ICON_NONE:
    case WEATHER_ICON_MAX:
      break;
    case WEATHER_ICON_CLEAR_NIGHT:
      lv_image_set_src(weather_icon, &material_clear_night);
      break;
    case WEATHER_ICON_CLOUD:
      lv_image_set_src(weather_icon, &material_cloud);
      break;
    case WEATHER_ICON_FOGGY:
      lv_image_set_src(weather_icon, &material_foggy);
      break;
    case WEATHER_ICON_PARTLY_CLOUDY_DAY:
      lv_image_set_src(weather_icon, &material_partly_cloudy_day);
      break;
    case WEATHER_ICON_PARTLY_CLOUDY_NIGHT:
      lv_image_set_src(weather_icon, &material_partly_cloudy_night);
      break;
    case WEATHER_ICON_RAINY:
      lv_image_set_src(weather_icon, &material_rainy);
      break;
    case WEATHER_ICON_SNOWY:
      lv_image_set_src(weather_icon, &material_snowy);
      break;
    case WEATHER_ICON_SUNNY:
      lv_image_set_src(weather_icon, &material_sunny);
      break;
    case WEATHER_ICON_THUNDERSTORM:
      lv_image_set_src(weather_icon, &material_thunderstorm);
      break;
    }

    active_icon = data->icon;
  }

  lv_helper_aqi_alert_update(&lv_helper_aqi_alert_data);

  first_update = false;
}
