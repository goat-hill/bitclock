#include "sharp_display.h"

#if DISPLAY_TYPE == DISPLAY_TYPE_SHARP

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "lvgl/lv_helper.h"
#include "lvgl/lvgl.h"
#include "lvgl/views/aqi_grid.h"
#include "scd4x.h"
#include "sgp4x.h"
#include "sht4x.h"

LV_FONT_DECLARE(sharp_display_font);

StaticTask_t dispTaskBuffer;
StackType_t dispTaskStack[DISP_STACK_SIZE];
static TickType_t last_frame_inversion_tick = 0;
static bool frame_inversion_enabled = false;

static spi_device_handle_t spi;

static const char *TAG = "disp";

#define DISPLAY_WIDTH 400
#define DISPLAY_HEIGHT 240

// 1/10 screen size
#define BUFFER_PIXEL_COUNT (40 * DISPLAY_WIDTH)

static lv_display_t *display;
static lv_color_t lvgl_pixel_buffer[BUFFER_PIXEL_COUNT];

#define DISP_ROW_LENGTH (2 + DISPLAY_WIDTH / 8)
#define DISP_BYTES 2 + DISP_ROW_LENGTH *DISPLAY_HEIGHT
#define DISP_ROW_ADDR_INDEX(y) (1 + DISP_ROW_LENGTH * y)
#define DISP_PIXEL_INDEX(x, y) (DISP_ROW_ADDR_INDEX(y) + 1 + x / 8)
#define DISP_PIXEL_BIT_MASK(x) (1 << (7 - (x % 8)))

// 12482 bytes for full screen flush.
//
// 1 - Mode select (3 clk + 5 dummy)
// For up 1-240 lines...
//    1 - Gate line address select (8 clk)
//    50 - Data write (400 clk)
//    1 - Data transfer (8 clk dummy)
// 1 - Data transfer (8 clk dummy)
DMA_ATTR static uint8_t disp_transfer_buffer[DISP_BYTES];

typedef enum {
  DISP_FLAG_MODE_UPDATE = 0x1 << 7,
  DISP_FLAG_FRAME_INVERSION = 0x1 << 6,
  DISP_FLAG_ALL_CLEAR = 0x1 << 5
} DispFlags;

static uint8_t reverse_bits(uint8_t value) {
  value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
  value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
  value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
  return value;
}

static void spi_init() {
  gpio_reset_pin(PIN_NUM_SPI_CLK);
  gpio_reset_pin(PIN_NUM_SPI_MOSI);
  gpio_reset_pin(PIN_NUM_SPI_CS_DISP);

  gpio_set_direction(PIN_NUM_SPI_CLK, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_SPI_MOSI, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_SPI_CS_DISP, GPIO_MODE_OUTPUT);

  gpio_set_level(PIN_NUM_SPI_CLK, 0);
  gpio_set_level(PIN_NUM_SPI_MOSI, 0);
  gpio_set_level(PIN_NUM_SPI_CS_DISP, 0);

  spi_bus_config_t buscfg = {.mosi_io_num = PIN_NUM_SPI_MOSI,
                             .sclk_io_num = PIN_NUM_SPI_CLK,
                             .miso_io_num = -1,
                             .quadwp_io_num = -1,
                             .quadhd_io_num = -1,
                             .max_transfer_sz = DISP_BYTES};

  spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

  // TODO: Any performance boost with half duplex?
  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = 2 * 1000 * 1000, // 1 MHz
      .mode = 0,
      .flags =
          SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE | SPI_DEVICE_POSITIVE_CS,
      .spics_io_num = PIN_NUM_SPI_CS_DISP,
      .queue_size = 1,
  };
  spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
}

static void disp_buffer_init() {
  for (uint16_t i = 0; i < DISPLAY_HEIGHT; i++) {
    disp_transfer_buffer[DISP_ROW_ADDR_INDEX(i)] = reverse_bits(i + 1);
  }
}

static void disp_send_rows(uint8_t start_row, uint8_t end_row) {
  esp_err_t ret;

  size_t msg_length = 2 + DISP_ROW_LENGTH * (1 + end_row - start_row);
  size_t msg_offset = DISP_ROW_LENGTH * start_row;

  disp_transfer_buffer[msg_offset] =
      DISP_FLAG_MODE_UPDATE |
      ((frame_inversion_enabled) ? DISP_FLAG_FRAME_INVERSION : 0x00);

  spi_transaction_t t = {
      .flags = SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL,
      .length = msg_length * 8,
      .tx_buffer = disp_transfer_buffer + msg_offset,
      .user = (void *)1,
  };

  ret = spi_device_polling_transmit(spi, &t);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI transmit send_rows: %u", ret);
    return;
  }
}

