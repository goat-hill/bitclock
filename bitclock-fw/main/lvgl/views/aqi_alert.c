#include "aqi_alert.h"

#include "lvgl/lvgl.h"
#include "lvgl/styles.h"
#include "lvgl/utils.h"

LV_IMAGE_DECLARE(material_mode_fan);

lv_obj_t *fan_img;
lv_obj_t *alert_label;

lv_helper_aqi_alert_data_t lv_helper_aqi_alert_data = {0};

void lv_helper_aqi_alert_create(bool align_right) {
  lv_obj_t *screen = lv_scr_act();
  // 10 padding
  // 20 image
  // 6 padding

  fan_img = lv_image_create(screen);
  lv_obj_align(fan_img, align_right ? LV_ALIGN_TOP_RIGHT : LV_ALIGN_TOP_LEFT,
               10 * (align_right ? -1 : 1), 14);
  lv_image_set_antialias(fan_img, false);

  lv_image_set_src(fan_img, &material_mode_fan);

  alert_label = lv_label_create(screen);
  lv_obj_add_style(alert_label, &sublabel_style, LV_PART_MAIN);
  lv_obj_align(alert_label,
               align_right ? LV_ALIGN_TOP_RIGHT : LV_ALIGN_TOP_LEFT,
               36 * (align_right ? -1 : 1), 17);
}

void lv_helper_aqi_alert_update(lv_helper_aqi_alert_data_t *data) {
  const char *label_str = NULL;
  switch (data->alert_reason) {
  case AQI_ALERT_TEMP_HIGH:
    label_str = "HOT";
    break;
  case AQI_ALERT_TEMP_LOW:
    label_str = "COLD";
    break;
  case AQI_ALERT_HUMDIITY_HIGH:
    label_str = "HUMID";
    break;
  case AQI_ALERT_HUMDIITY_LOW:
    label_str = "DRY";
    break;
  case AQI_ALERT_CO2_HIGH:
    label_str = "CO₂";
    break;
  case AQI_ALERT_NOX_HIGH:
    label_str = "NOₓ";
    break;
  case AQI_ALERT_VOC_HIGH:
    label_str = "VOC";
    break;
  case AQI_ALERT_NONE:
  case AQI_ALERT_COUNT:
    label_str = "";
    break;
  }
  set_text_if_changed(alert_label, label_str);

  void *image_src = NULL;
  if (data->alert_reason != AQI_ALERT_NONE) {
    image_src = &material_mode_fan;
  }

  // Only set if changed to avoid extra render
  if (image_src != lv_image_get_src(fan_img)) {
    lv_image_set_src(fan_img, image_src);
  }
}
