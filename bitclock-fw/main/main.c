#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "libs/i2c.h"
#include "libs/nvs.h"
#include "libs/ota.h"
#include "nvs_flash.h"
#include "pins.h"

#include "tasks/ble.h"
#include "tasks/eink_display.h"
#include "tasks/led.h"
#include "tasks/scd4x.h"
#include "tasks/sgp4x.h"
#include "tasks/sharp_display.h"
#include "tasks/sht4x.h"
#include "tasks/tasks.h"
#include "tasks/mqtt.h"
#include "tasks/weather.h"
#include "tasks/wifi.h"

void app_main(void) {
  ESP_ERROR_CHECK(bitclock_ota_on_boot());
  i2c_bus_init();

  // Used by wifi, ble tasks
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    /* NVS partition was truncated
     * and needs to be erased */
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
  }
  ESP_ERROR_CHECK(ret);
  ESP_ERROR_CHECK(bitclock_nvs_init());

  const char *tz = bitclock_nvs_get_tz();
  if (tz != NULL) {
    setenv("TZ", tz, 1);
    tzset();
  } else {
    setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
    tzset();
  }

#ifdef LED_TASK_ENABLED
  ledTask = xTaskCreateStatic(led_task_run, "led",
                              LED_STACK_SIZE,    // ulStackDepth
                              (void *)1,         // pvParamters
                              LED_TASK_PRIORITY, // uxPriority
                              ledTaskStack,      // puxStackBuffer
                              &ledTaskBuffer     // pxTaskBuffer
  );
#endif

#ifdef DISP_TASK_ENABLED
#if DISPLAY_TYPE == DISPLAY_TYPE_SHARP
  dispTask = xTaskCreateStatic(disp_task_run, "display",
                               DISP_STACK_SIZE,    // ulStackDepth
                               (void *)1,          // pvParamters
                               DISP_TASK_PRIORITY, // uxPriority
                               dispTaskStack,      // puxStackBuffer
                               &dispTaskBuffer     // pxTaskBuffer
  );
#elif DISPLAY_TYPE == DISPLAY_TYPE_EINK
  einkTask = xTaskCreateStatic(eink_task_run, "eink",
                               EINK_STACK_SIZE,    // ulStackDepth
                               (void *)1,          // pvParamters
                               EINK_TASK_PRIORITY, // uxPriority
                               einkTaskStack,      // puxStackBuffer
                               &einkTaskBuffer     // pxTaskBuffer
  );
#endif
#endif // DISP_TASK_ENABLED

#ifdef SHT4X_TASK_ENABLED
  sht4xTask = xTaskCreateStatic(sht4x_task_run, "sht4x",
                                SHT4X_STACK_SIZE,    // ulStackDepth
                                (void *)1,           // pvParamters
                                SHT4X_TASK_PRIORITY, // uxPriority
                                sht4xTaskStack,      // puxStackBuffer
                                &sht4xTaskBuffer     // pxTaskBuffer
  );
#endif

#ifdef SCD4X_TASK_ENABLED
  scd4xTask = xTaskCreateStatic(scd4x_task_run, "scd4x",
                                SCD4X_STACK_SIZE,    // ulStackDepth
                                (void *)1,           // pvParamters
                                SCD4X_TASK_PRIORITY, // uxPriority
                                scd4xTaskStack,      // puxStackBuffer
                                &scd4xTaskBuffer     // pxTaskBuffer
  );
#endif

#ifdef SGP4X_TASK_ENABLED
  sgp4xTask = xTaskCreateStatic(sgp4x_task_run, "scd4x",
                                SGP4X_STACK_SIZE,    // ulStackDepth
                                (void *)1,           // pvParamters
                                SGP4X_TASK_PRIORITY, // uxPriority
                                sgp4xTaskStack,      // puxStackBuffer
                                &sgp4xTaskBuffer     // pxTaskBuffer
  );
#endif

#ifdef WIFI_TASK_ENABLED
  wifiTask = xTaskCreateStatic(wifi_task_run, "wifi",
                               WIFI_STACK_SIZE,    // ulStackDepth
                               (void *)1,          // pvParamters
                               WIFI_TASK_PRIORITY, // uxPriority
                               wifiTaskStack,      // puxStackBuffer
                               &wifiTaskBuffer     // pxTaskBuffer
  );
#endif

#ifdef BLE_TASK_ENABLED
  bleTask = xTaskCreateStatic(ble_task_run, "ble",
                              BLE_STACK_SIZE,    // ulStackDepth
                              (void *)1,         // pvParamters
                              BLE_TASK_PRIORITY, // uxPriority
                              bleTaskStack,      // puxStackBuffer
                              &bleTaskBuffer     // pxTaskBuffer
  );
#endif

#ifdef WEATHER_TASK_ENABLED
  weatherTask = xTaskCreateStatic(weather_task_run, "weather",
                                  WEATHER_STACK_SIZE,    // ulStackDepth
                                  (void *)1,             // pvParamters
                                  WEATHER_TASK_PRIORITY, // uxPriority
                                  weatherTaskStack,      // puxStackBuffer
                                  &weatherTaskBuffer     // pxTaskBuffer
  );
#endif

#ifdef MQTT_TASK_ENABLED
  mqttTask = xTaskCreateStatic(mqtt_task_run, "mqtt",
                               MQTT_STACK_SIZE,    // ulStackDepth
                               (void *)1,          // pvParamters
                               MQTT_TASK_PRIORITY, // uxPriority
                               mqttTaskStack,      // puxStackBuffer
                               &mqttTaskBuffer     // pxTaskBuffer
  );
#endif

  assert(gpio_install_isr_service(0) == ESP_OK);

  vTaskDelete(NULL);
}
