#include "gatt.h"

#include "esp_log.h"
#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "libs/nvs.h"
#include "libs/ota.h"
#include "services/ans/ble_svc_ans.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
// FIXME: Move these task dependencies to libs
#include "tasks/mqtt.h"
#include "tasks/scd4x.h"
#include "tasks/sgp4x.h"
#include "tasks/sht4x.h"
#include "tasks/weather.h"
#include "tasks/wifi.h"

static const char *TAG = "gatt";

// ee2f3aa0-4ed8-71bd-f147-b2bc37195232
const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0x32, 0x52, 0x19, 0x37, 0xbc, 0xb2, 0x47, 0xf1, 0xbd, 0x71,
                     0xd8, 0x4e, 0xa0, 0x3a, 0x2f, 0xec);

/* A characteristic that can be subscribed to */
#define WIFI_LENGTH                                                            \
  32 + 64 + 2 // 32 char ssid + 64 char password + separator + termination
static uint8_t gatt_svr_chr_wifi_val[WIFI_LENGTH];
uint16_t gatt_svr_chr_wifi_val_handle;

//
// Characteristics
//

#define WIFI_IS_STARTED BIT0
#define WIFI_HAS_IP BIT1

// Wifi
// a5313036-a66e-e990-2147-49a7b1557044
static const ble_uuid128_t gatt_svr_chr_wifi_uuid =
    BLE_UUID128_INIT(0x44, 0x70, 0x55, 0xb1, 0xa7, 0x49, 0x47, 0x21, 0x90, 0xe9,
                     0x6e, 0xa6, 0x36, 0x30, 0x31, 0xa5);

// OTA Size
// c839db31-c8e7-4c59-938d-c4b46e7bc2a4
static const ble_uuid128_t gatt_svr_chr_ota_size_uuid =
    BLE_UUID128_INIT(0xa4, 0xc2, 0x7b, 0x6e, 0xb4, 0xc4, 0x8d, 0x93, 0x59, 0x4c,
                     0xe7, 0xc8, 0x31, 0xdb, 0x39, 0xc8);
static uint32_t gatt_svr_chr_ota_size_val;
uint16_t gatt_svr_chr_ota_size_val_handle;

// OTA data
// ed900bcc-2ed5-40b0-b33a-fe7483f254c7
static const ble_uuid128_t gatt_svr_chr_ota_data_uuid =
    BLE_UUID128_INIT(0xc7, 0x54, 0xf2, 0x83, 0x74, 0xfe, 0x3a, 0xb3, 0xb0, 0x40,
                     0xd5, 0x2e, 0xcc, 0x0b, 0x90, 0xed);
static uint8_t gatt_svr_chr_ota_data_val[512];
uint16_t gatt_svr_chr_ota_data_val_handle;

static const ble_uuid16_t gatt_svr_chr_temperature_uuid =
    BLE_UUID16_INIT(0x2a6e);
uint16_t gatt_svr_chr_temperature_val_handle;
static const ble_uuid16_t gatt_svr_chr_humidity_uuid = BLE_UUID16_INIT(0x2a6f);
uint16_t gatt_svr_chr_humidity_val_handle;
static const ble_uuid16_t gatt_svr_chr_co2_uuid = BLE_UUID16_INIT(0x2b8c);
uint16_t gatt_svr_chr_co2_val_handle;
static const ble_uuid16_t gatt_svr_chr_voc_index_uuid = BLE_UUID16_INIT(0x2be7);
uint16_t gatt_svr_chr_voc_index_val_handle;
static const ble_uuid16_t gatt_svr_chr_nox_index_uuid = BLE_UUID16_INIT(0x2bd2);
uint16_t gatt_svr_chr_nox_index_val_handle;

static const ble_uuid16_t gatt_svr_chr_timezone_uuid = BLE_UUID16_INIT(0x2a0e);
uint16_t gatt_svr_chr_timezone_val_handle;
char gatt_svr_chr_timezone_val[64];

