#ifndef SMART_RELAY_ARDUINO_H
#define SMART_RELAY_ARDUINO_H

#include <Arduino.h>
#include <Wire.h>

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

class SmartRelay {
public:
  explicit SmartRelay(uint8_t address = 0x2A);

  void begin(TwoWire &wire = Wire, uint32_t clock_hz = 0);

  bool relayOn(uint8_t relay_id);
  bool relayOff(uint8_t relay_id);
  bool relayOnFor(uint8_t relay_id, uint16_t duration_sec);
  bool relayOffFor(uint8_t relay_id, uint16_t duration_sec);

  bool watchdogEnable(uint8_t relay_id);
  bool watchdogDisable(void);
  bool watchdogPing(void);
  bool watchdogSetPingTimeout(uint16_t timeout_sec);
  bool watchdogSetResetDuration(uint16_t duration_sec);
  bool watchdogSetResetActiveState(uint8_t active_state);
  bool watchdogGetResetActiveState(uint8_t &out_active_state);
  bool watchdogGetTripCount(uint32_t &out_count);
  bool watchdogClearTripCount(void);

  bool eepromClear(void);

  bool powerCycleEnable(uint8_t relay_id);
  bool powerCycleEnable(uint8_t relay_id, bool sleep_enable);
  bool powerCycleDisable(void);
  bool powerCycleSetMaxOnTime(uint16_t max_on_sec);
  bool powerCycleSleep(uint16_t off_sec);

  bool relayStatePersistEnable(void);
  bool relayStatePersistDisable(void);
  bool relayStatePersistGet(bool &out_enabled);
  bool relayGetState(uint8_t &out_state_mask, uint8_t &out_init_mask);
  bool i2cSetAddress(uint8_t new_address);
  bool eepromGetWriteCount(uint32_t &out_count);
  bool eepromGetShiftCount(uint8_t &out_count);
  bool firmwareGetVersion(uint16_t &out_version);
  bool eepromGetVersion(uint8_t &out_version);
  bool deviceInfo(uint16_t &out_vendor_id, uint16_t &out_product_id, uint8_t &out_revision, uint16_t &out_fw_version);

private:
  bool sendCommand(uint8_t cmd, const uint8_t *payload, uint8_t payload_len);
  bool readStatus(uint8_t &status);
  bool readResponse(uint8_t *buf, uint8_t len);

  uint8_t _address;
  TwoWire *_wire;
};

#endif // SMART_RELAY_ARDUINO_H
