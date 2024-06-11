//
// Driver for E2271KS0C1 / eScreen_EPD_271_KS_0C
//

#include "eink_display.h"

#if DISPLAY_TYPE == DISPLAY_TYPE_EINK

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "libs/aqi_alert.h"
#include "libs/nvs.h"
#include "libs/sensor_utils.h"
#include "lvgl/lv_helper.h"
#include "lvgl/lvgl.h"
#include "lvgl/views/aqi_alert.h"
#include "lvgl/views/aqi_grid.h"
#include "lvgl/views/clock.h"
#include "lvgl/views/logo.h"
#include "lvgl/views/passkey.h"
#include "lvgl/views/weather.h"
#include "scd4x.h"
#include "sgp4x.h"
#include "sht4x.h"
#include "tasks/ble.h"
#include "tasks/weather.h"
#include "tasks/wifi.h"
#include <math.h>
#include <string.h>
#include <time.h>

StaticTask_t einkTaskBuffer;
StackType_t einkTaskStack[EINK_STACK_SIZE];

#define INVERT_COLORS 0

static const char *TAG = "eink";

spi_device_handle_t spi;

#define DISPLAY_WIDTH 176
#define DISPLAY_HEIGHT 264

#define LVGL_WIDTH DISPLAY_HEIGHT
#define LVGL_HEIGHT DISPLAY_WIDTH

#define BUFFER_PIXEL_COUNT (40 * DISPLAY_WIDTH)

static lv_display_t *display;
static uint8_t lvgl_pixel_buffer[BUFFER_PIXEL_COUNT];

static const uint8_t kDispAddrInputTemp = 0xe5;
static const uint8_t kDispAddrActiveTemp = 0xe0;
static const uint8_t kDispAddrPanelSettings = 0x00;
static const uint8_t kDispAddrOtp = 0xa2;
static const uint8_t kDispAddrFrame1 = 0x10;
static const uint8_t kDispAddrFrame2 = 0x13;
static const uint8_t kDispAddrPowerOn = 0x04;
static const uint8_t kDispAddrRefresh = 0x12;
static const uint8_t kDispAddrPowerOff = 0x02;
static const uint8_t kDispAddrVcomDataInterval = 0x50;

static const uint16_t kDispOtpBank0Addr = 0x0fb4;
static const uint16_t kDispOtpBank1Addr = 0x1fb4;

static const uint8_t kDispVcomCDI[] = {0x07};
static const uint8_t kDispVcomBorderSetting[] = {0x27};

static const uint8_t kDispDataPanelSettingsSoftReset[] = {0x0e};

#define DISP_ROW_LENGTH (DISPLAY_WIDTH / 8)
#define DISP_BYTES (DISP_ROW_LENGTH * DISPLAY_HEIGHT)
#define DISP_ROW_BEGIN(y) (DISP_ROW_LENGTH * y)
#define DISP_PIXEL_INDEX(x, y) (DISP_ROW_BEGIN(y) + x / 8)
#define DISP_PIXEL_BIT_MASK(x) (1 << (7 - (x % 8)))

DMA_ATTR static uint8_t disp_transfer_buffer[DISP_BYTES];
DMA_ATTR static uint8_t disp_transfer_buffer_prev[DISP_BYTES] = {0};
bool display_sync_needed = true;

// Display configuration read from on-device OTP memory
static uint8_t psr_data[2];

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
                             .miso_io_num = -1,
                             .sclk_io_num = PIN_NUM_SPI_CLK,
                             .quadwp_io_num = -1,
                             .quadhd_io_num = -1,
                             .max_transfer_sz = DISP_BYTES};

  spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
}

static esp_err_t eink_spi_add_device(uint32_t clock_speed) {
  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = clock_speed,
      .mode = 0,
      .flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE,
      .spics_io_num = PIN_NUM_SPI_CS_DISP,
      .queue_size = 1,
  };
  return spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
}

static esp_err_t eink_spi_remove_device() { return spi_bus_remove_device(spi); }

