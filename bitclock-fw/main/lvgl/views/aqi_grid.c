#include "aqi_grid.h"

#include "libs/sensor_utils.h"
#include "lvgl/lvgl.h"
#include "lvgl/utils.h"
#include "tasks/tasks.h"
#include <math.h>

#ifdef DISPLAY_TYPE
#if DISPLAY_TYPE == DISPLAY_TYPE_SHARP
LV_FONT_DECLARE(sharp_display_font);
#endif
#endif

lv_helper_view_mode_aqi_data_t lv_helper_view_mode_aqi_data;

static lv_obj_t *temperature_label;
static lv_obj_t *humidity_label;
static lv_obj_t *co2_label;
static lv_obj_t *voc_label;

static lv_obj_t *create_label(lv_obj_t *parent, const char *text,
                              lv_align_t align, lv_grid_align_t grid_align,
                              int32_t col, int32_t row) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_grid_cell(label, grid_align, col, 1, grid_align, row, 1);
  lv_obj_set_style_text_align(label, align, 0);
#if DISPLAY_TYPE == DISPLAY_TYPE_SHARP
  lv_obj_set_style_text_font(label, &sharp_display_font, 0);
#endif
  return label;
}

static void init_grid_layout(lv_obj_t *parent) {
  static lv_coord_t column_dsc[] = {100, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT,
                                 LV_GRID_CONTENT, LV_GRID_CONTENT,
                                 LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(parent, column_dsc, row_dsc);
  lv_obj_set_size(parent, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_pad_all(parent, 10, 0);
  lv_obj_set_style_pad_row(parent, 15, 0);
  lv_obj_set_layout(parent, LV_LAYOUT_GRID);
}

void lv_helper_aqi_grid_create() {
  lv_obj_t *screen = lv_scr_act();

  // Initialize the grid layout
  init_grid_layout(screen);

  // Create labels and value labels for each parameter
  create_label(screen, "TEMP", LV_TEXT_ALIGN_LEFT, LV_GRID_ALIGN_START, 0, 0);
  create_label(screen, "HUMIDITY", LV_TEXT_ALIGN_LEFT, LV_GRID_ALIGN_START, 0,
               1);
  create_label(screen, "CO₂ PPM", LV_TEXT_ALIGN_LEFT, LV_GRID_ALIGN_START, 0,
               2);
  create_label(screen, "VOC", LV_TEXT_ALIGN_LEFT, LV_GRID_ALIGN_START, 0, 3);

  temperature_label =
      create_label(screen, "", LV_TEXT_ALIGN_RIGHT, LV_GRID_ALIGN_END, 1, 0);
  humidity_label =
      create_label(screen, "", LV_TEXT_ALIGN_RIGHT, LV_GRID_ALIGN_END, 1, 1);
  co2_label =
      create_label(screen, "", LV_TEXT_ALIGN_RIGHT, LV_GRID_ALIGN_END, 1, 2);
  voc_label =
      create_label(screen, "", LV_TEXT_ALIGN_RIGHT, LV_GRID_ALIGN_END, 1, 3);
}

void lv_helper_aqi_grid_update(lv_helper_view_mode_aqi_data_t *data) {
  static char temp_str[10];
  snprintf(temp_str, sizeof(temp_str), "%.0f°",
           roundf(celsius_to_fahrenheit(data->temp_celsius)));
  set_text_if_changed(temperature_label, temp_str);

  static char humidity_str[10];
  snprintf(humidity_str, sizeof(humidity_str), "%u%%", data->humidity_pct);
  set_text_if_changed(humidity_label, humidity_str);

  static char co2_str[10];
  snprintf(co2_str, sizeof(co2_str), "%" PRIu16, data->co2_ppm);
  set_text_if_changed(co2_label, co2_str);

  static char voc_str[10];
  snprintf(voc_str, sizeof(voc_str), "%" PRId32, data->voc_index);
  set_text_if_changed(voc_label, voc_str);
}
