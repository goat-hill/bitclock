#include "mqtt.h"

#include "esp_log.h"
#include "scd4x.h"
#include "sgp4x.h"
#include "sht4x.h"
#include "libs/nvs.h"
#include "tasks.h"
#include "tasks/wifi.h"
#include <string.h>

#ifdef MQTT_TASK_ENABLED

static const char *TAG = "mqtt-task";

StaticTask_t mqttTaskBuffer;
StackType_t mqttTaskStack[MQTT_STACK_SIZE];

EventGroupHandle_t mqtt_event_group_handle;
static StaticEventGroup_t mqtt_event_group;

void get_aqi_data(aqi_data_t *aqi_data) {
  aqi_data->temp_celsius = sht4x_current_temp_celsius();
  aqi_data->humidity = sht4x_current_relative_humidity();
  aqi_data->co2_ppm = scd4x_current_co2_ppm();
  aqi_data->nox_index = sgp41_current_nox_index();
  aqi_data->voc_index = sgp4x_current_voc_index();
}

void mqtt_task_run(void *pvParameters) {
  mqtt_event_group_handle = xEventGroupCreateStatic(&mqtt_event_group);
  TickType_t delay_time = portMAX_DELAY;

  while (1) {
    xEventGroupWaitBits(mqtt_event_group_handle,
                        MQTT_EVENT_WIFI_CONNECTED |
                            MQTT_EVENT_URL_CHANGED,
                        true,  // clear = true
                        false, // wait for all = false
                        delay_time);

    const char *mqtt_url = bitclock_nvs_get_mqtt_url();

    if (mqtt_url == NULL || strlen(mqtt_url) < 1) {
      ESP_LOGI(TAG, "No mqtt_url so ignoring");
      delay_time = pdMS_TO_TICKS(1000 * 60); // 60 seconds
      continue;
    } else {
      ESP_LOGI(TAG, "MQTT URL: %s", mqtt_url);
    }

    if (!bitclock_wifi_is_started()) {
      ESP_LOGW(TAG, "WiFi not started");
      delay_time = pdMS_TO_TICKS(1000 * 5); // 5 seconds
      continue;
    }

    ESP_LOGI(TAG, "MQTT task running");

    aqi_data_t aqi_data = {0};
    get_aqi_data(&aqi_data);

    if (xSemaphoreTake(wifi_req_semaphore, 0)) {
      send_homeassistant_mqtt_sensor_data(mqtt_url, &aqi_data);

      xSemaphoreGive(wifi_req_semaphore);
    }

    // Send every 60 seconds
    delay_time = pdMS_TO_TICKS(1000 * 60); 
  }
}

#endif
