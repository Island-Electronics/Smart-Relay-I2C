/*
  Serial Console tool for Smart Relay I2C module
  --------
  Powerful interactive console for controlling and diagnosing the Smart Relay I2C module over a serial connection. 
  This sketch listens for commands on the serial port, executes them using the SmartRelay library, and prints results back to the console. 
  It supports all features of the Smart Relay, including relay control, watchdog configuration, power cycle mode, and diagnostics.
*/

#include <Wire.h>
#include <SmartRelay.h>
#include <stdlib.h>
#include <string.h>

SmartRelay relay(0x2A);

static char line_buf[64];
static uint8_t line_len = 0;

static void printHelp() {
  Serial.println(F("Smart Relay - Serial Console"));
  Serial.println(F("Commands: -------------"));
  Serial.println(F("- Controls:"));
  Serial.println(F("    on <relay>"));
  Serial.println(F("    off <relay>"));
  Serial.println(F("    on_for <relay> <sec>"));
  Serial.println(F("    off_for <relay> <sec>"));
  Serial.println(F("- Watchdog mode:"));
  Serial.println(F("    wd_enable <relay>"));
  Serial.println(F("    wd_disable"));
  Serial.println(F("    wd_ping"));
  Serial.println(F("    wd_set_timeout <sec>"));
  Serial.println(F("    wd_set_reset <sec>"));
  Serial.println(F("    wd_set_active_state <0|1>"));
  Serial.println(F("    wd_get_active_state"));
  Serial.println(F("    wd_get_count"));
  Serial.println(F("    wd_clear_count"));
  Serial.println(F("- Power Cycle mode:"));
  Serial.println(F("    pc_enable <relay> [sleep 0|1]"));
  Serial.println(F("    pc_disable"));
  Serial.println(F("    pc_set_max_on <sec>"));
  Serial.println(F("    pc_sleep <sec>"));
  Serial.println(F("- Relay persist settings:"));
  Serial.println(F("    relay_persist_on"));
  Serial.println(F("    relay_persist_off"));
  Serial.println(F("    relay_persist_get"));
  Serial.println(F("- Tools:"));
  Serial.println(F("    relay_get_state"));
  Serial.println(F("    i2c_set_addr <addr> (applies after reset)"));
  Serial.println(F("    eeprom_get_wcount"));
  Serial.println(F("    eeprom_get_shift"));
  Serial.println(F("    fw_get_version"));
  Serial.println(F("    eeprom_get_version"));
  Serial.println(F("    device_info"));
  Serial.println(F("    eeprom_clear"));
  Serial.println(F("    help"));
  Serial.println(F("-----------------------"));
}

static bool parse_u16(const char *str, uint16_t &out) {
  if (str == nullptr) {
    return false;
  }
  char *endptr = nullptr;
  unsigned long value = strtoul(str, &endptr, 10);
  if (endptr == str) {
    return false;
  }
  if (value > 65535UL) {
    return false;
  }
  out = (uint16_t)value;
  return true;
}

static bool parse_u8(const char *str, uint8_t &out) {
  uint16_t temp = 0;
  if (!parse_u16(str, temp)) {
    return false;
  }
  if (temp > 255) {
    return false;
  }
  out = (uint8_t)temp;
  return true;
}

static void printResult(bool ok) {
  Serial.println(ok ? "OK" : "ERR");
}

