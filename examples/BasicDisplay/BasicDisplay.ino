/*
  BasicDisplay.ino - Basic 4-Digit Display Example
  Library: FD1642_TVReceiverDisplay
*/

#include <FD1642_TVReceiverDisplay.h>

#define CLK_PIN  6
#define DATA_PIN 7

FD1642_TVReceiverDisplay display(CLK_PIN, DATA_PIN);

void setup() {
  display.begin();
  display.showDigits(1, 2, 3, 4); // Display '1 2 3 4'
}

void loop() {
  display.refresh();
}