static void disp_spi_send_cmd(spi_device_handle_t spi, const uint8_t addr,
                              const uint8_t *data, int data_len) {
  // Note that for uint8_t, most significant bit is sent first
  esp_err_t ret;
  static spi_transaction_t t = {.flags = SPI_TRANS_USE_TXDATA |
                                         SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL,
                                .length = 8,
                                .user = (void *)1};
  t.tx_data[0] = addr;

  gpio_set_level(PIN_NUM_DISP_DC, 0);
  ret = spi_device_polling_transmit(spi, &t);
  gpio_set_level(PIN_NUM_DISP_DC, 1);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI transmit: %u", ret);
    return;
  }

  for (uint16_t i = 0; i < data_len; i++) {
    t.tx_data[0] = data[i];
    ret = spi_device_polling_transmit(spi, &t);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to SPI transmit: %u", ret);
      return;
    }
  }

  ESP_LOGD(TAG, "Successfully sent %u bytes to SPI command to addr: 0x%02x",
           data_len, addr);
}

static void disp_spi_send_data_byte(spi_device_handle_t spi,
                                    const uint8_t data) {
  esp_err_t ret;
  static spi_transaction_t t = {.flags = SPI_TRANS_USE_TXDATA |
                                         SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL,
                                .length = 8,
                                .user = (void *)1};
  t.tx_data[0] = data;
  ret = spi_device_polling_transmit(spi, &t);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI transmit data byte: %u", ret);
  }
}

static void disp_send_frame(uint8_t *tx_buffer) {
  esp_err_t ret;
  static spi_transaction_t t = {.flags = SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL,
                                .length = DISP_BYTES * 8,
                                .user = (void *)1};
  t.tx_buffer = tx_buffer;
  ret = spi_device_polling_transmit(spi, &t);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI transmit frame: %u", ret);
  }
}

static void eink_gpio_init() {
  gpio_reset_pin(PIN_NUM_DISP_DC);
  gpio_reset_pin(PIN_NUM_DISP_RST);
  gpio_reset_pin(PIN_NUM_DISP_BUSY);

  gpio_set_direction(PIN_NUM_DISP_DC, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_DISP_RST, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_DISP_BUSY, GPIO_MODE_INPUT);

  gpio_set_level(PIN_NUM_DISP_DC, 0);
  gpio_set_level(PIN_NUM_DISP_RST, 0);
}

static esp_err_t eink_read_otp() {
  // Ensure max clock speed of 4.16 MHz
  esp_err_t ret;
  spi_transaction_t t = {.flags = SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL |
                                  SPI_TRANS_USE_TXDATA,
                         .length = 8,
                         .user = (void *)1};
  t.tx_data[0] = kDispAddrOtp;

  gpio_set_level(PIN_NUM_DISP_DC, 0);
  ret = spi_device_polling_transmit(spi, &t);
  gpio_set_level(PIN_NUM_DISP_DC, 1);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI transmit otp addr: %u", ret);
    return ret;
  }

  spi_transaction_t t_rx = {.flags = SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL |
                                     SPI_TRANS_USE_RXDATA,
                            .rxlength = 8,
                            .user = (void *)1};
  // Read dummy byte
  ret = spi_device_polling_transmit(spi, &t_rx);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI read otp dummy: %u", ret);
    return ret;
  }

  // Read address 0
  ret = spi_device_polling_transmit(spi, &t_rx);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI read address 0: %u", ret);
    return ret;
  }

  // If address 0 is 0xa5, we should use bank 0. Otherwise bank 1.
  uint16_t bank_addr =
      (t_rx.rx_data[0] == 0xa5) ? kDispOtpBank0Addr : kDispOtpBank1Addr;

  // Read up to kDispOtpBank0Addr
  for (uint16_t i = 1; i < bank_addr; i++) {
    ret = spi_device_polling_transmit(spi, &t_rx);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to SPI read otp byte: %u", ret);
      return ret;
    }
  }

  // Read PSR[0]
  ret = spi_device_polling_transmit(spi, &t_rx);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI read PSR[0]: %u", ret);
    return ret;
  }
  psr_data[0] = t_rx.rx_data[0];
  ESP_LOGI(TAG, "Successfully read PSR[0] @ 0x%04x: 0x%02x", bank_addr,
           psr_data[0]);

  // Read PSR[1]
  ret = spi_device_polling_transmit(spi, &t_rx);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to SPI read PSR[1]: %u", ret);
    return ret;
  }
  psr_data[1] = t_rx.rx_data[0];
  ESP_LOGI(TAG, "Successfully read PSR[1] @ 0x%04x: 0x%02x", bank_addr + 1,
           psr_data[1]);

  return ESP_OK;
}

