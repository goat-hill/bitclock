#include "ble.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/task.h"
#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "host/ble_hs_id.h"
#include "host/util/util.h"
#include "libs/gatt.h"
#include "nimble/nimble_npl.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "store/config/ble_store_config.h"

static const char *TAG = "ble";

StaticTask_t bleTaskBuffer;
StackType_t bleTaskStack[BLE_STACK_SIZE];

static uint8_t own_addr_type = BLE_OWN_ADDR_PUBLIC;
static struct ble_hs_adv_fields resp_data;
static uint32_t passkey = PASSKEY_NOT_ACTIVE;

static struct ble_npl_event subscription_event_temp;
static gatt_chr_subscription_t active_temp_subscription = {0};

static struct ble_npl_event subscription_event_humidity;
static gatt_chr_subscription_t active_humidity_subscription = {0};

static struct ble_npl_event subscription_event_co2;
static gatt_chr_subscription_t active_co2_subscription = {0};

static struct ble_npl_event subscription_event_voc_index;
static gatt_chr_subscription_t active_voc_index_subscription = {0};

static struct ble_npl_event subscription_event_nox_index;
static gatt_chr_subscription_t active_nox_index_subscription = {0};

static struct ble_npl_event subscription_event_wifi_status;
static gatt_chr_subscription_t active_wifi_status_subscription = {0};

void ble_store_config_init(void); // Implemented in NimBLE

static void ble_advertise();

static void print_addr(const void *addr) {
  const uint8_t *u8p;

  u8p = addr;
  ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x", u8p[5], u8p[4], u8p[3], u8p[2],
           u8p[1], u8p[0]);
}

static void ble_print_conn_desc(struct ble_gap_conn_desc *desc) {
  ESP_LOGI(TAG,
           "handle=%d our_ota_addr_type=%d our_ota_addr=", desc->conn_handle,
           desc->our_ota_addr.type);
  print_addr(desc->our_ota_addr.val);
  ESP_LOGI(TAG, " our_id_addr_type=%d our_id_addr=", desc->our_id_addr.type);
  print_addr(desc->our_id_addr.val);
  ESP_LOGI(TAG,
           " peer_ota_addr_type=%d peer_ota_addr=", desc->peer_ota_addr.type);
  print_addr(desc->peer_ota_addr.val);
  ESP_LOGI(TAG, " peer_id_addr_type=%d peer_id_addr=", desc->peer_id_addr.type);
  print_addr(desc->peer_id_addr.val);
  ESP_LOGI(TAG,
           " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
           "encrypted=%d authenticated=%d bonded=%d",
           desc->conn_itvl, desc->conn_latency, desc->supervision_timeout,
           desc->sec_state.encrypted, desc->sec_state.authenticated,
           desc->sec_state.bonded);
}

static uint32_t ble_enable_passkey() {
  passkey = esp_random() % 1000000;
  ESP_LOGI(TAG, "Enter passkey %" PRIu32 "on the peer side", passkey);
  return passkey;
}

uint32_t ble_active_passkey() { return passkey; }

static void ble_clear_passkey() { passkey = PASSKEY_NOT_ACTIVE; }