static const ble_uuid16_t gatt_svr_chr_temperature_unit_uuid =
    BLE_UUID16_INIT(0x2a1d);
uint16_t gatt_svr_chr_temperature_unit_val_handle;
bitclock_nvs_temp_unit_val_t gatt_svr_chr_temperature_unit_val;

static const ble_uuid128_t gatt_svr_chr_app_selection_uuid =
    BLE_UUID128_INIT(0xea, 0x49, 0xe2, 0x46, 0x73, 0xf7, 0xa2, 0x9a, 0xeb, 0x43,
                     0xee, 0x0a, 0xe2, 0xc5, 0x04, 0x35);
uint16_t gatt_svr_chr_app_selection_val_handle;
bitclock_nvs_app_selection_val_t gatt_svr_chr_app_selection_val;

static const ble_uuid128_t gatt_svr_chr_wifi_status_uuid =
    BLE_UUID128_INIT(0x5d, 0xb6, 0x11, 0x87, 0xe8, 0xc9, 0xa6, 0x8d, 0x26, 0x4d,
                     0xb2, 0xd9, 0xc3, 0xb3, 0xf4, 0x32);
uint16_t gatt_svr_chr_wifi_status_val_handle;

static const ble_uuid128_t gatt_svr_chr_clock_format_uuid =
    BLE_UUID128_INIT(0xd6, 0x45, 0xc4, 0x59, 0x87, 0x2a, 0xe5, 0xa0, 0x65, 0x44,
                     0x1d, 0x87, 0x38, 0xa5, 0x23, 0x83);
uint16_t gatt_svr_chr_clock_format_val_handle;
bitclock_nvs_clock_format_val_t gatt_svr_chr_clock_format_val;

static const ble_uuid128_t gatt_svr_chr_weather_path_uuid =
    BLE_UUID128_INIT(0x81, 0x1f, 0x8c, 0xd4, 0x26, 0x00, 0x80, 0xb8, 0xe2, 0x47,
                     0x66, 0x95, 0x1d, 0xee, 0xef, 0x32);
uint16_t gatt_svr_chr_weather_path_val_handle;
char gatt_svr_chr_weather_path_val[64];

static const ble_uuid128_t gatt_svr_chr_mqtt_url_uuid =
    BLE_UUID128_INIT(0x2a, 0xb4, 0x8c, 0x15, 0x93, 0x4d, 0x11, 0xec, 0xb9, 0x0a,
                     0x80, 0x27, 0x48, 0xc2, 0x7b, 0x9e);
uint16_t gatt_svr_chr_mqtt_url_val_handle;
char gatt_svr_chr_mqtt_url_val[64];

static int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    .uuid = &gatt_svr_chr_timezone_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                             BLE_GATT_CHR_F_WRITE_AUTHEN | BLE_GATT_CHR_F_READ |
                             BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN,
                    .val_handle = &gatt_svr_chr_timezone_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_temperature_unit_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                             BLE_GATT_CHR_F_WRITE_AUTHEN | BLE_GATT_CHR_F_READ |
                             BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN,
                    .val_handle = &gatt_svr_chr_temperature_unit_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_app_selection_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                             BLE_GATT_CHR_F_WRITE_AUTHEN | BLE_GATT_CHR_F_READ |
                             BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN,
                    .val_handle = &gatt_svr_chr_app_selection_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_clock_format_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                             BLE_GATT_CHR_F_WRITE_AUTHEN | BLE_GATT_CHR_F_READ |
                             BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN,
                    .val_handle = &gatt_svr_chr_clock_format_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_weather_path_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                             BLE_GATT_CHR_F_WRITE_AUTHEN | BLE_GATT_CHR_F_READ |
                             BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN,
                    .val_handle = &gatt_svr_chr_weather_path_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_mqtt_url_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                             BLE_GATT_CHR_F_WRITE_AUTHEN | BLE_GATT_CHR_F_READ |
                             BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN,
                    .val_handle = &gatt_svr_chr_mqtt_url_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_wifi_status_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN |
                             BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                    .val_handle = &gatt_svr_chr_wifi_status_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_wifi_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                             BLE_GATT_CHR_F_WRITE_AUTHEN,
                    .val_handle = &gatt_svr_chr_wifi_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_ota_size_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_AUTHEN,
                    .val_handle = &gatt_svr_chr_ota_size_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_ota_data_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_AUTHEN,
                    .val_handle = &gatt_svr_chr_ota_data_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_temperature_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN |
                             BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                    .val_handle = &gatt_svr_chr_temperature_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_humidity_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN |
                             BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                    .val_handle = &gatt_svr_chr_humidity_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_co2_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN |
                             BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                    .val_handle = &gatt_svr_chr_co2_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_voc_index_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN |
                             BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                    .val_handle = &gatt_svr_chr_voc_index_val_handle,
                },
                {
                    .uuid = &gatt_svr_chr_nox_index_uuid.u,
                    .access_cb = gatt_svc_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                             BLE_GATT_CHR_F_READ_AUTHEN |
                             BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                    .val_handle = &gatt_svr_chr_nox_index_val_handle,
                },
                {
                    0, /* No more characteristics in this service. */
                }},
    },

    {
        0, /* No more services. */
    },
};