static void disp_block_while_busy() {
  int isDone = gpio_get_level(PIN_NUM_DISP_BUSY);
  while (!isDone) {
    // TODO: Add timeout, block via ISR + task notification instead of CPU poll
    vTaskDelay(pdMS_TO_TICKS(500));
    isDone = gpio_get_level(PIN_NUM_DISP_BUSY);
  }
}

static void eink_power_on() {
  // Power on COG driver
  ESP_LOGI(TAG, "Resetting display...");
  gpio_set_level(PIN_NUM_DISP_RST, 0);
  vTaskDelay(pdMS_TO_TICKS(50)); // Suggested 5ms
  gpio_set_level(PIN_NUM_DISP_RST, 1);
  vTaskDelay(pdMS_TO_TICKS(50)); // Suggested 5ms
  gpio_set_level(PIN_NUM_DISP_RST, 0);
  vTaskDelay(pdMS_TO_TICKS(50)); // Suggested 10ms
  gpio_set_level(PIN_NUM_DISP_RST, 1);
  vTaskDelay(pdMS_TO_TICKS(50)); // Suggested 20ms

  disp_block_while_busy();

  ESP_LOGI(TAG, "Setting display settings...");
  disp_spi_send_cmd(spi, kDispAddrPanelSettings,
                    kDispDataPanelSettingsSoftReset,
                    sizeof(kDispDataPanelSettingsSoftReset));
  vTaskDelay(pdMS_TO_TICKS(50)); // Suggested 5ms
}

static void eink_send_config(bool fast_update) {
  // Set environment temperature and PSR
  //
  // SPI(0xe5, TSSET)
  // TSSET is temperature in celsius. High bit is positive/negative.
  //  If negative, 2's complement of temperature value
  //  +25 -> 0x19
  //  +5  -> 0x05
  //  -5  -> 0xFB
  //
  while (!sht4x_current_temp_available()) {
    ESP_LOGI(TAG, "Waiting for temp to init display.");
    vTaskDelay(pdMS_TO_TICKS(50));
  }
  float temp = sht4x_current_temp_celsius();
  uint8_t temperature_data[1] = {(uint8_t)(temp >= 0 ? temp : 0xff - temp + 1)};
  ESP_LOGD(TAG, "config temp: %f %u 0x%02x", temp, temperature_data[0],
           temperature_data[0]);
  if (fast_update) {
    temperature_data[0] |= 0x40;
  }
  disp_spi_send_cmd(spi, kDispAddrInputTemp, temperature_data,
                    sizeof(temperature_data));

  // Active temp
  disp_spi_send_cmd(spi, kDispAddrActiveTemp, (uint8_t[]){0x02}, 1);
  // Panel settings for 2.71"

  if (fast_update) {
    uint8_t fast_update_psr[2];
    fast_update_psr[0] = psr_data[0] | 0x10;
    fast_update_psr[1] = psr_data[1] | 0x02;
    disp_spi_send_cmd(spi, kDispAddrPanelSettings, fast_update_psr,
                      sizeof(fast_update_psr));
    disp_spi_send_cmd(spi, kDispAddrVcomDataInterval, kDispVcomCDI,
                      sizeof(kDispVcomCDI));
  } else {
    disp_spi_send_cmd(spi, kDispAddrPanelSettings, psr_data, sizeof(psr_data));
  }
}

static void eink_send_image_data() {
  uint16_t i;

  // Frame 1: Pixels (1 == Black)
  ESP_LOGD(TAG, "Sending frame 1...");
  disp_spi_send_cmd(spi, kDispAddrFrame1, NULL, 0);
  disp_send_frame(disp_transfer_buffer);

  // Frame 2: Zeros
  ESP_LOGD(TAG, "Sending frame 2...");
  disp_spi_send_cmd(spi, kDispAddrFrame2, NULL, 0);
  for (i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT / 8; i++) {
    disp_spi_send_data_byte(spi, 0x00);
  }
  ESP_LOGD(TAG, "Sent %u bytes", i);

  memcpy(disp_transfer_buffer_prev, disp_transfer_buffer,
         sizeof(disp_transfer_buffer_prev));
}

