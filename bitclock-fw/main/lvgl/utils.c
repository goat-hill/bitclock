#include "utils.h"
#include <ctype.h>
#include <string.h>

uint16_t round_to_nearest_50(uint16_t value) {
  return ((value + 25) / 50) * 50;
}

void str_to_upper(char *str) {
  while (*str) {
    *str = toupper((unsigned char)*str);
    str++;
  }
}

void set_text_if_changed(lv_obj_t *label, const char *new_value) {
  // Reduce LVGL renders for unchanged text
  static char *existing_text;
  existing_text = lv_label_get_text(label);
  if (existing_text == NULL || strcmp(new_value, existing_text) != 0) {
    lv_label_set_text(label, new_value);
  }
}

void lv_helper_clean() { lv_obj_clean(lv_screen_active()); }
