"""Smart Relay I2C module - Python library for Linux (e.g., Raspberry Pi)."""

from smbus2 import i2c_msg

# Command IDs
CMD_RELAY_ON = 0x01
CMD_RELAY_OFF = 0x02
CMD_RELAY_ON_FOR = 0x03
CMD_RELAY_OFF_FOR = 0x04
CMD_WATCHDOG_ENABLE = 0x05
CMD_WATCHDOG_DISABLE = 0x06
CMD_WATCHDOG_PING = 0x07
CMD_WATCHDOG_SET_PING_TIMEOUT = 0x08
CMD_WATCHDOG_SET_RESET_DURATION = 0x09
CMD_WATCHDOG_GET_TRIP_COUNT = 0x0A
CMD_WATCHDOG_CLEAR_TRIP_COUNT = 0x0B
CMD_EEPROM_CLEAR = 0x0C
CMD_POWER_CYCLE_ENABLE = 0x0D
CMD_POWER_CYCLE_DISABLE = 0x0E
CMD_POWER_CYCLE_SET_MAX_ON_TIME = 0x0F
CMD_POWER_CYCLE_SLEEP = 0x10
CMD_RELAY_STATE_PERSIST_ENABLE = 0x11
CMD_RELAY_STATE_PERSIST_DISABLE = 0x12
CMD_RELAY_STATE_PERSIST_GET = 0x13
CMD_RELAY_GET_STATE = 0x14
CMD_I2C_SET_ADDRESS = 0x15
CMD_EEPROM_GET_WRITE_COUNT = 0x16
CMD_WATCHDOG_SET_RESET_ACTIVE_STATE = 0x17
CMD_WATCHDOG_GET_RESET_ACTIVE_STATE = 0x18
CMD_EEPROM_GET_SHIFT_COUNT = 0x19
CMD_FIRMWARE_GET_VERSION = 0x1A
CMD_EEPROM_GET_VERSION = 0x1B
CMD_DEVICE_INFO = 0x1C

# Status codes
STATUS_OK = 0x00


