/*
  Watchdog
  --------
  Demonstrates the hardware watchdog behavior. The Smart Relay I2C module expects
  periodic pings; if pings stop, it will pulse the relay to reset power
  on the attached MCU.
*/

#include <Wire.h>
#include <SmartRelay.h>

SmartRelay relay(0x2A);

void setup() {
  relay.begin(Wire, 100000);

  // Configure watchdog: 10s ping timeout, 2s reset pulse.
  relay.watchdogSetPingTimeout(10);
  relay.watchdogSetResetDuration(2);

  // Select which relay controls the reset pulse.
  relay.watchdogEnable(0);
}

void loop() {
  // Keep the watchdog happy by pinging every 5 seconds.
  relay.watchdogPing();
  delay(5000);
}
