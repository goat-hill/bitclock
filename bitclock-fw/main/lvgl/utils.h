#include "lvgl/lvgl.h"
#include <stdint.h>

uint16_t round_to_nearest_50(uint16_t value);

void str_to_upper(char *str);

void set_text_if_changed(lv_obj_t *label, const char *new_value);

void lv_helper_clean();