class SmartRelay:
    def __init__(self, bus, address=0x2A):
        self.bus = bus
        self.address = address

    def _send(self, cmd, payload=b""):
        data = bytes([cmd]) + payload
        msg = i2c_msg.write(self.address, data)
        self.bus.i2c_rdwr(msg)

    def _read(self, length):
        msg = i2c_msg.read(self.address, length)
        self.bus.i2c_rdwr(msg)
        return bytes(msg)

    def _read_status(self):
        status = self._read(1)[0]
        return status == STATUS_OK

    def relay_on(self, relay_id):
        self._send(CMD_RELAY_ON, bytes([relay_id]))
        return self._read_status()

    def relay_off(self, relay_id):
        self._send(CMD_RELAY_OFF, bytes([relay_id]))
        return self._read_status()

    def relay_on_for(self, relay_id, duration_sec):
        payload = bytes([relay_id, duration_sec & 0xFF, (duration_sec >> 8) & 0xFF])
        self._send(CMD_RELAY_ON_FOR, payload)
        return self._read_status()

    def relay_off_for(self, relay_id, duration_sec):
        payload = bytes([relay_id, duration_sec & 0xFF, (duration_sec >> 8) & 0xFF])
        self._send(CMD_RELAY_OFF_FOR, payload)
        return self._read_status()

    def watchdog_enable(self, relay_id):
        self._send(CMD_WATCHDOG_ENABLE, bytes([relay_id]))
        return self._read_status()

    def watchdog_disable(self):
        self._send(CMD_WATCHDOG_DISABLE)
        return self._read_status()

    def watchdog_ping(self):
        self._send(CMD_WATCHDOG_PING)
        return self._read_status()

    def watchdog_set_ping_timeout(self, timeout_sec):
        payload = bytes([timeout_sec & 0xFF, (timeout_sec >> 8) & 0xFF])
        self._send(CMD_WATCHDOG_SET_PING_TIMEOUT, payload)
        return self._read_status()

    def watchdog_set_reset_duration(self, duration_sec):
        payload = bytes([duration_sec & 0xFF, (duration_sec >> 8) & 0xFF])
        self._send(CMD_WATCHDOG_SET_RESET_DURATION, payload)
        return self._read_status()

    def watchdog_set_reset_active_state(self, active_state):
        if active_state not in (0, 1):
            raise ValueError("active_state must be 0 or 1")
        self._send(CMD_WATCHDOG_SET_RESET_ACTIVE_STATE, bytes([active_state]))
        return self._read_status()

    def watchdog_get_reset_active_state(self):
        self._send(CMD_WATCHDOG_GET_RESET_ACTIVE_STATE)
        resp = self._read(2)
        if resp[0] != STATUS_OK:
            return None
        return 1 if resp[1] else 0

    def watchdog_get_trip_count(self):
        self._send(CMD_WATCHDOG_GET_TRIP_COUNT)
        resp = self._read(5)
        if resp[0] != STATUS_OK:
            return None
        return resp[1] | (resp[2] << 8) | (resp[3] << 16) | (resp[4] << 24)

    def watchdog_clear_trip_count(self):
        self._send(CMD_WATCHDOG_CLEAR_TRIP_COUNT)
        return self._read_status()

    def eeprom_clear(self):
        self._send(CMD_EEPROM_CLEAR)
        return self._read_status()

    def power_cycle_enable(self, relay_id, sleep_enable=False):
        payload = bytes([relay_id, 1 if sleep_enable else 0])
        if sleep_enable:
            self._send(CMD_POWER_CYCLE_ENABLE, payload)
        else:
            self._send(CMD_POWER_CYCLE_ENABLE, bytes([relay_id]))
        return self._read_status()

    def power_cycle_disable(self):
        self._send(CMD_POWER_CYCLE_DISABLE)
        return self._read_status()

    def power_cycle_set_max_on_time(self, max_on_sec):
        payload = bytes([max_on_sec & 0xFF, (max_on_sec >> 8) & 0xFF])
        self._send(CMD_POWER_CYCLE_SET_MAX_ON_TIME, payload)
        return self._read_status()

    def power_cycle_sleep(self, off_sec):
        payload = bytes([off_sec & 0xFF, (off_sec >> 8) & 0xFF])
        self._send(CMD_POWER_CYCLE_SLEEP, payload)
        return self._read_status()

    def relay_state_persist_enable(self):
        self._send(CMD_RELAY_STATE_PERSIST_ENABLE)
        return self._read_status()

    def relay_state_persist_disable(self):
        self._send(CMD_RELAY_STATE_PERSIST_DISABLE)
        return self._read_status()

    def relay_state_persist_get(self):
        self._send(CMD_RELAY_STATE_PERSIST_GET)
        resp = self._read(2)
        if resp[0] != STATUS_OK:
            return None
        return bool(resp[1])

    def relay_get_state(self):
        self._send(CMD_RELAY_GET_STATE)
        resp = self._read(3)
        if resp[0] != STATUS_OK:
            return None
        return resp[1], resp[2]

    def i2c_set_address(self, new_address):
        self._send(CMD_I2C_SET_ADDRESS, bytes([new_address]))
        return self._read_status()

    def eeprom_get_write_count(self):
        self._send(CMD_EEPROM_GET_WRITE_COUNT)
        resp = self._read(5)
        if resp[0] != STATUS_OK:
            return None
        return resp[1] | (resp[2] << 8) | (resp[3] << 16) | (resp[4] << 24)

    def eeprom_get_shift_count(self):
        self._send(CMD_EEPROM_GET_SHIFT_COUNT)
        resp = self._read(2)
        if resp[0] != STATUS_OK:
            return None
        return resp[1]

    def firmware_get_version(self):
        self._send(CMD_FIRMWARE_GET_VERSION)
        resp = self._read(3)
        if resp[0] != STATUS_OK:
            return None
        return resp[1] | (resp[2] << 8)

    def eeprom_get_version(self):
        self._send(CMD_EEPROM_GET_VERSION)
        resp = self._read(2)
        if resp[0] != STATUS_OK:
            return None
        return resp[1]

    def device_info(self):
        self._send(CMD_DEVICE_INFO)
        resp = self._read(8)
        if resp[0] != STATUS_OK:
            return None
        vendor = resp[1] | (resp[2] << 8)
        product = resp[3] | (resp[4] << 8)
        revision = resp[5]
        fw = resp[6] | (resp[7] << 8)
        return vendor, product, revision, fw
