#include <stdint.h>
#include <time.h>

#include "lvgl/lvgl.h"
#include "lvgl/styles.h"
#include "lvgl/utils.h"

// Functions shared between weather and clock views
lv_obj_t* create_label(lv_obj_t *parent, const lv_style_t *style, lv_align_t align, int x_offset, int y_offset);
void format_time_and_date(struct tm *timeinfo, bool hour24, char *time_label_str, size_t time_label_size, char *time_sublabel_str, size_t time_sublabel_size);
