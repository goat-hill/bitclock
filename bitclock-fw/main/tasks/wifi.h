#include "freertos/FreeRTOS.h"

#define WIFI_STACK_SIZE 1024 * 4

typedef struct {
  char ssid[32];
  char password[64];
} WiFiCredentials_t;
extern QueueHandle_t wifiCredentialsQueueHandle;

#define WIFI_CREDENTIALS_UPDATED_EVENT BIT0
#define WIFI_READY_TO_CONNECT_EVENT BIT1
extern EventGroupHandle_t wifi_event_group_handle;

extern SemaphoreHandle_t wifi_req_semaphore;

extern StaticTask_t wifiTaskBuffer;
extern StackType_t wifiTaskStack[WIFI_STACK_SIZE];

void wifi_task_run(void *pvParameters);

bool bitclock_wifi_is_started();
bool bitclock_wifi_has_ip();
