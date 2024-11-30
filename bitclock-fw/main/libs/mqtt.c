#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt.h"

static const char *TAG = "mqtt";

esp_mqtt_client_handle_t client;
static bool configs_published = false;

static const char *co2_unique_id = "bitclock_co2";
static const char *nox_unique_id = "bitclock_nox";
static const char *voc_unique_id = "bitclock_voc";
static const char *temp_unique_id = "bitclock_temp";
static const char *humidity_unique_id = "bitclock_humidity";
static char config_message[300];
static char config_topic[100];
static char state_topic[100];

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
    snprintf(config_message, sizeof(config_message), "{\"name\": \"%s\",\"unique_id\": \"%s\", \"device_class\": \"%s\", \"unit_of_measurement\": \"%s\",\"state_class\": \"%s\",\"platform\": \"%s\"}", name, unique_id, device_class, unit_of_measurement, state_class, platform);
    snprintf(config_topic, sizeof(config_topic), "homeassistant/sensor/%s/config", unique_id);
    return publish_message(config_topic, config_message, true);
}

bool publish_homeassistant_sensor(const char * unique_id, const char * state_message) {
    snprintf(state_topic, sizeof(state_topic), "homeassistant/sensor/%s/state", unique_id);
    return publish_message(state_topic, state_message, true);
}

void send_homeassistant_mqtt_sensor_data(const char *mqtt_url, aqi_data_t *aqi_data) {
  if (client == NULL) {
    ESP_LOGI(TAG, "MQTT client not initialized");
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = mqtt_url,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_LOGI(TAG, "MQTT client initialized");
    if (esp_mqtt_client_start(client) != ESP_OK) {
      ESP_LOGI(TAG, "MQTT client failed to initialize");
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

  char buffer[32];
  bool success = true;

  // Array of sensor data pairs
  struct {
      const char* unique_id;
      const char* format;
      double value;
  } sensors[] = {
      {co2_unique_id, "%u", (double)aqi_data->co2_ppm},
      {nox_unique_id, "%ld", (double)aqi_data->nox_index},
      {voc_unique_id, "%ld", (double)aqi_data->voc_index},
      {temp_unique_id, "%.2f", aqi_data->temp_celsius},
      {humidity_unique_id, "%.2f", aqi_data->humidity}
  };

  // Loop through each sensor
  for (size_t i = 0; i < sizeof(sensors)/sizeof(sensors[0]); i++) {
      snprintf(buffer, sizeof(buffer), sensors[i].format, sensors[i].value);
      success = success && publish_homeassistant_sensor(sensors[i].unique_id, buffer);
  }

  if (success) {
    ESP_LOGI(TAG, "Homeassistant sensor data published");    
  } else {
    ESP_LOGE(TAG, "Homeassistant sensor data failed to publish");
  }
}