static void eink_send_image_data_fast() {
  // 2x border setting
  ESP_LOGD(TAG, "Sending border setting...");
  disp_spi_send_cmd(spi, kDispAddrVcomDataInterval, kDispVcomBorderSetting,
                    sizeof(kDispVcomBorderSetting));
  disp_spi_send_cmd(spi, kDispAddrVcomDataInterval, kDispVcomBorderSetting,
                    sizeof(kDispVcomBorderSetting));

  // Previous frame
  ESP_LOGD(TAG, "Sending frame 1 fast...");
  disp_spi_send_cmd(spi, kDispAddrFrame1, NULL, 0);
  disp_send_frame(disp_transfer_buffer_prev);

  // Next frame
  ESP_LOGD(TAG, "Sending frame 2 fast...");
  disp_spi_send_cmd(spi, kDispAddrFrame2, NULL, 0);
  disp_send_frame(disp_transfer_buffer);

  // 2x border setting
  ESP_LOGD(TAG, "Sending border setting CDI...");
  disp_spi_send_cmd(spi, kDispAddrVcomDataInterval, kDispVcomCDI,
                    sizeof(kDispVcomCDI));
  disp_spi_send_cmd(spi, kDispAddrVcomDataInterval, kDispVcomCDI,
                    sizeof(kDispVcomCDI));

  memcpy(disp_transfer_buffer_prev, disp_transfer_buffer,
         sizeof(disp_transfer_buffer_prev));
}

static void eink_update() {
  disp_block_while_busy();

  // 2x according to datasheet...
  ESP_LOGI(TAG, "Powering on display...");
  disp_spi_send_cmd(spi, kDispAddrPowerOn, NULL, 0);
  disp_spi_send_cmd(spi, kDispAddrPowerOn, NULL, 0);

  disp_block_while_busy();

  // 2x according to datasheet...
  ESP_LOGI(TAG, "Refreshing...");
  disp_spi_send_cmd(spi, kDispAddrRefresh, NULL, 0);
  disp_spi_send_cmd(spi, kDispAddrRefresh, NULL, 0);

  disp_block_while_busy();
}

static void eink_power_off() {
  // 2x according to datasheet...
  ESP_LOGI(TAG, "Powering off display...");
  disp_spi_send_cmd(spi, kDispAddrPowerOff, NULL, 0);
  disp_spi_send_cmd(spi, kDispAddrPowerOff, NULL, 0);

  disp_block_while_busy();
}

static void lvgl_disp_flush(lv_display_t *disp, const lv_area_t *area,
                            uint8_t *px_map) {
  ESP_LOGD(TAG, "LVGL flush [%ld, %ld]", area->y1, area->y2);
  size_t pixel_byte;
  uint8_t pixel_mask;
  int32_t x, y, disp_x, disp_y;
  uint8_t white_color;
  white_color = 0xFF;

  uint8_t *pixel_color = (uint8_t *)px_map;

  for (y = area->y1; y <= area->y2; y++) {
    for (x = area->x1; x <= area->x2; x++) {
      bool pixel_black = (*pixel_color != white_color);

      // 50% dithering for all colors between white and black.
      if (*pixel_color > 0x00 && *pixel_color < 0xFF) {
        if ((x % 2 == 0) ^ (y % 2 == 0)) {
          pixel_black = false;
        }
      }

      pixel_black ^= INVERT_COLORS;

#if DISPLAY_ROTATION == 90
      // Rotate 90 degrees.
      disp_x = y;
      disp_y = DISPLAY_HEIGHT - (1 + x);
#elif DISPLAY_ROTATION == -90
      // Rotate -90 degrees
      disp_x = DISPLAY_WIDTH - (1 + y);
      disp_y = x;
#else
      disp_x = x;
      disp_y = y;
#endif

      pixel_byte = DISP_PIXEL_INDEX(disp_x, disp_y);
      pixel_mask = DISP_PIXEL_BIT_MASK(disp_x);
      disp_transfer_buffer[pixel_byte] =
          (disp_transfer_buffer[pixel_byte] & ~pixel_mask) |
          (((pixel_black) ? 0xFF : 0x00) & pixel_mask);
      pixel_color++;
    }
  }
  display_sync_needed = true;
  lv_disp_flush_ready(disp);
}

