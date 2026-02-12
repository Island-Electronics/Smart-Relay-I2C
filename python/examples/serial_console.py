#!/usr/bin/env python3
"""
Smart Relay I2C module - CLI tool for interactive control and diagnostics over Linux I2C (e.g., Raspberry Pi).
--------------------------------
Interactive console that exposes the full Smart Relay I2C module protocol over stdin.
Useful for configuration, testing, and diagnostics on Linux (Raspberry Pi).
"""

import argparse
import shlex
from smbus2 import SMBus
from smartrelay import SmartRelay


def print_help():
    print("Smart Relay - Python Console")
    print("Commands: -------------")
    print("- Controls:")
    print("    on <relay>")
    print("    off <relay>")
    print("    on_for <relay> <sec>")
    print("    off_for <relay> <sec>")
    print("- Watchdog mode:")
    print("    wd_enable <relay>")
    print("    wd_disable")
    print("    wd_ping")
    print("    wd_set_timeout <sec>")
    print("    wd_set_reset <sec>")
    print("    wd_set_active_state <0|1>")
    print("    wd_get_active_state")
    print("    wd_get_count")
    print("    wd_clear_count")
    print("- Power Cycle mode:")
    print("    pc_enable <relay> [sleep 0|1]")
    print("    pc_disable")
    print("    pc_set_max_on <sec>")
    print("    pc_sleep <sec>")
    print("- Relay persist settings:")
    print("    relay_persist_on")
    print("    relay_persist_off")
    print("    relay_persist_get")
    print("- Tools:")
    print("    relay_get_state")
    print("    i2c_set_addr <addr> (applies after reset)")
    print("    eeprom_get_wcount")
    print("    eeprom_get_shift")
    print("    fw_get_version")
    print("    eeprom_get_version")
    print("    device_info")
    print("    eeprom_clear")
    print("    help")
    print("    exit")
    print("-----------------------")


def parse_int(value):
    return int(value, 0)


def print_result(ok):
    print("OK" if ok else "ERR")


def handle_command(relay, tokens):
    if not tokens:
        return

    cmd = tokens[0]

    if cmd == "help":
        print_help()
        return
    if cmd == "exit":
        raise SystemExit(0)

    try:
        if cmd == "on":
            print_result(relay.relay_on(parse_int(tokens[1])))
        elif cmd == "off":
            print_result(relay.relay_off(parse_int(tokens[1])))
        elif cmd == "on_for":
            print_result(relay.relay_on_for(parse_int(tokens[1]), parse_int(tokens[2])))
        elif cmd == "off_for":
            print_result(relay.relay_off_for(parse_int(tokens[1]), parse_int(tokens[2])))
        elif cmd == "wd_enable":
            print_result(relay.watchdog_enable(parse_int(tokens[1])))
        elif cmd == "wd_disable":
            print_result(relay.watchdog_disable())
        elif cmd == "wd_ping":
            print_result(relay.watchdog_ping())
        elif cmd == "wd_set_timeout":
            print_result(relay.watchdog_set_ping_timeout(parse_int(tokens[1])))
        elif cmd == "wd_set_reset":
            print_result(relay.watchdog_set_reset_duration(parse_int(tokens[1])))
        elif cmd == "wd_set_active_state":
            print_result(relay.watchdog_set_reset_active_state(parse_int(tokens[1])))
        elif cmd == "wd_get_active_state":
            state = relay.watchdog_get_reset_active_state()
            if state is None:
                print("ERR")
            else:
                print(f"RESET_ACTIVE_STATE {state}")
        elif cmd == "wd_get_count":
            count = relay.watchdog_get_trip_count()
            print("ERR" if count is None else f"COUNT {count}")
        elif cmd == "wd_clear_count":
            print_result(relay.watchdog_clear_trip_count())
        elif cmd == "pc_enable":
            relay_id = parse_int(tokens[1])
            sleep_enable = parse_int(tokens[2]) if len(tokens) > 2 else 0
            print_result(relay.power_cycle_enable(relay_id, sleep_enable != 0))
        elif cmd == "pc_disable":
            print_result(relay.power_cycle_disable())
        elif cmd == "pc_set_max_on":
            print_result(relay.power_cycle_set_max_on_time(parse_int(tokens[1])))
        elif cmd == "pc_sleep":
            print_result(relay.power_cycle_sleep(parse_int(tokens[1])))
        elif cmd == "relay_persist_on":
            print_result(relay.relay_state_persist_enable())
        elif cmd == "relay_persist_off":
            print_result(relay.relay_state_persist_disable())
        elif cmd == "relay_persist_get":
            enabled = relay.relay_state_persist_get()
            print("ERR" if enabled is None else f"PERSIST {1 if enabled else 0}")
        elif cmd == "relay_get_state":
            state = relay.relay_get_state()
            if state is None:
                print("ERR")
            else:
                state_mask, init_mask = state
                print(f"STATE MASK 0x{state_mask:02X} | INIT MASK 0x{init_mask:02X}")
                for i in range(8):
                    init = (init_mask >> i) & 1
                    if not init:
                        print(f"R{i}: UNINIT")
                    else:
                        on = (state_mask >> i) & 1
                        print(f"R{i}: {'ON' if on else 'OFF'}")
        elif cmd == "i2c_set_addr":
            print_result(relay.i2c_set_address(parse_int(tokens[1])))
        elif cmd == "eeprom_get_wcount":
            count = relay.eeprom_get_write_count()
            print("ERR" if count is None else f"EEPROM_WRITES {count}")
        elif cmd == "eeprom_get_shift":
            count = relay.eeprom_get_shift_count()
            print("ERR" if count is None else f"EEPROM_SHIFTS {count}")
        elif cmd == "fw_get_version":
            version = relay.firmware_get_version()
            print("ERR" if version is None else f"FIRMWARE_VERSION 0x{version:04X}")
        elif cmd == "eeprom_get_version":
            version = relay.eeprom_get_version()
            print("ERR" if version is None else f"EEPROM_VERSION 0x{version:02X}")
        elif cmd == "device_info":
            info = relay.device_info()
            if info is None:
                print("ERR")
            else:
                vendor, product, rev, fw = info
                print(f"VENDOR 0x{vendor:04X} PRODUCT 0x{product:04X} REV {rev} FW 0x{fw:04X}")
        elif cmd == "eeprom_clear":
            print_result(relay.eeprom_clear())
        else:
            print("BAD_CMD")
    except (IndexError, ValueError):
        print("BAD_PARAM")


def main():
    parser = argparse.ArgumentParser(description="Smart Relay Python Console")
    parser.add_argument("--bus", type=int, default=1, help="I2C bus number (default: 1)")
    parser.add_argument("--addr", type=lambda v: int(v, 0), default=0x2A, help="I2C address (default: 0x2A)")
    args = parser.parse_args()

    with SMBus(args.bus) as bus:
        relay = SmartRelay(bus, address=args.addr)
        print_help()
        while True:
            try:
                line = input("> ")
            except EOFError:
                break
            tokens = shlex.split(line)
            handle_command(relay, tokens)


if __name__ == "__main__":
    main()