static int gatt_svr_write(struct os_mbuf *om, uint16_t min_len,
                          uint16_t max_len, void *dst, uint16_t *len) {
  uint16_t om_len;
  int rc;

  om_len = OS_MBUF_PKTLEN(om);
  if (om_len < min_len || om_len > max_len) {
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
  }

  rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
  if (rc != 0) {
    return BLE_ATT_ERR_UNLIKELY;
  }

  return 0;
}

void notify_temperature_update(struct ble_npl_event *ev) {
  gatt_chr_subscription_t *subscription = ble_npl_event_get_arg(ev);
  if (!subscription->cur_notify) {
    return;
  }
  ble_gatts_notify(subscription->conn_handle,
                   gatt_svr_chr_temperature_val_handle);
}

void notify_humidity_update(struct ble_npl_event *ev) {
  gatt_chr_subscription_t *subscription = ble_npl_event_get_arg(ev);
  if (!subscription->cur_notify) {
    return;
  }
  ble_gatts_notify(subscription->conn_handle, gatt_svr_chr_humidity_val_handle);
}

void notify_co2_update(struct ble_npl_event *ev) {
  gatt_chr_subscription_t *subscription = ble_npl_event_get_arg(ev);
  if (!subscription->cur_notify) {
    return;
  }
  ble_gatts_notify(subscription->conn_handle, gatt_svr_chr_co2_val_handle);
}

void notify_voc_index_update(struct ble_npl_event *ev) {
  gatt_chr_subscription_t *subscription = ble_npl_event_get_arg(ev);
  if (!subscription->cur_notify) {
    return;
  }
  ble_gatts_notify(subscription->conn_handle,
                   gatt_svr_chr_voc_index_val_handle);
}

void notify_nox_index_update(struct ble_npl_event *ev) {
  gatt_chr_subscription_t *subscription = ble_npl_event_get_arg(ev);
  if (!subscription->cur_notify) {
    return;
  }
  ble_gatts_notify(subscription->conn_handle,
                   gatt_svr_chr_nox_index_val_handle);
}

void notify_wifi_status_update(struct ble_npl_event *ev) {
  gatt_chr_subscription_t *subscription = ble_npl_event_get_arg(ev);
  if (!subscription->cur_notify) {
    return;
  }
  ble_gatts_notify(subscription->conn_handle,
                   gatt_svr_chr_wifi_status_val_handle);
}

