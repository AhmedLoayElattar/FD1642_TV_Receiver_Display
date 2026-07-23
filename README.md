# FD1642_TVReceiverDisplay

An Arduino C++ library for driving salvaged **FD1642**, **CT1642**, **SM1642**, and **ET6227** 4-digit 7-segment LED display modules harvested from TV receivers, set-top boxes, and DVD players.

[![Arduino IDE Compatible](https://img.shields.io/badge/Arduino-Compatible-00979D.svg)](https://www.arduino.cc/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

## 🌟 Overview

Front panel boards from scavenged set-top boxes and TV receivers often feature a clear 4-digit 7-segment LED display driven by an **FD1642** or **CT1642** serial IC. This library handles the 18-bit serial bit-banging protocol and Active-LOW grid multiplexing scheme to provide an easy, high-level API for Arduino microcontrollers (Arduino Mega, UNO, Nano, ESP32, STM32, etc.).

### Features:
* 🚀 **Zero External Dependencies**: Pure Bit-Bang 2-wire serial protocol (CLK & DATA).
* 🔢 **Global & Single Digit Controls**: Set all 4 digits simultaneously or modify any single digit independently.
* ⏰ **Clock & Counter Formatting**: Built-in helpers for integer rendering (`0`..`9999`) and digital time (`HH:MM`).
* ⚡ **High Performance Multiplexing**: Low overhead multiplexing engine designed for smooth, flicker-free operation.

---

## 🔌 Hardware Wiring

Connect your display module header pins to your Arduino:

| Display Pin | Arduino Pin (Example) | Description |
| :--- | :--- | :--- |
| **5V / VCC** | **5V** | 5V Power Supply |
| **GND** | **GND** | Ground |
| **CLK** | **Digital Pin 6** | Serial Clock Input |
| **DATA (DIN)** | **Digital Pin 7** | Serial Data Input |

---

## 📦 Installation

1. Copy the `FD1642_TVReceiverDisplay` folder into your Arduino libraries directory:
   * **Windows**: `Documents\Arduino\libraries\`
   * **macOS / Linux**: `~/Documents/Arduino/libraries/`
2. Restart the Arduino IDE.
3. Open examples via **File -> Examples -> FD1642_TVReceiverDisplay**.

---

## 💻 Quick Start

```cpp
#include <FD1642_TVReceiverDisplay.h>

#define CLK_PIN  6
#define DATA_PIN 7

// Instantiate the display driver
FD1642_TVReceiverDisplay display(CLK_PIN, DATA_PIN);

void setup() {
  display.begin();
  
  // Set digits: 1 2 3 4
  display.showDigits(1, 2, 3, 4);
}

void loop() {
  // Call refresh() continuously in loop() to keep LEDs lit
  display.refresh();
}
```

---

## 📚 API Reference

### Initialization
```cpp
FD1642_TVReceiverDisplay(uint8_t clkPin, uint8_t dataPin);
void begin();
```

### Digit Setters
```cpp
// Set all 4 digits at once
void showDigits(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);

// Set digit by index (0=Leftmost, 3=Rightmost)
void setDigit(uint8_t index, uint8_t value);

// Set specific single digits directly
void setDigit1(uint8_t value); // Leftmost
void setDigit2(uint8_t value); // Middle-Left
void setDigit3(uint8_t value); // Middle-Right
void setDigit4(uint8_t value); // Rightmost
```

### High-Level Utilities
```cpp
// Display 4-digit number (0 to 9999)
void showNumber(uint16_t value);

// Display digital time HH:MM
void showClock(uint8_t hours, uint8_t minutes);

// Turn off all segments
void clear();
```

### Multiplexing Engine
```cpp
// Must be called repeatedly inside loop() to refresh display
void refresh();
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
