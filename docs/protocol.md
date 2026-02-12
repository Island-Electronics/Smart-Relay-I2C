# Smart Relay I2C Protocol

## Transport

- I2C 7-bit address (default `0x2A`).
- Little-endian for multi-byte values.
- Master writes a command, then performs a separate read to get the response.

## Response Format

- Byte 0: status code.
- Additional bytes: command-specific data.

Status codes

- `0x00` OK
- `0x01` ERR
- `0x02` BAD_CMD
- `0x03` BAD_PARAM
- `0x04` BUSY

## Commands

All commands return a status response. Commands that return data include it after the status byte.


| Command                         | ID     | Payload                                                | Response                                                                               |
| --------------------------------- | -------- | -------------------------------------------------------- | ---------------------------------------------------------------------------------------- |
| Relay On                        | `0x01` | `relay_id`                                             | `status`                                                                               |
| Relay Off                       | `0x02` | `relay_id`                                             | `status`                                                                               |
| Relay On For                    | `0x03` | `relay_id`, `duration_sec` (u16)                       | `status`                                                                               |
| Relay Off For                   | `0x04` | `relay_id`, `duration_sec` (u16)                       | `status`                                                                               |
| Watchdog Enable                 | `0x05` | `relay_id`                                             | `status`                                                                               |
| Watchdog Disable                | `0x06` | none                                                   | `status`                                                                               |
| Watchdog Ping                   | `0x07` | none                                                   | `status`                                                                               |
| Watchdog Set Ping Timeout       | `0x08` | `timeout_sec` (u16)                                    | `status`                                                                               |
| Watchdog Set Reset Duration     | `0x09` | `duration_sec` (u16)                                   | `status`                                                                               |
| Watchdog Get Trip Count         | `0x0A` | none                                                   | `status`, `count` (u32)                                                                |
| Watchdog Clear Trip Count       | `0x0B` | none                                                   | `status`                                                                               |
| EEPROM Clear Trip Count (alias) | `0x0B` | none                                                   | `status`                                                                               |
| EEPROM Clear                    | `0x0C` | none                                                   | `status`                                                                               |
| Power Cycle Enable              | `0x0D` | `relay_id`[, `sleep_enable` (u8, 0=disable, 1=enable)] | `status`                                                                               |
| Power Cycle Disable             | `0x0E` | none                                                   | `status`                                                                               |
| Power Cycle Set Max On Time     | `0x0F` | `max_on_sec` (u16)                                     | `status`                                                                               |
| Power Cycle Sleep               | `0x10` | `off_sec` (u16)                                        | `status`                                                                               |
| Relay State Persist Enable      | `0x11` | none                                                   | `status`                                                                               |
| Relay State Persist Disable     | `0x12` | none                                                   | `status`                                                                               |
| Relay State Persist Get         | `0x13` | none                                                   | `status`, `enabled` (u8)                                                               |
| Relay Get State                 | `0x14` | none                                                   | `status`, `state_mask` (u8), `init_mask` (u8)                                          |
| I2C Set Address                 | `0x15` | `addr` (u8)                                            | `status`                                                                               |
| EEPROM Get Write Count          | `0x16` | none                                                   | `status`, `count` (u32)                                                                |
| Watchdog Set Reset Active State | `0x17` | `active_state` (u8, 0=reset on OFF, 1=reset on ON)     | `status`                                                                               |
| Watchdog Get Reset Active State | `0x18` | none                                                   | `status`, `active_state` (u8)                                                          |
| EEPROM Get Shift Count          | `0x19` | none                                                   | `status`, `count` (u8)                                                                 |
| Firmware Get Version            | `0x1A` | none                                                   | `status`, `version` (u16)                                                              |
| EEPROM Get Version              | `0x1B` | none                                                   | `status`, `version` (u8)                                                               |
| Device Info                     | `0x1C` | none                                                   | `status`, `vendor_id` (u16), `product_id` (u16), `device_rev` (u8), `fw_version` (u16) |

## Mode Interaction

