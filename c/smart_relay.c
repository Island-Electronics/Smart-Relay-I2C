#include "smart_relay.h"

static int send_command(smart_relay_t *dev, uint8_t cmd, const uint8_t *payload, uint8_t payload_len) {
  if (dev == 0 || dev->i2c_write == 0) {
    return SMART_RELAY_ERR_PARAM;
  }

  uint8_t buf[1 + 8];
  uint8_t total_len = 1 + payload_len;
  if (payload_len > 8) {
    return SMART_RELAY_ERR_PARAM;
  }

  buf[0] = cmd;
  for (uint8_t i = 0; i < payload_len; i++) {
    buf[1 + i] = payload[i];
  }

  int ret = dev->i2c_write(dev->address, buf, total_len);
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  return SMART_RELAY_OK;
}

static int read_status(smart_relay_t *dev) {
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }

  uint8_t status = 0;
  int ret = dev->i2c_read(dev->address, &status, 1);
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (status != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  return SMART_RELAY_OK;
}

int smart_relay_relay_on(smart_relay_t *dev, uint8_t relay_id) {
  uint8_t payload[1] = { relay_id };
  int ret = send_command(dev, CMD_RELAY_ON, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_relay_off(smart_relay_t *dev, uint8_t relay_id) {
  uint8_t payload[1] = { relay_id };
  int ret = send_command(dev, CMD_RELAY_OFF, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_relay_on_for(smart_relay_t *dev, uint8_t relay_id, uint16_t duration_sec) {
  uint8_t payload[3] = { relay_id, (uint8_t)(duration_sec & 0xFF), (uint8_t)((duration_sec >> 8) & 0xFF) };
  int ret = send_command(dev, CMD_RELAY_ON_FOR, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_relay_off_for(smart_relay_t *dev, uint8_t relay_id, uint16_t duration_sec) {
  uint8_t payload[3] = { relay_id, (uint8_t)(duration_sec & 0xFF), (uint8_t)((duration_sec >> 8) & 0xFF) };
  int ret = send_command(dev, CMD_RELAY_OFF_FOR, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_watchdog_enable(smart_relay_t *dev, uint8_t relay_id) {
  uint8_t payload[1] = { relay_id };
  int ret = send_command(dev, CMD_WATCHDOG_ENABLE, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_watchdog_disable(smart_relay_t *dev) {
  int ret = send_command(dev, CMD_WATCHDOG_DISABLE, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_watchdog_ping(smart_relay_t *dev) {
  int ret = send_command(dev, CMD_WATCHDOG_PING, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_watchdog_set_ping_timeout(smart_relay_t *dev, uint16_t timeout_sec) {
  uint8_t payload[2] = { (uint8_t)(timeout_sec & 0xFF), (uint8_t)((timeout_sec >> 8) & 0xFF) };
  int ret = send_command(dev, CMD_WATCHDOG_SET_PING_TIMEOUT, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_watchdog_set_reset_duration(smart_relay_t *dev, uint16_t duration_sec) {
  uint8_t payload[2] = { (uint8_t)(duration_sec & 0xFF), (uint8_t)((duration_sec >> 8) & 0xFF) };
  int ret = send_command(dev, CMD_WATCHDOG_SET_RESET_DURATION, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_watchdog_set_reset_active_state(smart_relay_t *dev, uint8_t active_state) {
  if (active_state > 1) {
    return SMART_RELAY_ERR_PARAM;
  }
  uint8_t payload[1] = { active_state };
  int ret = send_command(dev, CMD_WATCHDOG_SET_RESET_ACTIVE_STATE, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_watchdog_get_reset_active_state(smart_relay_t *dev, uint8_t *out_active_state) {
  if (out_active_state == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_WATCHDOG_GET_RESET_ACTIVE_STATE, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[2];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_active_state = buf[1] ? 1 : 0;
  return SMART_RELAY_OK;
}

int smart_relay_watchdog_get_trip_count(smart_relay_t *dev, uint32_t *out_count) {
  if (out_count == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_WATCHDOG_GET_TRIP_COUNT, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[5];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }

  *out_count = (uint32_t)buf[1] |
               ((uint32_t)buf[2] << 8) |
               ((uint32_t)buf[3] << 16) |
               ((uint32_t)buf[4] << 24);
  return SMART_RELAY_OK;
}

int smart_relay_watchdog_clear_trip_count(smart_relay_t *dev) {
  int ret = send_command(dev, CMD_WATCHDOG_CLEAR_TRIP_COUNT, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_eeprom_clear(smart_relay_t *dev) {
  int ret = send_command(dev, CMD_EEPROM_CLEAR, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_power_cycle_enable(smart_relay_t *dev, uint8_t relay_id) {
  return smart_relay_power_cycle_enable_ex(dev, relay_id, 0);
}

int smart_relay_power_cycle_enable_ex(smart_relay_t *dev, uint8_t relay_id, uint8_t sleep_enable) {
  uint8_t payload[2] = { relay_id, sleep_enable ? 1 : 0 };
  uint8_t payload_len = sleep_enable ? 2 : 1;
  int ret = send_command(dev, CMD_POWER_CYCLE_ENABLE, payload, payload_len);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_power_cycle_disable(smart_relay_t *dev) {
  int ret = send_command(dev, CMD_POWER_CYCLE_DISABLE, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_power_cycle_set_max_on_time(smart_relay_t *dev, uint16_t max_on_sec) {
  uint8_t payload[2] = { (uint8_t)(max_on_sec & 0xFF), (uint8_t)((max_on_sec >> 8) & 0xFF) };
  int ret = send_command(dev, CMD_POWER_CYCLE_SET_MAX_ON_TIME, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_power_cycle_sleep(smart_relay_t *dev, uint16_t off_sec) {
  uint8_t payload[2] = { (uint8_t)(off_sec & 0xFF), (uint8_t)((off_sec >> 8) & 0xFF) };
  int ret = send_command(dev, CMD_POWER_CYCLE_SLEEP, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_relay_state_persist_enable(smart_relay_t *dev) {
  int ret = send_command(dev, CMD_RELAY_STATE_PERSIST_ENABLE, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_relay_state_persist_disable(smart_relay_t *dev) {
  int ret = send_command(dev, CMD_RELAY_STATE_PERSIST_DISABLE, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_relay_state_persist_get(smart_relay_t *dev, uint8_t *out_enabled) {
  if (out_enabled == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_RELAY_STATE_PERSIST_GET, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[2];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_enabled = buf[1];
  return SMART_RELAY_OK;
}

int smart_relay_relay_get_state(smart_relay_t *dev, uint8_t *out_state_mask, uint8_t *out_init_mask) {
  if (out_state_mask == 0 || out_init_mask == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_RELAY_GET_STATE, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[3];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_state_mask = buf[1];
  *out_init_mask = buf[2];
  return SMART_RELAY_OK;
}

int smart_relay_i2c_set_address(smart_relay_t *dev, uint8_t new_address) {
  uint8_t payload[1] = { new_address };
  int ret = send_command(dev, CMD_I2C_SET_ADDRESS, payload, sizeof(payload));
  if (ret != SMART_RELAY_OK) return ret;
  return read_status(dev);
}

int smart_relay_eeprom_get_write_count(smart_relay_t *dev, uint32_t *out_count) {
  if (out_count == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_EEPROM_GET_WRITE_COUNT, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[5];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_count = (uint32_t)buf[1] |
               ((uint32_t)buf[2] << 8) |
               ((uint32_t)buf[3] << 16) |
               ((uint32_t)buf[4] << 24);
  return SMART_RELAY_OK;
}

int smart_relay_eeprom_get_shift_count(smart_relay_t *dev, uint8_t *out_count) {
  if (out_count == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_EEPROM_GET_SHIFT_COUNT, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[2];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_count = buf[1];
  return SMART_RELAY_OK;
}

int smart_relay_firmware_get_version(smart_relay_t *dev, uint16_t *out_version) {
  if (out_version == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_FIRMWARE_GET_VERSION, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[3];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_version = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
  return SMART_RELAY_OK;
}

int smart_relay_eeprom_get_version(smart_relay_t *dev, uint8_t *out_version) {
  if (out_version == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_EEPROM_GET_VERSION, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[2];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_version = buf[1];
  return SMART_RELAY_OK;
}

int smart_relay_device_info(smart_relay_t *dev, uint16_t *out_vendor_id, uint16_t *out_product_id,
                            uint8_t *out_revision, uint16_t *out_fw_version) {
  if (out_vendor_id == 0 || out_product_id == 0 || out_revision == 0 || out_fw_version == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  int ret = send_command(dev, CMD_DEVICE_INFO, 0, 0);
  if (ret != SMART_RELAY_OK) return ret;

  uint8_t buf[8];
  if (dev == 0 || dev->i2c_read == 0) {
    return SMART_RELAY_ERR_PARAM;
  }
  ret = dev->i2c_read(dev->address, buf, sizeof(buf));
  if (ret != 0) {
    return SMART_RELAY_ERR_IO;
  }
  if (buf[0] != STATUS_OK) {
    return SMART_RELAY_ERR_STATUS;
  }
  *out_vendor_id = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
  *out_product_id = (uint16_t)buf[3] | ((uint16_t)buf[4] << 8);
  *out_revision = buf[5];
  *out_fw_version = (uint16_t)buf[6] | ((uint16_t)buf[7] << 8);
  return SMART_RELAY_OK;
}