static int ble_gap_event(struct ble_gap_event *event, void *arg) {
  struct ble_gap_conn_desc desc;
  int rc;

  switch (event->type) {
  case BLE_GAP_EVENT_CONNECT:
    /* A new connection was established or a connection attempt failed. */
    ESP_LOGI(TAG, "connection %s; status=%d ",
             event->connect.status == 0 ? "established" : "failed",
             event->connect.status);
    if (event->connect.status == 0) {
      rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
      assert(rc == 0);
      ble_print_conn_desc(&desc);
    }
    if (event->connect.status != 0) {
      /* Connection failed; resume advertising. */
      ble_advertise();
    }

    // TODO: Invesitgate if we want BLE_POWER_CONTROL
    // ble_gap_read_remote_transmit_power_level
    return 0;
  case BLE_GAP_EVENT_DISCONNECT:
    ESP_LOGI(TAG, "disconnect; reason=%d ", event->disconnect.reason);
    ble_print_conn_desc(&event->disconnect.conn);
    ble_clear_passkey();
    ble_advertise();
    return 0;
  case BLE_GAP_EVENT_CONN_UPDATE:
    /* The central has updated the connection parameters. */
    ESP_LOGI(TAG, "connection updated; status=%d ", event->conn_update.status);
    rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
    assert(rc == 0);
    ble_print_conn_desc(&desc);
    return 0;
  case BLE_GAP_EVENT_ADV_COMPLETE:
    ESP_LOGI(TAG, "advertise complete; reason=%d", event->adv_complete.reason);
    ble_advertise();
    return 0;
  case BLE_GAP_EVENT_ENC_CHANGE:
    /* Encryption has been enabled or disabled for this connection. */
    ESP_LOGI(TAG, "encryption change event; status=%d ",
             event->enc_change.status);
    ble_clear_passkey();
    rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
    assert(rc == 0);
    ble_print_conn_desc(&desc);
    return 0;
  case BLE_GAP_EVENT_NOTIFY_TX:
    ESP_LOGD(TAG,
             "notify_tx event; conn_handle=%d attr_handle=%d "
             "status=%d is_indication=%d",
             event->notify_tx.conn_handle, event->notify_tx.attr_handle,
             event->notify_tx.status, event->notify_tx.indication);
    return 0;
  case BLE_GAP_EVENT_SUBSCRIBE:
    rc = ble_gap_conn_find(event->subscribe.conn_handle, &desc);
    if (rc != 0) {
      return BLE_ATT_ERR_UNLIKELY;
    }
    if (!desc.sec_state.encrypted) {
      ESP_LOGI(TAG,
               "Blocking subscribe for reason BLE_ATT_ERR_INSUFFICIENT_ENC");
      return BLE_ATT_ERR_INSUFFICIENT_ENC;
    }
    if (!desc.sec_state.authenticated) {
      ESP_LOGI(TAG,
               "Blocking subscribe for reason BLE_ATT_ERR_INSUFFICIENT_AUTHEN");
      return BLE_ATT_ERR_INSUFFICIENT_AUTHEN;
    }
    ESP_LOGI(TAG,
             "subscribe event; conn_handle=%d attr_handle=%d "
             "reason=%d prevn=%d curn=%d previ=%d curi=%d",
             event->subscribe.conn_handle, event->subscribe.attr_handle,
             event->subscribe.reason, event->subscribe.prev_notify,
             event->subscribe.cur_notify, event->subscribe.prev_indicate,
             event->subscribe.cur_indicate);
    if (event->subscribe.attr_handle == gatt_svr_chr_temperature_val_handle) {
      active_temp_subscription.conn_handle = event->subscribe.conn_handle;
      active_temp_subscription.cur_notify = event->subscribe.cur_notify;
      if (active_temp_subscription.cur_notify) {
        ble_notify_temperature_update();
      }
    } else if (event->subscribe.attr_handle ==
               gatt_svr_chr_humidity_val_handle) {
      active_humidity_subscription.conn_handle = event->subscribe.conn_handle;
      active_humidity_subscription.cur_notify = event->subscribe.cur_notify;
      if (active_humidity_subscription.cur_notify) {
        ble_notify_humidity_update();
      }
    } else if (event->subscribe.attr_handle == gatt_svr_chr_co2_val_handle) {
      active_co2_subscription.conn_handle = event->subscribe.conn_handle;
      active_co2_subscription.cur_notify = event->subscribe.cur_notify;
      if (active_co2_subscription.cur_notify) {
        ble_notify_co2_update();
      }
    } else if (event->subscribe.attr_handle ==
               gatt_svr_chr_voc_index_val_handle) {
      active_voc_index_subscription.conn_handle = event->subscribe.conn_handle;
      active_voc_index_subscription.cur_notify = event->subscribe.cur_notify;
      if (active_voc_index_subscription.cur_notify) {
        ble_notify_voc_index_update();
      }
    } else if (event->subscribe.attr_handle ==
               gatt_svr_chr_nox_index_val_handle) {
      active_nox_index_subscription.conn_handle = event->subscribe.conn_handle;
      active_nox_index_subscription.cur_notify = event->subscribe.cur_notify;
      if (active_nox_index_subscription.cur_notify) {
        ble_notify_nox_index_update();
      }
    } else if (event->subscribe.attr_handle ==
               gatt_svr_chr_wifi_status_val_handle) {
      active_wifi_status_subscription.conn_handle =
          event->subscribe.conn_handle;
      active_wifi_status_subscription.cur_notify = event->subscribe.cur_notify;
      if (active_wifi_status_subscription.cur_notify) {
        ble_notify_wifi_status_update();
      }
    }
    return 0;
  case BLE_GAP_EVENT_MTU:
    ESP_LOGI(TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
             event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
    return 0;
  case BLE_GAP_EVENT_REPEAT_PAIRING:
    /* We already have a bond with the peer, but it is attempting to
     * establish a new secure link.  This app sacrifices security for
     * convenience: just throw away the old bond and accept the new link.
     */

    /* Delete the old bond. */
    rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
    assert(rc == 0);
    ble_store_util_delete_peer(&desc.peer_id_addr);

    /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
     * continue with the pairing operation.
     */
    return BLE_GAP_REPEAT_PAIRING_RETRY;
  case BLE_GAP_EVENT_PASSKEY_ACTION:
    ESP_LOGI(TAG, "PASSKEY_ACTION_EVENT started");
    struct ble_sm_io pkey = {0};
    if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
      pkey.action = event->passkey.params.action;
      pkey.passkey = ble_enable_passkey();
      rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
      ESP_LOGI(TAG, "ble_sm_inject_io result: %d", rc);
    } else {
      ESP_LOGE(TAG, "Unexpected passkey action: %d",
               event->passkey.params.action);
    }
    return 0;
  case BLE_GAP_EVENT_AUTHORIZE:
    ESP_LOGI(TAG, "authorize event: conn_handle=%d attr_handle=%d is_read=%d",
             event->authorize.conn_handle, event->authorize.attr_handle,
             event->authorize.is_read);

    /* The default behaviour for the event is to reject authorize request */
    event->authorize.out_response = BLE_GAP_AUTHORIZE_REJECT;
    return 0;
    // BLE_POWER_CONTROL events
    // case BLE_GAP_EVENT_TRANSMIT_POWER:
    // case BLE_GAP_EVENT_PATHLOSS_THRESHOLD:
  }

  return 0;
}