- Enabling Watchdog disables Power Cycle.
- Enabling Power Cycle disables Watchdog.

## EEPROM Persistence

- Watchdog enable state, Power Cycle enable state, and relay output states are persisted in EEPROM.
- On power reset, the module restores these states so the watchdog remains active without extra logic on the master MCU.
- Relay state persistence can be enabled/disabled to reduce EEPROM wear in high-frequency relay use.
- I2C address is stored in EEPROM and takes effect after power reset.
- Temporary relay transitions (`Relay On/Off For`, watchdog reset pulses, power-cycle sleep) are not persisted.
- EEPROM writes are wear-leveled across multiple slots. After the write counter reaches the shift threshold (90 000 firmware default), the active slot advances (until space remains).
- `EEPROM Clear` resets settings and trip count but does not reset the write or shift counters.

## Watchdog Behavior

- The relay expects a ping from the master MCU within the configured timeout.
- On timeout, the relay is driven to the reset-active state for the configured reset duration.
- After each reset, the ping timeout doubles up to a maximum.
- When a ping is received after a reset series, the timeout returns to the configured base value.
- The ping timeout is counted only after the reset pulse finishes.
- The reset pulse drives the relay into the watchdog reset active state.
- Use `Watchdog Set Reset Active State` to change the polarity at runtime (persisted in EEPROM).

## Power Cycle (Battery Mode)

- The relay keeps the system powered on and waits for the master to send `Power Cycle Sleep`.
- `Power Cycle Sleep` immediately powers off the system for the specified duration and then powers it back on.
- If the master fails to send a sleep command within `max_on_sec` (fallback for stuck master MCU), the relay powers off for the last configured off duration.
- If `sleep_enable` command flag is set during `Power Cycle Enable` command execution, the module enters low-power sleep during the off interval.
- `Power Cycle Sleep` returns `ERR` if power cycle mode is not enabled.

## Device Identification

- `Device Info` returns vendor and product identifiers plus device revision and firmware version.

## Command Semantics

- `Relay On/Off`: set a relay output immediately and cancel any pending timer for that relay.
- `Relay On/Off For`: set a relay output now, then revert after `duration_sec` (not persisted to EEPROM).
- `Watchdog Enable`: arms the watchdog on the given relay and starts the ping timer.
- `Watchdog Disable`: disarms the watchdog and releases the relay.
- `Watchdog Ping`: resets the ping timer to the base timeout.
- `Watchdog Set Ping Timeout`: sets the base timeout (seconds) for watchdog ping.
- `Watchdog Set Reset Duration`: sets the reset pulse duration (seconds).
- `Watchdog Set/Get Reset Active State`: selects whether relay ON or OFF represents a reset pulse.
- `Watchdog Get/Clear Trip Count`: read or clear the persisted watchdog trip counter.
- `EEPROM Clear`: resets all settings and counters to defaults.
- `Power Cycle Enable`: enables battery mode; `sleep_enable=1` allows ATtiny85 to sleep during off time.
- `Power Cycle Disable`: disables battery mode.
- `Power Cycle Set Max On Time`: sets the maximum allowed on-time before a forced sleep.
- `Power Cycle Sleep`: immediately powers off for `off_sec` seconds.
- `Relay State Persist Enable/Disable/Get`: controls EEPROM persistence of relay states.
- `Relay Get State`: returns two bitmasks: `state_mask` (1=ON) and `init_mask` (1=initialized). If a relay bit is 0 in `init_mask`, its state is undefined.
- `I2C Set Address`: stores new 7-bit I2C address (0x08–0x77, applied after power reset).
- `EEPROM Get Write Count`: returns the total number of EEPROM write cycles performed by firmware (not cleared by `EEPROM Clear`).
- `EEPROM Get Shift Count`: returns how many times the EEPROM data slot has shifted.
- `Firmware Get Version`: returns the firmware version constant (`FIRMWARE_VERSION`).
- `EEPROM Get Version`: returns the EEPROM layout version constant (`EEPROM_VERSION`).
- `Device Info`: returns vendor/product identity plus firmware revision for discovery.
