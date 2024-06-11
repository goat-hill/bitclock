#include "led.h"

#include "pins.h"
#include "tasks.h"

#ifdef LED_TASK_ENABLED

#define CONFIG_BLINK_PERIOD 500

StaticTask_t ledTaskBuffer;
StackType_t ledTaskStack[LED_STACK_SIZE];

static uint8_t s_led_state = 0;

static void configure_led(void) {
  gpio_reset_pin(PIN_NUM_BLINK);
  gpio_set_direction(PIN_NUM_BLINK, GPIO_MODE_OUTPUT);
}

static void blink_led(void) {
  gpio_set_level(PIN_NUM_BLINK, s_led_state);
  s_led_state = !s_led_state;
}

void led_task_run(void *pvParameters) {
  configure_led();
  while (1) {
    blink_led();
    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
  }
}

#endif
