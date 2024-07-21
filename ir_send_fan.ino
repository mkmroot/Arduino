#include <IRremote.h>

const int irLedPin = 3; // Pin connected to IR LED (or transistor base)
IRsend irsend(irLedPin);

void setup() {
  // Initialize IR sender
}

void loop() {
  // Send the IR signal using the code received from CFD12E
  irsend.sendNEC(0xCF8976, 32); //Fan Power button
  delay(50000);
  irsend.sendNEC(0xCFD12E, 32); // Example NEC code
  delay(20000); // Wait 20 seconds before sending again
}