static void ble_advertise() {
  struct ble_gap_adv_params adv_params;
  struct ble_hs_adv_fields fields;
  const char *name;
  int rc;

  memset(&fields, 0, sizeof fields);

  // General discoverability + BLE only
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

  fields.tx_pwr_lvl_is_present = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

  name = "bclk";
  fields.name = (uint8_t *)name;
  fields.name_len = strlen(name);
  fields.name_is_complete = 0;

  fields.uuids128 = &gatt_svr_svc_uuid;
  fields.num_uuids128 = 1;
  fields.uuids128_is_complete = 1;

  rc = ble_gap_adv_set_fields(&fields);
  if (rc != 0) {
    ESP_LOGE(TAG, "error setting advertisement data; rc=%d", rc);
    return;
  }

  memset(&adv_params, 0, sizeof adv_params);
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
  rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                         ble_gap_event, NULL);
  if (rc != 0) {
    ESP_LOGE(TAG, "error enabling advertisement; rc=%d", rc);
    return;
  }
}

static void ble_reset_cb(int reason) {
  ESP_LOGE(TAG, "Resetting state; reason=%d", reason);
}

static void ble_sync_cb(void) {
  int rc;

  // TODO: Resolvable public address for privacy?
  rc = ble_hs_util_ensure_addr(0);
  if (rc != 0) {
    ESP_LOGE(TAG, "Error loading address");
    return;
  }

  rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc != 0) {
    ESP_LOGE(TAG, "error determining address type; rc=%d", rc);
    return;
  }

  uint8_t addr_val[6] = {0};
  rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
  print_addr(addr_val);

  // Increase the preferred MTU size to max
  rc = ble_att_set_preferred_mtu(BLE_ATT_MTU_MAX);
  assert(rc == 0);

  ble_advertise();
}

