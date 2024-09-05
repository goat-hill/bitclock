#include <stdbool.h>
#include <time.h>
#include "lv_helper.h"

typedef struct {
  time_t curtime;
  bool hour24;
} lv_helper_view_mode_clock_data_t;
extern lv_helper_view_mode_clock_data_t lv_helper_view_mode_clock_data;

void lv_helper_clock_create();
void lv_helper_clock_update(lv_helper_view_mode_clock_data_t *data);
