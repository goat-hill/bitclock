#include "i2c.h"

#include "pins.h"

#define I2C_PORT_NUM I2C_NUM_0

i2c_master_bus_handle_t i2c_bus_handle;

SemaphoreHandle_t i2c_semaphore;
StaticSemaphore_t i2c_semaphore_mutex_buffer;

void i2c_bus_init() {
  i2c_master_bus_config_t i2c_mst_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_PORT_NUM,
      .scl_io_num = PIN_NUM_I2C_SCL,
      .sda_io_num = PIN_NUM_I2C_SDA,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = I2C_REQUIRES_PULLUP,
  };
  ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &i2c_bus_handle));

  i2c_semaphore = xSemaphoreCreateMutexStatic(&i2c_semaphore_mutex_buffer);
}