void ble_notify_temperature_update() {
  if (active_temp_subscription.cur_notify) {
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(), &subscription_event_temp);
  }
}

void ble_notify_humidity_update() {
  if (active_humidity_subscription.cur_notify) {
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(),
                       &subscription_event_humidity);
  }
}

void ble_notify_co2_update() {
  if (active_co2_subscription.cur_notify) {
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(), &subscription_event_co2);
  }
}

void ble_notify_voc_index_update() {
  if (active_voc_index_subscription.cur_notify) {
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(),
                       &subscription_event_voc_index);
  }
}

void ble_notify_nox_index_update() {
  if (active_nox_index_subscription.cur_notify) {
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(),
                       &subscription_event_nox_index);
  }
}

void ble_notify_wifi_status_update() {
  if (active_wifi_status_subscription.cur_notify) {
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(),
                       &subscription_event_wifi_status);
  }
}

static void nimble_host_task(void *param) {
  ESP_LOGI(TAG, "NimBLE task started");
  ble_npl_event_init(&subscription_event_temp, notify_temperature_update,
                     &active_temp_subscription);
  ble_npl_event_init(&subscription_event_humidity, notify_humidity_update,
                     &active_humidity_subscription);
  ble_npl_event_init(&subscription_event_co2, notify_co2_update,
                     &active_co2_subscription);
  ble_npl_event_init(&subscription_event_voc_index, notify_voc_index_update,
                     &active_voc_index_subscription);
  ble_npl_event_init(&subscription_event_nox_index, notify_nox_index_update,
                     &active_nox_index_subscription);
  ble_npl_event_init(&subscription_event_wifi_status, notify_wifi_status_update,
                     &active_wifi_status_subscription);

  nimble_port_run(); // Runs forever.

  nimble_port_freertos_deinit();
  ble_npl_event_deinit(&subscription_event_temp);
  ble_npl_event_deinit(&subscription_event_humidity);
  ble_npl_event_deinit(&subscription_event_co2);
  ble_npl_event_deinit(&subscription_event_voc_index);
  ble_npl_event_deinit(&subscription_event_nox_index);
  ble_npl_event_deinit(&subscription_event_wifi_status);
}

void ble_task_run(void *pvParameters) {
  // Ensure nvs_flash_init() called in main()

  esp_err_t ret = nimble_port_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to init nimble %d", ret);
  }
  ESP_ERROR_CHECK(ret);

  ble_hs_cfg.reset_cb = ble_reset_cb;
  ble_hs_cfg.sync_cb = ble_sync_cb;
  // ble_hs_cfg.gatts_register_cb = ble_gatts_register_cb;
  ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

  ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_DISP_ONLY;
  ble_hs_cfg.sm_bonding = 1;
  ble_hs_cfg.sm_mitm = 1;
  ble_hs_cfg.sm_sc = 1;

  ble_hs_cfg.sm_our_key_dist |=
      BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
  ble_hs_cfg.sm_their_key_dist |=
      BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;

  int rc;
  rc = gatt_svr_init();
  assert(rc == 0);

  // FIXME: Add unique identifier for multiple devices in range?
  rc = ble_svc_gap_device_name_set("bitclock");
  if (rc != 0) {
    ESP_LOGE(TAG, "Error setting device name");
  }

  // Clock appearance 0x0100
  rc = ble_svc_gap_device_appearance_set(0x0100);
  if (rc != 0) {
    ESP_LOGE(TAG, "Error setting appearance");
  }

  resp_data.name = (void *)ble_svc_gap_device_name();
  if (resp_data.name != NULL) {
    resp_data.name_len = strlen(ble_svc_gap_device_name());
    resp_data.name_is_complete = 1;
  }

  ble_store_config_init();
  nimble_port_freertos_init(nimble_host_task);

  while (1) {
    vTaskDelay(portMAX_DELAY);
  }
}
