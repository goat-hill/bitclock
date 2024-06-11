#include "host/ble_uuid.h"

extern const ble_uuid128_t gatt_svr_svc_uuid;

extern uint16_t gatt_svr_chr_wifi_val_handle;
extern uint16_t gatt_svr_chr_ota_size_val_handle;
extern uint16_t gatt_svr_chr_ota_data_val_handle;
extern uint16_t gatt_svr_chr_temperature_val_handle;
extern uint16_t gatt_svr_chr_humidity_val_handle;
extern uint16_t gatt_svr_chr_co2_val_handle;
extern uint16_t gatt_svr_chr_voc_index_val_handle;
extern uint16_t gatt_svr_chr_nox_index_val_handle;
extern uint16_t gatt_svr_chr_wifi_status_val_handle;

typedef struct {
  uint16_t conn_handle;
  uint8_t cur_notify;
} gatt_chr_subscription_t;

// Only call these from nimble task to avoid race conditions
struct ble_npl_event;
void notify_temperature_update(struct ble_npl_event *ev);
void notify_humidity_update(struct ble_npl_event *ev);
void notify_co2_update(struct ble_npl_event *ev);
void notify_voc_index_update(struct ble_npl_event *ev);
void notify_nox_index_update(struct ble_npl_event *ev);
void notify_wifi_status_update(struct ble_npl_event *ev);

int gatt_svr_init(void);
void gatt_notify_temp();
