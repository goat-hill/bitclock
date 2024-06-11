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