static void enqueue_wifi_creds_update() {
  // This code might be terrible...
  WiFiCredentials_t creds = {0};
  const char *input = (const char *)gatt_svr_chr_wifi_val;
  const char *delimiter = "|";
  char *delimiter_pos;
  delimiter_pos = strchr(input, *delimiter);
  if (delimiter_pos) {
    size_t ssid_length = delimiter_pos - input;
    if (ssid_length >= sizeof(creds.ssid)) {
      ssid_length = sizeof(creds.ssid) - 1; // Truncate if necessary
    }
    strncpy(creds.ssid, input, ssid_length);
    creds.ssid[ssid_length] = '\0';

    const char *password_start = delimiter_pos + 1;
    strncpy(creds.password, password_start, sizeof(creds.password));
  } else {
    // If delimiter is not found, copy the whole input to SSID
    strncpy(creds.ssid, input, sizeof(creds.ssid));
  }

  ESP_LOGI(TAG, "SSID: %s added", creds.ssid);

  xQueueSend(wifiCredentialsQueueHandle, &creds,
             0 // Ignore if queue is full
  );
  // Poke the wifi taks to check the queue.
  xEventGroupSetBits(wifi_event_group_handle, WIFI_CREDENTIALS_UPDATED_EVENT);
}

static int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc;
  uint16_t copy_len;

  switch (ctxt->op) {
  case BLE_GATT_ACCESS_OP_READ_CHR:
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
      MODLOG_DFLT(DEBUG, "Characteristic read; conn_handle=%d attr_handle=%d\n",
                  conn_handle, attr_handle);
    } else {
      MODLOG_DFLT(DEBUG,
                  "Characteristic read by NimBLE stack; attr_handle=%d\n",
                  attr_handle);
    }

    if (attr_handle == gatt_svr_chr_timezone_val_handle) {
      const char *timezone = bitclock_nvs_get_tz();
      if (timezone == NULL) {
        timezone = "";
      }
      rc = os_mbuf_append(ctxt->om, timezone, strlen(timezone));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_temperature_unit_val_handle) {
      gatt_svr_chr_temperature_unit_val = bitclock_nvs_get_temp_unit();
      rc = os_mbuf_append(ctxt->om, &gatt_svr_chr_temperature_unit_val,
                          sizeof(gatt_svr_chr_temperature_unit_val));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_app_selection_val_handle) {
      gatt_svr_chr_app_selection_val = bitclock_nvs_get_app_selection();
      rc = os_mbuf_append(ctxt->om, &gatt_svr_chr_app_selection_val,
                          sizeof(gatt_svr_chr_app_selection_val));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_clock_format_val_handle) {
      gatt_svr_chr_clock_format_val = bitclock_nvs_get_clock_format();
      rc = os_mbuf_append(ctxt->om, &gatt_svr_chr_clock_format_val,
                          sizeof(gatt_svr_chr_clock_format_val));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_weather_path_val_handle) {
      const char *weather_path = bitclock_nvs_get_weather_path();
      if (weather_path == NULL) {
        weather_path = "";
      }
      rc = os_mbuf_append(ctxt->om, weather_path, strlen(weather_path));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_mqtt_url_val_handle) {
      const char *mqtt_url = bitclock_nvs_get_mqtt_url();
      if (mqtt_url == NULL) {
        mqtt_url = "";
      }
      rc = os_mbuf_append(ctxt->om, mqtt_url, strlen(mqtt_url));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_wifi_status_val_handle) {
      static uint8_t wifi_status;
      wifi_status = (bitclock_wifi_is_started() ? WIFI_IS_STARTED : 0) |
                    (bitclock_wifi_has_ip() ? WIFI_HAS_IP : 0);
      rc = os_mbuf_append(ctxt->om, &wifi_status, sizeof(wifi_status));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_temperature_val_handle) {
      static float temp_c;
      temp_c = sht4x_current_temp_celsius();
      rc = os_mbuf_append(ctxt->om, &temp_c, sizeof(temp_c));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_humidity_val_handle) {
      static float rel_h;
      rel_h = sht4x_current_relative_humidity();
      rc = os_mbuf_append(ctxt->om, &rel_h, sizeof(rel_h));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_co2_val_handle) {
      static uint16_t co2;
      co2 = scd4x_current_co2_ppm();
      rc = os_mbuf_append(ctxt->om, &co2, sizeof(co2));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_voc_index_val_handle) {
      static int32_t voc_index;
      voc_index = sgp4x_current_voc_index();
      rc = os_mbuf_append(ctxt->om, &voc_index, sizeof(voc_index));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == gatt_svr_chr_nox_index_val_handle) {
      static int32_t nox_index;
      nox_index = sgp41_current_nox_index();
      rc = os_mbuf_append(ctxt->om, &nox_index, sizeof(nox_index));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else {
      return BLE_ATT_ERR_READ_NOT_PERMITTED;
    }
    break;
  case BLE_GATT_ACCESS_OP_WRITE_CHR:
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
      MODLOG_DFLT(INFO, "Characteristic write; conn_handle=%d attr_handle=%d",
                  conn_handle, attr_handle);
    } else {
      MODLOG_DFLT(INFO, "Characteristic write by NimBLE stack; attr_handle=%d",
                  attr_handle);
    }

    if (attr_handle == gatt_svr_chr_timezone_val_handle) {
      rc = gatt_svr_write(ctxt->om, // data
                          1,        // min length
                          sizeof(gatt_svr_chr_timezone_val) -
                              1, // max length - 1 for null terminator
                          &gatt_svr_chr_timezone_val, // dest
                          &copy_len);
      if (rc == 0) {
        // Bluetooth doesn't add ending \0
        gatt_svr_chr_timezone_val[copy_len] = 0;
        bitclock_nvs_set_tz(gatt_svr_chr_timezone_val, copy_len + 1);
        setenv("TZ", gatt_svr_chr_timezone_val, 1);
        tzset();
      }
      return rc;
    } else if (attr_handle == gatt_svr_chr_weather_path_val_handle) {
      rc = gatt_svr_write(ctxt->om, // data
                          1,        // min length
                          sizeof(gatt_svr_chr_weather_path_val) -
                              1, // max length - 1 for null terminator
                          &gatt_svr_chr_weather_path_val, // dest
                          &copy_len);
      if (rc == 0) {
        // Bluetooth doesn't add ending \0
        gatt_svr_chr_weather_path_val[copy_len] = 0;
        bitclock_nvs_set_weather_path(gatt_svr_chr_weather_path_val,
                                      copy_len + 1);
      }
      xEventGroupSetBits(weather_event_group_handle,
                         WEATHER_EVENT_LOCATION_CHANGED);
      return rc;
    } else if (attr_handle == gatt_svr_chr_mqtt_url_val_handle) {
      rc = gatt_svr_write(ctxt->om, // data
                          1,        // min length
                          sizeof(gatt_svr_chr_mqtt_url_val) -
                              1, // max length - 1 for null terminator
                          &gatt_svr_chr_mqtt_url_val, // dest
                          &copy_len);
      if (rc == 0) {
        // Bluetooth doesn't add ending \0
        gatt_svr_chr_mqtt_url_val[copy_len] = 0;
        bitclock_nvs_set_mqtt_url(gatt_svr_chr_mqtt_url_val, copy_len + 1);
      }
      xEventGroupSetBits(weather_event_group_handle,
                         MQTT_EVENT_URL_CHANGED);
      return rc;
    } else if (attr_handle == gatt_svr_chr_temperature_unit_val_handle) {
      rc = gatt_svr_write(
          ctxt->om,                                  // data
          sizeof(gatt_svr_chr_temperature_unit_val), // min length
          sizeof(gatt_svr_chr_temperature_unit_val), // max length
          &gatt_svr_chr_temperature_unit_val,        // dest
          &copy_len                                  // length dest
      );
      if (rc == 0) {
        bitclock_nvs_set_temp_unit(gatt_svr_chr_temperature_unit_val);
      }
      return rc;
    } else if (attr_handle == gatt_svr_chr_app_selection_val_handle) {
      rc = gatt_svr_write(ctxt->om,                               // data
                          sizeof(gatt_svr_chr_app_selection_val), // min length
                          sizeof(gatt_svr_chr_app_selection_val), // max length
                          &gatt_svr_chr_app_selection_val,        // dest
                          &copy_len                               // length dest
      );
      if (rc == 0) {
        bitclock_nvs_set_app_selection(gatt_svr_chr_app_selection_val);
      }
      return rc;
    } else if (attr_handle == gatt_svr_chr_clock_format_val_handle) {
      rc = gatt_svr_write(ctxt->om,                              // data
                          sizeof(gatt_svr_chr_clock_format_val), // min length
                          sizeof(gatt_svr_chr_clock_format_val), // max length
                          &gatt_svr_chr_clock_format_val,        // dest
                          &copy_len                              // length dest
      );
      if (rc == 0) {
        bitclock_nvs_set_clock_format(gatt_svr_chr_clock_format_val);
      }
      return rc;
    } else if (attr_handle == gatt_svr_chr_wifi_val_handle) {
      rc = gatt_svr_write(ctxt->om,                      // data
                          1,                             // min length
                          sizeof(gatt_svr_chr_wifi_val), // max length
                          &gatt_svr_chr_wifi_val,        // dest
                          &copy_len                      // length dest
      );
      if (rc == 0) {
        // Bluetooth doesn't add ending \0
        if (copy_len < sizeof(gatt_svr_chr_wifi_val)) {
          gatt_svr_chr_wifi_val[copy_len] = 0;
        }
        enqueue_wifi_creds_update();
        ble_gatts_chr_updated(attr_handle);
      }
      return rc;
    } else if (attr_handle == gatt_svr_chr_ota_size_val_handle) {
      rc = gatt_svr_write(ctxt->om,                          // data
                          sizeof(gatt_svr_chr_ota_size_val), // min length
                          sizeof(gatt_svr_chr_ota_size_val), // max length
                          &gatt_svr_chr_ota_size_val,        // dest
                          &copy_len                          // length dest
      );
      if (rc == 0) {
        bitclock_ota_start(gatt_svr_chr_ota_size_val);
      }
      return rc;
    } else if (attr_handle == gatt_svr_chr_ota_data_val_handle) {
      rc = gatt_svr_write(ctxt->om,                          // data
                          1,                                 // min length
                          sizeof(gatt_svr_chr_ota_data_val), // max length
                          &gatt_svr_chr_ota_data_val,        // dest
                          &copy_len                          // length dest
      );
      if (rc == 0) {
        bitclock_ota_write(gatt_svr_chr_ota_data_val, copy_len);
        if (bitclock_ota_ready_to_update()) {
          ESP_LOGI(TAG, "Ready to switch images!");
          bitclock_ota_complete();
        }
      } else {
        return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
      }
      return rc;
    }
    break;
  }
  assert(0);
  return BLE_ATT_ERR_UNLIKELY;
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
  char buf[BLE_UUID_STR_LEN];

  switch (ctxt->op) {
  case BLE_GATT_REGISTER_OP_SVC:
    MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                ctxt->svc.handle);
    break;

  case BLE_GATT_REGISTER_OP_CHR:
    MODLOG_DFLT(DEBUG,
                "registering characteristic %s with "
                "def_handle=%d val_handle=%d\n",
                ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                ctxt->chr.def_handle, ctxt->chr.val_handle);
    break;

  case BLE_GATT_REGISTER_OP_DSC:
    MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                ctxt->dsc.handle);
    break;

  default:
    assert(0);
    break;
  }
}

int gatt_svr_init(void) {
  int rc;

  ble_svc_gap_init();
  ble_svc_gatt_init();
  ble_svc_ans_init();

  rc = ble_gatts_count_cfg(gatt_svr_svcs);
  if (rc != 0) {
    return rc;
  }

  rc = ble_gatts_add_svcs(gatt_svr_svcs);
  if (rc != 0) {
    return rc;
  }

  return 0;
}
