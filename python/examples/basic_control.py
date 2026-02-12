"""Basic relay control example for Smart Relay I2C module on Linux I2C."""

from smbus2 import SMBus
from smartrelay import SmartRelay
import time


def main():
    with SMBus(1) as bus:
        relay = SmartRelay(bus, address=0x2A)

        relay.relay_on(0)
        time.sleep(1)

        relay.relay_off(0)
        time.sleep(1)

        relay.relay_on_for(0, 2)


if __name__ == "__main__":
    main()
