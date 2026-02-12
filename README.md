# Island Electronics - Smart Relay I2C module Libraries

Libraries and examples for Smart Relay I2C module.

## Smart Relay I2C module introduction

**What Smart Relay Does**

- Multi-relay* control over I2C (7-bit addressable, many devices per bus).
- Immediate and timed switching (`on_for` / `off_for`) without extra MCU logic.
- Hardware watchdog with exponential backoff and a configurable reset polarity.
- Battery power-cycle mode with optional ultra-low-power sleep during OFF time.
- Persistent relay state and configuration stored in EEPROM.
- Device discovery via `Device Info` (vendor/product/revision/firmware).

**Host Libraries Included**

- `arduino/SmartRelay/` Arduino library with examples and SerialConsole tool.
- `python/` Python library for Raspberry Pi / Ubuntu with examples and console tool.
- `c/` Portable C library with examples.

> *number of relays - depends on purchased model of Smart Relay

## Arduino Library

**Install**

- Copy `arduino/SmartRelay/` into your Arduino libraries folder.

**Examples**

- `BasicControl` toggles a relay and performs a timed ON.
- `Watchdog` configures watchdog and pings it periodically.
- `BatteryPowerCycle` shows battery-friendly power cycling.
- `SerialConsole` exposes the full protocol over UART and acts as a complete
  configuration and diagnostics tool.

## Python Library (Raspberry Pi / Ubuntu)

**Requirements**

- Enable I2C bus in OS configuration of your Raspberry or another Linux device you are using.
- Install dependencies:
  - `pip install -r python/requirements.txt`

**Usage**

```python
from smbus2 import SMBus
from smartrelay import SmartRelay

with SMBus(1) as bus:
    relay = SmartRelay(bus, address=0x2A)
    relay.relay_on(0)
```

**Examples**

- `python/examples/basic_control.py`
- `python/examples/watchdog.py`
- `python/examples/battery_power_cycle.py`
  Set `PYTHONPATH=python` when running examples so Python can find the library.

**Python Console**

- `python/examples/serial_console.py` is an interactive CLI equivalent to Arduino
  SerialConsole. It implements the full protocol and can be used for setup,
  diagnostics, and field testing.

## C Library

**Overview**

- Portable C API with user-provided `i2c_write`/`i2c_read` callbacks.
- Ideal for embedded Linux and RTOS environments.
- See `c/` for headers, source, and examples.

## Smart Relay I2C Protocol Functions

**Core Relay Control**

- Relay ON/OFF
- Timed ON/OFF
- Relay state readback
- Persistence control option (keeps relay states on power reset)

**Watchdog**

- Enable/disable watchdog on a selected relay
- Ping timeout and reset duration control
- Reset polarity selection (relay ON or OFF = reset)
- Trip counter readback and clear

**Power Cycle (Battery Mode)**

- Enable power-cycle mode and set max ON time (max ON if fallback for a stuck master MCU to reset it)
- Sleep request from MCU with OFF interval
- Optional ultra-low-power sleep during OFF time

**System and Diagnostics**

- EEPROM clear (factory reset)
- EEPROM write count and shift count
- EEPROM layout version
- Firmware version
- Device identity info

## Protocol Command Reference (Summary)

> For full details see `docs/protocol.md` in the repository.


| Command                             | Payload                      | Response                                                 |
| ------------------------------------- | ------------------------------ | ---------------------------------------------------------- |
| Relay On/Off                        | `relay_id`                   | `status`                                                 |
| Relay On/Off For                    | `relay_id`, `duration_sec`   | `status`                                                 |
| Watchdog Enable                     | `relay_id`                   | `status`                                                 |
| Watchdog Disable/Ping               | none                         | `status`                                                 |
| Watchdog Set Ping Timeout           | `timeout_sec`                | `status`                                                 |
| Watchdog Set Reset Duration         | `duration_sec`               | `status`                                                 |
| Watchdog Set/Get Reset Active State | `active_state`               | `status`, `active_state`                                 |
| Watchdog Get/Clear Trip Count       | none                         | `status`, `count`                                        |
| Power Cycle Enable                  | `relay_id`[, `sleep_enable`] | `status`                                                 |
| Power Cycle Disable                 | none                         | `status`                                                 |
| Power Cycle Set Max On              | `max_on_sec`                 | `status`                                                 |
| Power Cycle Sleep                   | `off_sec`                    | `status`                                                 |
| Relay Persist Enable/Disable/Get    | none                         | `status`, `enabled`                                      |
| Relay Get State                     | none                         | `status`, `state_mask`, `init_mask`                      |
| I2C Set Address                     | `addr`                       | `status`                                                 |
| EEPROM Get Write/Shift Count        | none                         | `status`, `count`                                        |
| Firmware/Eeprom Version             | none                         | `status`, `version`                                      |
| Device Info                         | none                         | `status`, `vendor_id`, `product_id`, `rev`, `fw_version` |


## Hardware Notes

**Hardware Options - different models of Smart Relay I2C are available**

- SSR relays or classic(EMR) relays on PCB.
- Controller-only model to combine with your own relay modules.

**Community vs. Commercial models**

- Maker edition uses ATtiny85 MCU for easy Arduino programming. Users can load their own custom firmware to the module.
- Commercial editions use modern, low-cost Microchip MCUs with native I2C bus and higher specs (exact model depends on the industry requirements).
