#include <stdio.h>
#include "../smart_relay.h"

static int i2c_write_stub(uint8_t addr, const uint8_t *data, uint8_t len) {
  (void)addr; (void)data; (void)len;
  // Replace with your platform I2C write.
  return -1;
}

static int i2c_read_stub(uint8_t addr, uint8_t *data, uint8_t len) {
  (void)addr; (void)data; (void)len;
  // Replace with your platform I2C read.
  return -1;
}

int main(void) {
  smart_relay_t relay = {
    .address = 0x2A,
    .i2c_write = i2c_write_stub,
    .i2c_read = i2c_read_stub
  };

  if (smart_relay_relay_on(&relay, 0) != SMART_RELAY_OK) {
    printf("relay_on failed\n");
  }

  if (smart_relay_relay_off(&relay, 0) != SMART_RELAY_OK) {
    printf("relay_off failed\n");
  }

  return 0;
}
