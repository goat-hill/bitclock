#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt.h"

static const char *TAG = "mqtt";

esp_mqtt_client_handle_t client;
static bool configs_published = false;

const char *co2_unique_id = "bitclock_co2";
const char *nox_unique_id = "bitclock_nox";
const char *voc_unique_id = "bitclock_voc";
const char *temp_unique_id = "bitclock_temp";
const char *humidity_unique_id = "bitclock_humidity";


bool publish_message(const char *topic, const char *message, bool retain) {
    bool success = esp_mqtt_client_publish(client, topic, message, 0, 1, retain) >= 0;
    if (success) {
      ESP_LOGI(TAG, "MQTT publish successful, topic=%s message=%s", topic, message);
    } else { 
      ESP_LOGE(TAG, "MQTT publish failed, topic=%s message=%s", topic, message);
    }
    return success;
}


bool publish_homeassistant_config(const char * name, const char * unique_id, const char * device_class, const char * unit_of_measurement, const char * state_class, const char * platform) {
    char * config_message = malloc(300);
    sprintf(config_message, "{\"name\": \"%s\",\"unique_id\": \"%s\", \"device_class\": \"%s\", \"unit_of_measurement\": \"%s\",\"state_class\": \"%s\",\"platform\": \"%s\"}", name, unique_id, device_class, unit_of_measurement, state_class, platform);
    char * config_topic = malloc(100);
    sprintf(config_topic, "homeassistant/sensor/%s/config", unique_id);
    return publish_message(config_topic, config_message, true);
}


bool publish_homeassistant_sensor(const char * unique_id, const char * state_message) {
    char * state_topic = malloc(100);
    sprintf(state_topic, "homeassistant/sensor/%s/state", unique_id);
    return publish_message(state_topic, state_message, true);
}


void send_homeassistant_mqtt_sensor_data(const char *mqtt_url, aqi_data_t *aqi_data) {
  if (client == NULL) {
    ESP_LOGE(TAG, "MQTT client not initialized");
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = mqtt_url,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_LOGI(TAG, "MQTT client initialized");
    if (esp_mqtt_client_start(client) != ESP_OK) {
      ESP_LOGE(TAG, "MQTT client failed to initialize");
      return;
    }
  }

  if (!configs_published) {
    configs_published = true &&
      publish_homeassistant_config("Bitclock CO2", co2_unique_id, "carbon_dioxide", "ppm", "measurement", "sensor") &&
      publish_homeassistant_config("Bitclock NOx", nox_unique_id, "nitrogen_dioxide", "ppm", "measurement", "sensor") &&
      publish_homeassistant_config("Bitclock VOC", voc_unique_id, "volatile_organic_compounds", "ppm", "measurement", "sensor") &&
      publish_homeassistant_config("Bitclock Temperature", temp_unique_id, "temperature", "°C", "measurement", "sensor") &&
      publish_homeassistant_config("Bitclock Humidity", humidity_unique_id, "humidity", "%%", "measurement", "sensor")
    ;
    if (!configs_published) {
      ESP_LOGE(TAG, "Homeassistant configs failed to publish");
      return;
    } else {
      ESP_LOGI(TAG, "Homeassistant configs published");
    }
  }

  ESP_LOGI(TAG, "Sending MQTT sensor data");
  
  char * co2_buffer = malloc(100);
  sprintf(co2_buffer, "%u", aqi_data->co2_ppm);
  char * nox_buffer = malloc(100);
  sprintf(nox_buffer, "%ld", aqi_data->nox_index);
  char * voc_buffer = malloc(100);
  sprintf(voc_buffer, "%ld", aqi_data->voc_index);
  char * temp_buffer = malloc(100);
  sprintf(temp_buffer, "%f", aqi_data->temp_celsius);
  char * humidity_buffer = malloc(100);
  sprintf(humidity_buffer, "%f", aqi_data->humidity);

  bool success = true &&
    publish_homeassistant_sensor(co2_unique_id, co2_buffer) &&
    publish_homeassistant_sensor(nox_unique_id, nox_buffer) &&
    publish_homeassistant_sensor(voc_unique_id, voc_buffer) &&
    publish_homeassistant_sensor(temp_unique_id, temp_buffer) &&
    publish_homeassistant_sensor(humidity_unique_id, humidity_buffer)
  ;
  if (success) {
    ESP_LOGI(TAG, "Homeassistant sensor data published");    
  } else {
    ESP_LOGE(TAG, "Homeassistant sensor data failed to publish");
  }
}
