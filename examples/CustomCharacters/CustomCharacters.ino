/*
  FD1642_TVReceiverDisplay - Custom Characters Example
  
  This example demonstrates how to completely redefine how specific letters 
  and numbers look on your 7-segment display, as well as how to directly
  control individual segments.

  Because 7-segment displays cannot show diagonals, letters like 'W', 'M', 
  and 'K' are always approximations. This library provides a good standard
  approximation, but your specific display glass might look better with 
  a different layout!
*/

#include <FD1642_TVReceiverDisplay.h>

// Define the connections to the driver IC
#define CLK_PIN 2
#define DATA_PIN 3

FD1642_TVReceiverDisplay display(CLK_PIN, DATA_PIN);

// Standard 7-Segment Bit Positions (0b0GFEDCBA)
// Use these to easily build custom characters!
#define S_A 0x01 // Top
#define S_B 0x02 // Top Right
#define S_C 0x04 // Bottom Right
#define S_D 0x08 // Bottom
#define S_E 0x10 // Bottom Left
#define S_F 0x20 // Top Left
#define S_G 0x40 // Middle

void setup() {
  display.begin();

  // The default for 'M' is currently a small 'n' shape (S_C | S_E | S_G), 
  // because a true M is impossible on a standard 7-segment.
  // 
  // Let's redefine 'M' to look like two tall bars with a top bar (looks like a wide A).
  display.setCustomChar('M', S_A | S_B | S_C | S_E | S_F); 

  // The default for 'W' is a 'U' shape (S_B | S_C | S_D | S_E | S_F).
  // Let's redefine 'W' to look like a small 'v' in the bottom center.
  // We'll use just the bottom segments to make it distinct.
  display.setCustomChar('W', S_C | S_D | S_E);

  // You can even redefine numbers! Let's make '0' just an empty square.
  display.setCustomChar('0', S_A | S_B | S_F | S_G); // Wait, this is just a weird shape!
}

void loop() {
  // We MUST call refresh() repeatedly to keep the display on.
  display.refresh();

  // Every 2 seconds, we'll switch what we are displaying.
  static unsigned long lastUpdate = 0;
  static int state = 0;

  if (millis() - lastUpdate > 2000) {
    lastUpdate = millis();

    if (state == 0) {
      // 1. Show our newly redefined 'M'
      // It will use our custom segment mask instead of the default!
      display.showString("M   ");
    } 
    else if (state == 1) {
      // 2. Show our newly redefined 'W'
      display.showString("W   ");
    } 
    else if (state == 2) {
      // 3. Show our custom '0'
      display.showString("0000");
    }
    else if (state == 3) {
      // 4. DIRECT SEGMENT CONTROL
      // You don't even need to use characters! You can directly set the 
      // segments of any digit using `setSegments(digit_index, segment_mask)`.
      
      // Let's draw an underscore on the left, and a top bar on the right.
      display.setSegments(0, S_D); // Digit 0: Bottom segment
      display.setSegments(1, S_G); // Digit 1: Middle segment
      display.setSegments(2, S_G); // Digit 2: Middle segment
      display.setSegments(3, S_A); // Digit 3: Top segment
    }

    state++;
    if (state > 3) {
      state = 0;
    }
  }
}
