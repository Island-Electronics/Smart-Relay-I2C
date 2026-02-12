#include <stdio.h>
#include "../smart_relay.h"

static int i2c_write_stub(uint8_t addr, const uint8_t *data, uint8_t len) {
  (void)addr; (void)data; (void)len;
  return -1;
}

static int i2c_read_stub(uint8_t addr, uint8_t *data, uint8_t len) {
  (void)addr; (void)data; (void)len;
  return -1;
}

int main(void) {
  smart_relay_t relay = {
    .address = 0x2A,
    .i2c_write = i2c_write_stub,
    .i2c_read = i2c_read_stub
  };

  if (smart_relay_power_cycle_set_max_on_time(&relay, 20) != SMART_RELAY_OK) {
    printf("set max on failed\n");
  }
  if (smart_relay_power_cycle_enable(&relay, 0) != SMART_RELAY_OK) {
    printf("enable power cycle failed\n");
  }

  // Do work here...

  if (smart_relay_power_cycle_sleep(&relay, 300) != SMART_RELAY_OK) {
    printf("sleep command failed\n");
  }

  return 0;
}
