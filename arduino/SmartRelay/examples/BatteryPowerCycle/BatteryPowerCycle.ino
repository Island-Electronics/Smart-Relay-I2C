/*
  BatteryPowerCycle
  -----------------
  Demonstrates battery-friendly power cycling. The Smart Relay keeps the
  system powered ON while your MCU does its work, then you request a sleep
  interval and the relay cuts power.
*/

#include <Wire.h>
#include <SmartRelay.h>

SmartRelay relay(0x2A);

void setup() {
  relay.begin(Wire, 100000);

  // If the MCU never calls powerCycleSleep(), the relay will force a sleep
  // after this max-on interval.
  relay.powerCycleSetMaxOnTime(20);

  // Enable power-cycle mode on relay 0, with low-power sleep enabled.
  relay.powerCycleEnable(0, true);

  // Do work while power is ON.
  delay(2000);

  // Request power-off for 5 minutes (300 seconds).
  relay.powerCycleSleep(300);
}

void loop() {
  // The MCU can enter its own sleep here to save power while it is still ON.
}
