#include "freertos/FreeRTOS.h"
#include <time.h>

#define TICKS_ELAPSED(current_ticks, prev_ticks)                               \
  (current_ticks < prev_ticks ? portMAX_DELAY : current_ticks - prev_ticks)

typedef enum {
  VIEW_MODE_NONE = 0,
  VIEW_MODE_LOGO,
  VIEW_MODE_PASSKEY,
  VIEW_MODE_CLOCK,
  VIEW_MODE_AQI_GRID,
  VIEW_MODE_WEATHER,
  VIEW_MODE_MAX // Always keep this as the last element
} view_mode_t;

uint32_t lv_tick_cb();

void lv_helper_styles_init();
void lv_helper_set_view_mode(view_mode_t view_mode);
void lv_helper_update();

// Functions shared between weather and clock views
lv_obj_t* create_label(lv_obj_t *parent, const lv_style_t *style, lv_align_t align, int x_offset, int y_offset);
void format_time_and_date(struct tm *timeinfo, bool hour24, char *time_label_str, size_t time_label_size, char *time_sublabel_str, size_t time_sublabel_size);
