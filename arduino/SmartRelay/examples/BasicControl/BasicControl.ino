/*
  BasicControl
  ------------
  Simple example that toggles a relay ON/OFF and then uses a timed ON.
  Use this as a quick sanity check that I2C wiring and addressing works.
*/

#include <Wire.h>
#include <SmartRelay.h>

// Default Smart Relay I2C address is 0x2A.
SmartRelay relay(0x2A);

void setup() {
  // Start I2C at 100 kHz. Use 50 kHz if you have long wires or weak pull-ups.
  relay.begin(Wire, 100000);

  // Turn relay 0 ON for 1 second, then OFF for 1 second.
  relay.relayOn(0);
  delay(1000);

  relay.relayOff(0);
  delay(1000);

  // Turn relay 0 ON for 2 seconds, then it will revert automatically.
  relay.relayOnFor(0, 2);
}

void loop() {
  // Nothing to do here for this simple demo.
}
