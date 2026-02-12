#ifndef SMART_RELAY_C_H
#define SMART_RELAY_C_H

#include <stdint.h>

// Command IDs
enum {
  CMD_RELAY_ON = 0x01,
  CMD_RELAY_OFF = 0x02,
  CMD_RELAY_ON_FOR = 0x03,
  CMD_RELAY_OFF_FOR = 0x04,

  CMD_WATCHDOG_ENABLE = 0x05,
  CMD_WATCHDOG_DISABLE = 0x06,
  CMD_WATCHDOG_PING = 0x07,
  CMD_WATCHDOG_SET_PING_TIMEOUT = 0x08,
  CMD_WATCHDOG_SET_RESET_DURATION = 0x09,
  CMD_WATCHDOG_GET_TRIP_COUNT = 0x0A,
  CMD_WATCHDOG_CLEAR_TRIP_COUNT = 0x0B,
  CMD_EEPROM_CLEAR_TRIP_COUNT = 0x0B,

  CMD_EEPROM_CLEAR = 0x0C,

  CMD_POWER_CYCLE_ENABLE = 0x0D,
  CMD_POWER_CYCLE_DISABLE = 0x0E,
  CMD_POWER_CYCLE_SET_MAX_ON_TIME = 0x0F,
  CMD_POWER_CYCLE_SLEEP = 0x10,
  CMD_RELAY_STATE_PERSIST_ENABLE = 0x11,
  CMD_RELAY_STATE_PERSIST_DISABLE = 0x12,
  CMD_RELAY_STATE_PERSIST_GET = 0x13,
  CMD_RELAY_GET_STATE = 0x14,
  CMD_I2C_SET_ADDRESS = 0x15,
  CMD_EEPROM_GET_WRITE_COUNT = 0x16,
  CMD_WATCHDOG_SET_RESET_ACTIVE_STATE = 0x17,
  CMD_WATCHDOG_GET_RESET_ACTIVE_STATE = 0x18,
  CMD_EEPROM_GET_SHIFT_COUNT = 0x19,
  CMD_FIRMWARE_GET_VERSION = 0x1A,
  CMD_EEPROM_GET_VERSION = 0x1B,
  CMD_DEVICE_INFO = 0x1C
};

// Status codes
enum {
  STATUS_OK = 0x00,
  STATUS_ERR = 0x01,
  STATUS_BAD_CMD = 0x02,
  STATUS_BAD_PARAM = 0x03,
  STATUS_BUSY = 0x04
};

// Return codes for the C API
#define SMART_RELAY_OK 0
#define SMART_RELAY_ERR_IO -1
#define SMART_RELAY_ERR_STATUS -2
#define SMART_RELAY_ERR_PARAM -3

typedef struct {
  uint8_t address;
  int (*i2c_write)(uint8_t addr, const uint8_t *data, uint8_t len);
  int (*i2c_read)(uint8_t addr, uint8_t *data, uint8_t len);
} smart_relay_t;

int smart_relay_relay_on(smart_relay_t *dev, uint8_t relay_id);
int smart_relay_relay_off(smart_relay_t *dev, uint8_t relay_id);
int smart_relay_relay_on_for(smart_relay_t *dev, uint8_t relay_id, uint16_t duration_sec);
int smart_relay_relay_off_for(smart_relay_t *dev, uint8_t relay_id, uint16_t duration_sec);

int smart_relay_watchdog_enable(smart_relay_t *dev, uint8_t relay_id);
int smart_relay_watchdog_disable(smart_relay_t *dev);
int smart_relay_watchdog_ping(smart_relay_t *dev);
int smart_relay_watchdog_set_ping_timeout(smart_relay_t *dev, uint16_t timeout_sec);
int smart_relay_watchdog_set_reset_duration(smart_relay_t *dev, uint16_t duration_sec);
int smart_relay_watchdog_set_reset_active_state(smart_relay_t *dev, uint8_t active_state);
int smart_relay_watchdog_get_reset_active_state(smart_relay_t *dev, uint8_t *out_active_state);
int smart_relay_watchdog_get_trip_count(smart_relay_t *dev, uint32_t *out_count);
int smart_relay_watchdog_clear_trip_count(smart_relay_t *dev);

int smart_relay_eeprom_clear(smart_relay_t *dev);

int smart_relay_power_cycle_enable(smart_relay_t *dev, uint8_t relay_id);
int smart_relay_power_cycle_enable_ex(smart_relay_t *dev, uint8_t relay_id, uint8_t sleep_enable);
int smart_relay_power_cycle_disable(smart_relay_t *dev);
int smart_relay_power_cycle_set_max_on_time(smart_relay_t *dev, uint16_t max_on_sec);
int smart_relay_power_cycle_sleep(smart_relay_t *dev, uint16_t off_sec);

int smart_relay_relay_state_persist_enable(smart_relay_t *dev);
int smart_relay_relay_state_persist_disable(smart_relay_t *dev);
int smart_relay_relay_state_persist_get(smart_relay_t *dev, uint8_t *out_enabled);
int smart_relay_relay_get_state(smart_relay_t *dev, uint8_t *out_state_mask, uint8_t *out_init_mask);
int smart_relay_i2c_set_address(smart_relay_t *dev, uint8_t new_address);
int smart_relay_eeprom_get_write_count(smart_relay_t *dev, uint32_t *out_count);
int smart_relay_eeprom_get_shift_count(smart_relay_t *dev, uint8_t *out_count);
int smart_relay_firmware_get_version(smart_relay_t *dev, uint16_t *out_version);
int smart_relay_eeprom_get_version(smart_relay_t *dev, uint8_t *out_version);
int smart_relay_device_info(smart_relay_t *dev, uint16_t *out_vendor_id, uint16_t *out_product_id,
                            uint8_t *out_revision, uint16_t *out_fw_version);

#endif // SMART_RELAY_C_H