static void handleLine(char *line) {
  char *cmd = strtok(line, " ");
  if (!cmd) {
    return;
  }

  if (strcmp(cmd, "help") == 0) {
    printHelp();
    return;
  }

  if (strcmp(cmd, "on") == 0) {
    uint8_t relay_id;
    if (!parse_u8(strtok(nullptr, " "), relay_id)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.relayOn(relay_id));
    return;
  }

  if (strcmp(cmd, "off") == 0) {
    uint8_t relay_id;
    if (!parse_u8(strtok(nullptr, " "), relay_id)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.relayOff(relay_id));
    return;
  }

  if (strcmp(cmd, "on_for") == 0) {
    uint8_t relay_id;
    uint16_t sec;
    if (!parse_u8(strtok(nullptr, " "), relay_id)) { Serial.println("BAD_PARAM"); return; }
    if (!parse_u16(strtok(nullptr, " "), sec)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.relayOnFor(relay_id, sec));
    return;
  }

  if (strcmp(cmd, "off_for") == 0) {
    uint8_t relay_id;
    uint16_t sec;
    if (!parse_u8(strtok(nullptr, " "), relay_id)) { Serial.println("BAD_PARAM"); return; }
    if (!parse_u16(strtok(nullptr, " "), sec)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.relayOffFor(relay_id, sec));
    return;
  }

  if (strcmp(cmd, "wd_enable") == 0) {
    uint8_t relay_id;
    if (!parse_u8(strtok(nullptr, " "), relay_id)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.watchdogEnable(relay_id));
    return;
  }

  if (strcmp(cmd, "wd_disable") == 0) {
    printResult(relay.watchdogDisable());
    return;
  }

  if (strcmp(cmd, "wd_ping") == 0) {
    printResult(relay.watchdogPing());
    return;
  }

  if (strcmp(cmd, "wd_set_timeout") == 0) {
    uint16_t sec;
    if (!parse_u16(strtok(nullptr, " "), sec)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.watchdogSetPingTimeout(sec));
    return;
  }

  if (strcmp(cmd, "wd_set_reset") == 0) {
    uint16_t sec;
    if (!parse_u16(strtok(nullptr, " "), sec)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.watchdogSetResetDuration(sec));
    return;
  }

  if (strcmp(cmd, "wd_set_active_state") == 0) {
    uint8_t state = 0;
    if (!parse_u8(strtok(nullptr, " "), state)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.watchdogSetResetActiveState(state));
    return;
  }

  if (strcmp(cmd, "wd_get_active_state") == 0) {
    uint8_t state = 0;
    if (relay.watchdogGetResetActiveState(state)) {
      Serial.print("RESET_ACTIVE_STATE ");
      Serial.println(state ? "1" : "0");
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "wd_get_count") == 0) {
    uint32_t count = 0;
    if (relay.watchdogGetTripCount(count)) {
      Serial.print("COUNT ");
      Serial.println(count);
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "wd_clear_count") == 0) {
    printResult(relay.watchdogClearTripCount());
    return;
  }

  if (strcmp(cmd, "eeprom_clear") == 0) {
    printResult(relay.eepromClear());
    return;
  }

  if (strcmp(cmd, "pc_enable") == 0) {
    uint8_t relay_id;
    if (!parse_u8(strtok(nullptr, " "), relay_id)) { Serial.println("BAD_PARAM"); return; }
    char *sleep_arg = strtok(nullptr, " ");
    if (sleep_arg) {
      uint8_t sleep_en = 0;
      if (!parse_u8(sleep_arg, sleep_en)) { Serial.println("BAD_PARAM"); return; }
      printResult(relay.powerCycleEnable(relay_id, sleep_en != 0));
    } else {
      printResult(relay.powerCycleEnable(relay_id));
    }
    return;
  }

  if (strcmp(cmd, "pc_disable") == 0) {
    printResult(relay.powerCycleDisable());
    return;
  }

  if (strcmp(cmd, "pc_set_max_on") == 0) {
    uint16_t sec;
    if (!parse_u16(strtok(nullptr, " "), sec)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.powerCycleSetMaxOnTime(sec));
    return;
  }

  if (strcmp(cmd, "pc_sleep") == 0) {
    uint16_t sec;
    if (!parse_u16(strtok(nullptr, " "), sec)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.powerCycleSleep(sec));
    return;
  }

  if (strcmp(cmd, "relay_persist_on") == 0) {
    printResult(relay.relayStatePersistEnable());
    return;
  }

  if (strcmp(cmd, "relay_persist_off") == 0) {
    printResult(relay.relayStatePersistDisable());
    return;
  }

  if (strcmp(cmd, "relay_persist_get") == 0) {
    bool enabled = false;
    if (relay.relayStatePersistGet(enabled)) {
      Serial.print("PERSIST ");
      Serial.println(enabled ? "1" : "0");
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "relay_get_state") == 0) {
    uint8_t state_mask = 0;
    uint8_t init_mask = 0;
    if (relay.relayGetState(state_mask, init_mask)) {
      Serial.print("STATE MASK 0x");
      Serial.print(state_mask, HEX);
      Serial.print(" | INIT MASK 0x");
      Serial.println(init_mask, HEX);
      for (uint8_t i = 0; i < 8; i++) {
        bool init = (init_mask & (1U << i)) != 0;
        Serial.print("R");
        Serial.print(i);
        Serial.print(": ");
        if (!init) {
          Serial.println("UNINIT");
          continue;
        }
        bool on = (state_mask & (1U << i)) != 0;
        Serial.println(on ? "ON" : "OFF");
      }
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "i2c_set_addr") == 0) {
    uint8_t addr;
    if (!parse_u8(strtok(nullptr, " "), addr)) { Serial.println("BAD_PARAM"); return; }
    printResult(relay.i2cSetAddress(addr));
    return;
  }

  if (strcmp(cmd, "eeprom_get_wcount") == 0) {
    uint32_t count = 0;
    if (relay.eepromGetWriteCount(count)) {
      Serial.print("EEPROM_WRITES ");
      Serial.println(count);
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "eeprom_get_shift") == 0) {
    uint8_t count = 0;
    if (relay.eepromGetShiftCount(count)) {
      Serial.print("EEPROM_SHIFTS ");
      Serial.println(count);
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "fw_get_version") == 0) {
    uint16_t version = 0;
    if (relay.firmwareGetVersion(version)) {
      Serial.print("FIRMWARE_VERSION 0x");
      Serial.println(version, HEX);
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "eeprom_get_version") == 0) {
    uint8_t version = 0;
    if (relay.eepromGetVersion(version)) {
      Serial.print("EEPROM_VERSION 0x");
      Serial.println(version, HEX);
    } else {
      Serial.println("ERR");
    }
    return;
  }

  if (strcmp(cmd, "device_info") == 0) {
    uint16_t vendor = 0;
    uint16_t product = 0;
    uint8_t rev = 0;
    uint16_t fw = 0;
    if (relay.deviceInfo(vendor, product, rev, fw)) {
      Serial.print("VENDOR 0x");
      Serial.print(vendor, HEX);
      Serial.print(" PRODUCT 0x");
      Serial.print(product, HEX);
      Serial.print(" REV ");
      Serial.print(rev, DEC);
      Serial.print(" FW 0x");
      Serial.println(fw, HEX);
    } else {
      Serial.println("ERR");
    }
    return;
  }

  Serial.println("BAD_CMD");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  relay.begin(Wire, 50000);
  while (!Serial) {
    ;  // wait for serial port to connect - sketch has no use without it
  }
  digitalWrite(LED_BUILTIN, HIGH);
  printHelp();
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r' || c == '\n') {
      if (line_len > 0) {
        line_buf[line_len] = '\0';
        handleLine(line_buf);
        line_len = 0;
      }
    } else if (line_len < sizeof(line_buf) - 1) {
      line_buf[line_len++] = c;
    }
  }
}
