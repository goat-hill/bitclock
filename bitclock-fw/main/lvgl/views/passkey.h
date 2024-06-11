#include <stdint.h>

typedef struct {
  uint32_t passkey;
} lv_helper_view_mode_passkey_data_t;
extern lv_helper_view_mode_passkey_data_t lv_helper_view_mode_passkey_data;

void lv_helper_passkey_create();
void lv_helper_passkey_update(lv_helper_view_mode_passkey_data_t *data);
