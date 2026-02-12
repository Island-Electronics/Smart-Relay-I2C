"""Watchdog example for Smart Relay I2C module on Linux."""

from smbus2 import SMBus
from smartrelay import SmartRelay
import time


def main():
    with SMBus(1) as bus:
        relay = SmartRelay(bus, address=0x2A)

        relay.watchdog_set_ping_timeout(10)
        relay.watchdog_set_reset_duration(2)
        relay.watchdog_enable(0)

        while True:
            relay.watchdog_ping()
            time.sleep(5)


if __name__ == "__main__":
    main()