static void lvgl_disp_flush(lv_display_t *disp, const lv_area_t *area,
                            uint8_t *px_map) {
  size_t pixel_byte;
  uint8_t pixel_mask;
  int32_t x, y;
  uint8_t white_color;
  white_color = lv_color_to_u16(lv_color_white());

  uint8_t *pixel_color = px_map;
  for (y = area->y1; y <= area->y2; y++) {
    for (x = area->x1; x <= area->x2; x++) {
      bool pixel_black = *pixel_color != white_color;
      pixel_byte = DISP_PIXEL_INDEX(x, y);
      pixel_mask = DISP_PIXEL_BIT_MASK(x);
      disp_transfer_buffer[pixel_byte] =
          (disp_transfer_buffer[pixel_byte] & ~pixel_mask) |
          (((pixel_black) ? 0x00 : 0xFF) & pixel_mask);
      pixel_color++;
    }
  }
  disp_send_rows(area->y1, area->y2);
  lv_disp_flush_ready(disp);
}

void disp_all_clear() {
  esp_err_t ret;
  uint8_t data = 0;

  data |= DISP_FLAG_ALL_CLEAR;
  if (frame_inversion_enabled) {
    data |= DISP_FLAG_FRAME_INVERSION;
  }

  static spi_transaction_t t = {
      .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL,
      .length = 8,
      .user = (void *)1,
  };
  t.tx_data[0] = data;
  ret = spi_device_polling_transmit(spi, &t);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI transmit all_clear: %u", ret);
    return;
  }
}

void disp_empty_update() {
  // Send after flipping frame_inversion_enabled
  esp_err_t ret;
  uint8_t data = 0;

  if (frame_inversion_enabled) {
    data |= DISP_FLAG_FRAME_INVERSION;
  }

  static spi_transaction_t t = {
      .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL,
      .length = 8,
      .user = (void *)1,
  };
  t.tx_data[0] = data;
  ret = spi_device_polling_transmit(spi, &t);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI transmit empty_update: %u", ret);
    return;
  }
}

static void disp_log(lv_log_level_t level, const char *buf) {
  ESP_LOGW(TAG, "%s", buf);
}

static void check_for_frame_inversion() {
  static TickType_t current_tick;
  current_tick = xTaskGetTickCount();
  // Datasheet recommends inverting display comms at least once per second.
  if (TICKS_ELAPSED(current_tick, last_frame_inversion_tick) >
      pdMS_TO_TICKS(1000)) {
    frame_inversion_enabled = !frame_inversion_enabled;
    last_frame_inversion_tick = current_tick;
  }
}

void disp_task_run(void *pvParameters) {
  spi_init();
  lv_init();
  disp_buffer_init();

  display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);

  lv_display_set_buffers(display, lvgl_pixel_buffer, NULL,
                         sizeof(lvgl_pixel_buffer),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(display, lvgl_disp_flush);
  lv_log_register_print_cb(disp_log);
  lv_tick_set_cb(lv_tick_cb);

  lv_theme_t *theme = lv_theme_mono_init(display, false, &sharp_display_font);
  lv_display_set_theme(display, theme);

  disp_all_clear();

  lv_helper_styles_init();

   lv_helper_set_view_mode(VIEW_MODE_AQI_GRID);

  while (1) {
    lv_helper_view_mode_aqi_data.temp_celsius = sht4x_current_temp_celsius();
    lv_helper_view_mode_aqi_data.humidity_pct = sht4x_current_relative_humidity();
    lv_helper_view_mode_aqi_data.co2_ppm = scd4x_current_co2_ppm();
    lv_helper_view_mode_aqi_data.voc_index = sgp4x_current_voc_index();
    lv_helper_view_mode_aqi_data.nox_index = sgp41_current_nox_index();

    lv_helper_update();

    uint32_t ms_til_next = lv_timer_handler();
    vTaskDelay(ms_til_next);
    check_for_frame_inversion();
    disp_empty_update();
  }
}

#endif
