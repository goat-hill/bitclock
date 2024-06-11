#include "logo.h"
#include "lvgl/lvgl.h"

LV_IMAGE_DECLARE(logo);

void lv_helper_logo_create() {
  lv_obj_t *screen = lv_scr_act();

  lv_obj_t *logo_obj = lv_image_create(screen);
  lv_obj_align(logo_obj, LV_ALIGN_CENTER, 0, 0);
  lv_image_set_antialias(logo_obj, false);

  lv_image_set_src(logo_obj, &logo);
}

void lv_helper_logo_update() {}
