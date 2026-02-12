"""Battery power-cycle example for Smart Relay I2C module on Linux."""

from smbus2 import SMBus
from smartrelay import SmartRelay
import time


def main():
    with SMBus(1) as bus:
        relay = SmartRelay(bus, address=0x2A)

        relay.power_cycle_set_max_on_time(20)
        relay.power_cycle_enable(0, sleep_enable=True)

        # Do work while power is ON.
        time.sleep(2)

        # Request power-off for 5 minutes.
        relay.power_cycle_sleep(300)


if __name__ == "__main__":
    main()