void eink_task_run(void *pvParameters) {
  lv_init();
  eink_gpio_init();
  spi_init();

  //
  // OTP read
  //
  ESP_ERROR_CHECK(eink_spi_add_device(4 * 1000 * 1000)); // 4 MHz read OTP

  eink_power_on();
  ESP_ERROR_CHECK(eink_read_otp());
  ESP_ERROR_CHECK(eink_spi_remove_device());

  // LVGL init
  ESP_ERROR_CHECK(eink_spi_add_device(10 * 1000 * 1000)); // 10 MHz write
  display = lv_display_create(LVGL_WIDTH, LVGL_HEIGHT);

  lv_display_set_buffers(display, lvgl_pixel_buffer, NULL,
                         sizeof(lvgl_pixel_buffer),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(display, lvgl_disp_flush);
  lv_tick_set_cb(lv_tick_cb);
  lv_theme_t *theme = lv_theme_mono_init(display, false, &sublabel);
  lv_display_set_theme(display, theme);

  uint16_t frame_count = 0;
  lv_helper_styles_init();

  // Short boot logo
  // lv_helper_set_view_mode(VIEW_MODE_LOGO);
  // lv_helper_update();
  // lv_timer_handler();
  // eink_send_config(false);
  // eink_send_image_data();
  // eink_update();
  // eink_power_off();
  // frame_count++;
  // vTaskDelay(pdMS_TO_TICKS(1500));

  aqi_data_t aqi_data = {0};
  view_mode_t view_mode = VIEW_MODE_CLOCK;

  while (1) {
    bool fast_update = frame_count % 120 != 0;

    time(&lv_helper_view_mode_clock_data.curtime);

    bool celsius_mode =
        bitclock_nvs_get_temp_unit() == BITCLOCK_NVS_TEMP_UNIT_VAL_CELSIUS;
    lv_helper_view_mode_clock_data.hour24 =
        bitclock_nvs_get_clock_format() == BITCLOCK_NVS_CLOCK_FORMAT_VAL_24HR;

    aqi_data.temp_celsius = sht4x_current_temp_celsius();
    aqi_data.humidity = sht4x_current_relative_humidity();
    aqi_data.co2_ppm = scd4x_current_co2_ppm();
    aqi_data.voc_index = sgp4x_current_voc_index();
    aqi_data.nox_index = sgp41_current_nox_index();

    lv_helper_view_mode_aqi_data.temp_celsius = aqi_data.temp_celsius;
    lv_helper_view_mode_aqi_data.co2_ppm = aqi_data.co2_ppm;
    lv_helper_view_mode_aqi_data.voc_index = aqi_data.voc_index;
    lv_helper_view_mode_aqi_data.nox_index = aqi_data.nox_index;

    lv_helper_aqi_alert_data.alert_reason = aqi_alerts_check(&aqi_data);

    lv_helper_view_mode_passkey_data.passkey = ble_active_passkey();

    lv_helper_view_mode_weather_data.icon = latest_weather.icon;
    lv_helper_view_mode_weather_data.temp_high =
        celsius_mode
            ? (int16_t)roundf(fahrenheit_to_celsius(latest_weather.temp_max))
            : latest_weather.temp_max;
    lv_helper_view_mode_weather_data.temp_low =
        celsius_mode
            ? (int16_t)roundf(fahrenheit_to_celsius(latest_weather.temp_min))
            : latest_weather.temp_min;

    if (lv_helper_view_mode_passkey_data.passkey != PASSKEY_NOT_ACTIVE) {
      lv_helper_set_view_mode(VIEW_MODE_PASSKEY);
    } else {
      bitclock_nvs_app_selection_val_t app_selection =
          bitclock_nvs_get_app_selection();
      if (app_selection == BITCLOCK_NVS_APP_SELECTION_VAL_CLOCK) {
        view_mode = VIEW_MODE_CLOCK;
      } else if (app_selection == BITCLOCK_NVS_APP_SELECTION_VAL_WEATHER) {
        view_mode = VIEW_MODE_WEATHER;
      }
      lv_helper_set_view_mode(view_mode);
    }

    lv_helper_update();
    lv_timer_handler();

    if (display_sync_needed) {
      eink_send_config(fast_update);
      if (fast_update) {
        eink_send_image_data_fast();
      } else {
        eink_send_image_data();
      }
      eink_update();
      eink_power_off();
      display_sync_needed = false;

      frame_count++;
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

#endif
