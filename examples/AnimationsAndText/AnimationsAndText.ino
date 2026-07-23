/*
  AnimationsAndText.ino - Text, Scrolling & Animations Example
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
  // 1. Text Display ("PLAY", "LOAD", "COOL", "DONE")
  display.showString("PLAY");
  delayDisplay(2000);

  // 2. Chasing Ring Loading Spinner
  display.animLoadingSpinner(3000, 70); // 3 sec spinner, 70ms step

  // 3. Text Display
  display.showString("LOAD");
  delayDisplay(1500);

  // 4. Perimeter Wave Animation
  display.animWave(3, 60); // 3 wave cycles, 60ms step

  // 5. Bouncing Dash Animation
  display.animBounce(4, 90); // 4 bounces, 90ms speed

  // 6. Slot Machine Roll
  display.animSlotMachine(7777, 2000); // Rolls to 7777
  delayDisplay(1500);

  // 7. Fill & Wipe
  display.animFillWipe(120);

  // 8. Text Scrolling
  display.scrollText("  HELLO WORLD FROM ARDUINO MEGA  ", 200);
}

void delayDisplay(uint16_t durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    display.refresh();
  }
}
