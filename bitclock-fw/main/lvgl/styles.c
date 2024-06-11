#include "styles.h"

lv_style_t large_number_style;
lv_style_t medium_number_style;
lv_style_t small_number_style;
lv_style_t sublabel_style;

LV_FONT_DECLARE(large_number);
LV_FONT_DECLARE(medium_number);
LV_FONT_DECLARE(small_number);

void lv_helper_styles_init() {
  lv_style_init(&large_number_style);
  lv_style_set_text_font(&large_number_style, &large_number);

  lv_style_init(&sublabel_style);
  lv_style_set_text_font(&sublabel_style, &sublabel);

  lv_style_init(&medium_number_style);
  lv_style_set_text_font(&medium_number_style, &medium_number);

  lv_style_init(&small_number_style);
  lv_style_set_text_font(&small_number_style, &small_number);
  lv_style_set_text_opa(&small_number_style, LV_OPA_50);
}
