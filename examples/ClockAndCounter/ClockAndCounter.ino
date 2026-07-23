/*
  ClockAndCounter.ino - Digital Clock and Real-Time Counter Example
  Library: FD1642_TVReceiverDisplay
*/

#include <FD1642_TVReceiverDisplay.h>

#define CLK_PIN  6
#define DATA_PIN 7

FD1642_TVReceiverDisplay display(CLK_PIN, DATA_PIN);

void setup() {
  display.begin();
}

void loop() {
  // Demo 1: Clock (12:30 -> 12:35)
  for (int min = 30; min <= 35; min++) {
    display.showClock(12, min);
    unsigned long tStart = millis();
    while (millis() - tStart < 1000) {
      display.refresh();
    }
  }

  // Demo 2: Up-Counter (0000 to 0100)
  for (int count = 0; count <= 100; count += 2) {
    display.showNumber(count);
    unsigned long cStart = millis();
    while (millis() - cStart < 100) {
      display.refresh();
    }
  }
}
