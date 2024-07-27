#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t irLedPin = 4; // D2 on NodeMCU, which corresponds to GPIO14
IRsend irsend(irLedPin);

void setup() {
  irsend.begin();
}

void loop() {
  // Send the IR signal using the code received from CFD12E
  irsend.sendNEC(0xCF8976, 32); // Fan Power button
  delay(1000);
}
