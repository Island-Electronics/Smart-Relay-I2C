#include "SmartRelay.h"

SmartRelay::SmartRelay(uint8_t address)
  : _address(address), _wire(&Wire) {}

void SmartRelay::begin(TwoWire &wire, uint32_t clock_hz) {
  _wire = &wire;
  _wire->begin();
  if (clock_hz > 0) {
    _wire->setClock(clock_hz);
  }
}

bool SmartRelay::sendCommand(uint8_t cmd, const uint8_t *payload, uint8_t payload_len) {
  _wire->beginTransmission(_address);
  _wire->write(cmd);
  if (payload != nullptr && payload_len > 0) {
    _wire->write(payload, payload_len);
  }
  uint8_t result = _wire->endTransmission();
  return result == 0;
}

bool SmartRelay::readResponse(uint8_t *buf, uint8_t len) {
  uint8_t received = _wire->requestFrom(_address, len);
  if (received != len) {
    return false;
  }
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = _wire->read();
  }
  return true;
}

bool SmartRelay::readStatus(uint8_t &status) {
  uint8_t buf[1];
  if (!readResponse(buf, 1)) {
    return false;
  }
  status = buf[0];
  return true;
}

bool SmartRelay::relayOn(uint8_t relay_id) {
  uint8_t payload[1] = { relay_id };
  if (!sendCommand(CMD_RELAY_ON, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::relayOff(uint8_t relay_id) {
  uint8_t payload[1] = { relay_id };
  if (!sendCommand(CMD_RELAY_OFF, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::relayOnFor(uint8_t relay_id, uint16_t duration_sec) {
  uint8_t payload[3] = { relay_id, (uint8_t)(duration_sec & 0xFF), (uint8_t)((duration_sec >> 8) & 0xFF) };
  if (!sendCommand(CMD_RELAY_ON_FOR, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::relayOffFor(uint8_t relay_id, uint16_t duration_sec) {
  uint8_t payload[3] = { relay_id, (uint8_t)(duration_sec & 0xFF), (uint8_t)((duration_sec >> 8) & 0xFF) };
  if (!sendCommand(CMD_RELAY_OFF_FOR, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::watchdogEnable(uint8_t relay_id) {
  uint8_t payload[1] = { relay_id };
  if (!sendCommand(CMD_WATCHDOG_ENABLE, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::watchdogDisable(void) {
  if (!sendCommand(CMD_WATCHDOG_DISABLE, nullptr, 0)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::watchdogPing(void) {
  if (!sendCommand(CMD_WATCHDOG_PING, nullptr, 0)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::watchdogSetPingTimeout(uint16_t timeout_sec) {
  uint8_t payload[2] = { (uint8_t)(timeout_sec & 0xFF), (uint8_t)((timeout_sec >> 8) & 0xFF) };
  if (!sendCommand(CMD_WATCHDOG_SET_PING_TIMEOUT, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::watchdogSetResetDuration(uint16_t duration_sec) {
  uint8_t payload[2] = { (uint8_t)(duration_sec & 0xFF), (uint8_t)((duration_sec >> 8) & 0xFF) };
  if (!sendCommand(CMD_WATCHDOG_SET_RESET_DURATION, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::watchdogSetResetActiveState(uint8_t active_state) {
  if (active_state > 1) {
    return false;
  }
  uint8_t payload[1] = { active_state };
  if (!sendCommand(CMD_WATCHDOG_SET_RESET_ACTIVE_STATE, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::watchdogGetResetActiveState(uint8_t &out_active_state) {
  if (!sendCommand(CMD_WATCHDOG_GET_RESET_ACTIVE_STATE, nullptr, 0)) return false;
  uint8_t buf[2];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_active_state = buf[1] ? 1 : 0;
  return true;
}

bool SmartRelay::watchdogGetTripCount(uint32_t &out_count) {
  if (!sendCommand(CMD_WATCHDOG_GET_TRIP_COUNT, nullptr, 0)) return false;
  uint8_t buf[5];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_count = (uint32_t)buf[1] |
              ((uint32_t)buf[2] << 8) |
              ((uint32_t)buf[3] << 16) |
              ((uint32_t)buf[4] << 24);
  return true;
}

bool SmartRelay::watchdogClearTripCount(void) {
  if (!sendCommand(CMD_WATCHDOG_CLEAR_TRIP_COUNT, nullptr, 0)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::eepromClear(void) {
  if (!sendCommand(CMD_EEPROM_CLEAR, nullptr, 0)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::powerCycleEnable(uint8_t relay_id) {
  return powerCycleEnable(relay_id, false);
}

bool SmartRelay::powerCycleEnable(uint8_t relay_id, bool sleep_enable) {
  uint8_t payload[1] = { relay_id };
  uint8_t payload_len = 1;
  uint8_t payload_ext[2] = { relay_id, (uint8_t)(sleep_enable ? 1 : 0) };
  if (sleep_enable) {
    payload_len = 2;
  }
  if (!sendCommand(CMD_POWER_CYCLE_ENABLE, sleep_enable ? payload_ext : payload, payload_len)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::powerCycleDisable(void) {
  if (!sendCommand(CMD_POWER_CYCLE_DISABLE, nullptr, 0)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::powerCycleSetMaxOnTime(uint16_t max_on_sec) {
  uint8_t payload[2] = { (uint8_t)(max_on_sec & 0xFF), (uint8_t)((max_on_sec >> 8) & 0xFF) };
  if (!sendCommand(CMD_POWER_CYCLE_SET_MAX_ON_TIME, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::powerCycleSleep(uint16_t off_sec) {
  uint8_t payload[2] = { (uint8_t)(off_sec & 0xFF), (uint8_t)((off_sec >> 8) & 0xFF) };
  if (!sendCommand(CMD_POWER_CYCLE_SLEEP, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::relayStatePersistEnable(void) {
  if (!sendCommand(CMD_RELAY_STATE_PERSIST_ENABLE, nullptr, 0)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::relayStatePersistDisable(void) {
  if (!sendCommand(CMD_RELAY_STATE_PERSIST_DISABLE, nullptr, 0)) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::relayStatePersistGet(bool &out_enabled) {
  if (!sendCommand(CMD_RELAY_STATE_PERSIST_GET, nullptr, 0)) return false;
  uint8_t buf[2];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_enabled = (buf[1] != 0);
  return true;
}

bool SmartRelay::relayGetState(uint8_t &out_state_mask, uint8_t &out_init_mask) {
  if (!sendCommand(CMD_RELAY_GET_STATE, nullptr, 0)) return false;
  uint8_t buf[3];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_state_mask = buf[1];
  out_init_mask = buf[2];
  return true;
}

bool SmartRelay::i2cSetAddress(uint8_t new_address) {
  uint8_t payload[1] = { new_address };
  if (!sendCommand(CMD_I2C_SET_ADDRESS, payload, sizeof(payload))) return false;
  uint8_t status = STATUS_ERR;
  if (!readStatus(status)) return false;
  return status == STATUS_OK;
}

bool SmartRelay::eepromGetWriteCount(uint32_t &out_count) {
  if (!sendCommand(CMD_EEPROM_GET_WRITE_COUNT, nullptr, 0)) return false;
  uint8_t buf[5];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_count = (uint32_t)buf[1] |
              ((uint32_t)buf[2] << 8) |
              ((uint32_t)buf[3] << 16) |
              ((uint32_t)buf[4] << 24);
  return true;
}

bool SmartRelay::eepromGetShiftCount(uint8_t &out_count) {
  if (!sendCommand(CMD_EEPROM_GET_SHIFT_COUNT, nullptr, 0)) return false;
  uint8_t buf[2];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_count = buf[1];
  return true;
}

bool SmartRelay::firmwareGetVersion(uint16_t &out_version) {
  if (!sendCommand(CMD_FIRMWARE_GET_VERSION, nullptr, 0)) return false;
  uint8_t buf[3];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_version = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
  return true;
}

bool SmartRelay::eepromGetVersion(uint8_t &out_version) {
  if (!sendCommand(CMD_EEPROM_GET_VERSION, nullptr, 0)) return false;
  uint8_t buf[2];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_version = buf[1];
  return true;
}

bool SmartRelay::deviceInfo(uint16_t &out_vendor_id, uint16_t &out_product_id, uint8_t &out_revision, uint16_t &out_fw_version) {
  if (!sendCommand(CMD_DEVICE_INFO, nullptr, 0)) return false;
  uint8_t buf[8];
  if (!readResponse(buf, sizeof(buf))) return false;
  if (buf[0] != STATUS_OK) return false;
  out_vendor_id = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
  out_product_id = (uint16_t)buf[3] | ((uint16_t)buf[4] << 8);
  out_revision = buf[5];
  out_fw_version = (uint16_t)buf[6] | ((uint16_t)buf[7] << 8);
  return true;
}
