#include "wifi.h"

#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_wifi.h"
#include "libs/http_buffer.h"
#include "tasks/ble.h"
#include "tasks/weather.h"

static const char *TAG = "wifi";

EventGroupHandle_t wifi_event_group_handle;
static StaticEventGroup_t wifi_event_group;

StaticTask_t wifiTaskBuffer;
StackType_t wifiTaskStack[WIFI_STACK_SIZE];

SemaphoreHandle_t wifi_req_semaphore;
StaticSemaphore_t wifi_req_semaphore_mutex_buffer;

QueueHandle_t wifiCredentialsQueueHandle;
static StaticQueue_t wifiCredentialsQueue;
uint8_t wifiCredentialsQueueStorageArea[1 * sizeof(WiFiCredentials_t)];
static bool wifi_started = false;
static bool wifi_has_ip_val = false;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
  // We avoid any esp_wifi_*() calls directly here so we do
  // those in the context of the Wi-Fi task which claims the wifi_req_semaphore
  // in order to avoid race conditions with other tasks.
  if (event_base == WIFI_EVENT) {
    switch (event_id) {
    case WIFI_EVENT_STA_START:
      xEventGroupSetBits(wifi_event_group_handle, WIFI_READY_TO_CONNECT_EVENT);
      break;
    case WIFI_EVENT_STA_DISCONNECTED:
      ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
      xEventGroupSetBits(wifi_event_group_handle, WIFI_READY_TO_CONNECT_EVENT);
      break;
    default:
      break;
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
      ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
      wifi_has_ip_val = true;
      ESP_LOGI(TAG, "Connected with IP Address:" IPSTR,
               IP2STR(&event->ip_info.ip));
      xEventGroupSetBits(weather_event_group_handle,
                         WEATHER_EVENT_WIFI_CONNECTED);
      ble_notify_wifi_status_update();
      break;
    case IP_EVENT_STA_LOST_IP:
      wifi_has_ip_val = false;
      ble_notify_wifi_status_update();
      break;
    default:
      break;
    }
  }
}

static void wifi_init() {
  // Initialize TCP/IP
  ESP_ERROR_CHECK(esp_netif_init());

  // Create system event handlers
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_event_group_handle = xEventGroupCreateStatic(&wifi_event_group);

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &event_handler, NULL));

  // Initialize wifi
  esp_netif_create_default_wifi_sta();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_req_semaphore =
      xSemaphoreCreateMutexStatic(&wifi_req_semaphore_mutex_buffer);
}

bool bitclock_wifi_is_started() { return wifi_started; }

bool bitclock_wifi_has_ip() { return wifi_has_ip_val; }

void wifi_task_run(void *pvParameters) {
  // Ensure nvs_flash_init() called in main()
  wifiCredentialsQueueHandle = xQueueCreateStatic(
      1, sizeof(WiFiCredentials_t), wifiCredentialsQueueStorageArea,
      &wifiCredentialsQueue);

  wifi_init();

  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  esp_netif_sntp_init(&config);

  WiFiCredentials_t wifiCreds;
  wifi_config_t wifi_cfg;

  EventBits_t wifi_event_bits = 0;

  while (1) {
    // Get the request lock before we do anything else to block other networking
    // tasks
    if (!xSemaphoreTake(wifi_req_semaphore, portMAX_DELAY)) {
      continue;
    }

    // Signal from event task that it is time to connect.
    if (wifi_event_bits & WIFI_READY_TO_CONNECT_EVENT) {
      esp_wifi_connect();
    }

    // Signal from Bluetooth BLE GATT that we have new credentials
    if (xQueueReceive(wifiCredentialsQueueHandle, &wifiCreds, 0) == pdPASS) {
      if (wifi_started) {
        esp_wifi_stop();
        wifi_started = false;
        wifi_has_ip_val = false;
        ble_notify_wifi_status_update();
      }

      // Update settings
      wifi_config_t new_cfg = {0};
      strncpy((char *)new_cfg.sta.ssid, wifiCreds.ssid,
              sizeof(new_cfg.sta.ssid));
      strncpy((char *)new_cfg.sta.password, wifiCreds.password,
              sizeof(new_cfg.sta.password));
      ESP_LOGI(TAG, "New wifi creds %s:%s", new_cfg.sta.ssid,
               new_cfg.sta.password);
      ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &new_cfg));
    }

    // Start wifi if we haven't already, and have creds
    if (!wifi_started &&
        esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) == ESP_OK) {
      wifi_started = true;
      ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");
      ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
      ESP_ERROR_CHECK(esp_wifi_start());
      ble_notify_wifi_status_update();
    }

    if (!wifi_has_ip_val) {
      ESP_LOGI(TAG, "Waiting for wifi to connect...");
    }

    // Release the semaphore to let other tasks continue with wifi networking
    xSemaphoreGive(wifi_req_semaphore);

    // Sleep until we have credentials or need to connect
    wifi_event_bits = xEventGroupWaitBits(wifi_event_group_handle,
                                          WIFI_CREDENTIALS_UPDATED_EVENT |
                                              WIFI_READY_TO_CONNECT_EVENT,
                                          true,  // clear on exit
                                          false, // wait for all
                                          portMAX_DELAY);
  }
}
