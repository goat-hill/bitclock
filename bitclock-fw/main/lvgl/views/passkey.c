#include "passkey.h"
#include "lvgl/lvgl.h"
#include "lvgl/styles.h"
#include "lvgl/utils.h"

lv_helper_view_mode_passkey_data_t lv_helper_view_mode_passkey_data;

static lv_obj_t *passkey_label;
static lv_obj_t *passkey_sublabel;

void lv_helper_passkey_create() {
  passkey_label = lv_label_create(lv_screen_active());

  lv_obj_add_style(passkey_label, &medium_number_style, LV_PART_MAIN);
  lv_obj_align(passkey_label, LV_ALIGN_CENTER, 0, 0);

  passkey_sublabel = lv_label_create(lv_screen_active());

  lv_obj_add_style(passkey_sublabel, &sublabel_style, LV_PART_MAIN);
  lv_obj_align(passkey_sublabel, LV_ALIGN_CENTER, 0, 60);
  lv_label_set_text(passkey_sublabel, "BLUETOOTH PAIRING CODE");
}

void lv_helper_passkey_update(lv_helper_view_mode_passkey_data_t *data) {
  static char formatted_passkey[7]; // 6 digits + null terminator
  snprintf(formatted_passkey, sizeof(formatted_passkey), "%06" PRIu32,
           data->passkey);
  set_text_if_changed(passkey_label, formatted_passkey);
}
