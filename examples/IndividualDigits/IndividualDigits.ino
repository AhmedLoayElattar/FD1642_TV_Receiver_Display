/*
  IndividualDigits.ino - Individual Digit Setter Example
  Library: FD1642_TVReceiverDisplay
*/

#include <FD1642_TVReceiverDisplay.h>

#define CLK_PIN  6
#define DATA_PIN 7

FD1642_TVReceiverDisplay display(CLK_PIN, DATA_PIN);

void setup() {
  display.begin();

  // Set each digit individually
  display.setDigit1(9);
  display.setDigit2(8);
  display.setDigit3(7);
  display.setDigit4(6);
}

void loop() {
  display.refresh();
}
