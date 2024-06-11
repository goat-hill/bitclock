#include "ota.h"

#include "esp_flash_partitions.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_system.h"
#include <string.h>

static const char *TAG = "ota";
#define HASH_LEN 32 // SHA-256 digest length

typedef struct {
  esp_ota_handle_t handle;
  const esp_partition_t *partition;
  size_t progress_bytes;
  size_t total_bytes;
} update_metadata_t;
static update_metadata_t update_metadata = {0};

static void print_sha256(const uint8_t *image_hash, const char *label) {
  char hash_print[HASH_LEN * 2 + 1];
  hash_print[HASH_LEN * 2] = 0;
  for (int i = 0; i < HASH_LEN; ++i) {
    sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
  }
  ESP_LOGI(TAG, "%s: %s", label, hash_print);
}

esp_err_t bitclock_ota_start(size_t image_size) {
  esp_err_t err;

  if (update_metadata.handle) {
    ESP_LOGI(TAG, "Existing unfinished OTA. Clearing.");
    bitclock_ota_abort();
  }

  ESP_LOGI(TAG, "Starting with size %u", image_size);

  update_metadata.partition = esp_ota_get_next_update_partition(NULL);
  update_metadata.total_bytes = image_size;
  assert(update_metadata.partition != NULL);
  ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%" PRIx32,
           update_metadata.partition->subtype,
           update_metadata.partition->address);

  err = esp_ota_begin(update_metadata.partition, image_size,
                      &update_metadata.handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
    esp_ota_abort(update_metadata.handle);
    return err;
  }

  return ESP_OK;
}

esp_err_t bitclock_ota_write(const void *data, size_t size) {
  update_metadata.progress_bytes += size;
  return esp_ota_write(update_metadata.handle, data, size);
}

esp_err_t bitclock_ota_abort() {
  esp_err_t err = esp_ota_abort(update_metadata.handle);
  memset(&update_metadata, 0, sizeof(update_metadata));
  return err;
}

bool bitclock_ota_ready_to_update() {
  return update_metadata.total_bytes > 0 &&
         update_metadata.progress_bytes == update_metadata.total_bytes;
}

esp_err_t bitclock_ota_complete() {
  esp_err_t err;
  err = esp_ota_end(update_metadata.handle);
  if (err != ESP_OK) {
    memset(&update_metadata, 0, sizeof(update_metadata));
    if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
      ESP_LOGE(TAG, "Image validation failed, image is corrupted");
    } else {
      ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
    }
    return err;
  }

  err = esp_ota_set_boot_partition(update_metadata.partition);
  if (err != ESP_OK) {
    memset(&update_metadata, 0, sizeof(update_metadata));
    ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!",
             esp_err_to_name(err));
    return err;
  }
  ESP_LOGI(TAG, "Prepare to restart system!");
  esp_restart();
  // Should never get here...
  return ESP_OK;
}

esp_err_t bitclock_ota_on_boot() {
  esp_err_t err;
  esp_partition_t partition;
  uint8_t sha_256[HASH_LEN] = {0};

  // get sha256 digest for the partition table
  partition.address = ESP_PARTITION_TABLE_OFFSET;
  partition.size = ESP_PARTITION_TABLE_MAX_LEN;
  partition.type = ESP_PARTITION_TYPE_DATA;
  ESP_ERROR_CHECK(esp_partition_get_sha256(&partition, sha_256));
  print_sha256(sha_256, "SHA-256 for the partition table: ");

  // get sha256 digest for bootloader
  partition.address = ESP_BOOTLOADER_OFFSET;
  partition.size = ESP_PARTITION_TABLE_OFFSET;
  partition.type = ESP_PARTITION_TYPE_APP;
  ESP_ERROR_CHECK(esp_partition_get_sha256(&partition, sha_256));
  print_sha256(sha_256, "SHA-256 for bootloader: ");

  // get sha256 digest for running partition
  ESP_ERROR_CHECK(
      esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256));
  print_sha256(sha_256, "SHA-256 for current firmware: ");

  const esp_partition_t *running = esp_ota_get_running_partition();
  esp_ota_img_states_t ota_state;
  err = esp_ota_get_state_partition(running, &ota_state);
  if (err == ESP_OK) {
    if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
      // TODO: Communicate with sensors or some sort of health check?
      bool diagnostic_is_ok = true;
      if (diagnostic_is_ok) {
        ESP_LOGI(TAG, "Image diagnostic passed. Marking image as good.");
        err = esp_ota_mark_app_valid_cancel_rollback();
      } else {
        ESP_LOGE(TAG, "Image diagnostic failed. Rollback.");
        err = esp_ota_mark_app_invalid_rollback_and_reboot();
      }
    }
  }
  return err;
}
