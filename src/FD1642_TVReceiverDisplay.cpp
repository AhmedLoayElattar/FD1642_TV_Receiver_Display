/*
  FD1642_TVReceiverDisplay.cpp - Implementation for FD1642 / CT1642 Display Library.
*/

#include "FD1642_TVReceiverDisplay.h"

// Segment Bit Definitions (Bits 11..17, Active-HIGH)
#define SEG_A (1UL << 11)
#define SEG_B (1UL << 12)
#define SEG_C (1UL << 13)
#define SEG_D (1UL << 14)
#define SEG_E (1UL << 15)
#define SEG_F (1UL << 16)
#define SEG_G (1UL << 17)

static const uint32_t DIGIT_SEGMENTS[10] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,         // 0
  SEG_B | SEG_C,                                         // 1
  SEG_A | SEG_B | SEG_D | SEG_E | SEG_G,                 // 2
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,                 // 3
  SEG_B | SEG_C | SEG_F | SEG_G,                         // 4
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,                 // 5
  SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,         // 6
  SEG_A | SEG_B | SEG_C,                                 // 7
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G          // 9
};

// Mapped Active-LOW Grids: Digit 1 = Bit 4, Digit 2 = Bit 3, Digit 3 = Bit 2, Digit 4 = Bit 1
static const uint8_t DIGIT_GRIDS[4] = {4, 3, 2, 1};

FD1642_TVReceiverDisplay::FD1642_TVReceiverDisplay(uint8_t clkPin, uint8_t dataPin) {
  _clkPin = clkPin;
  _dataPin = dataPin;
  _digits[0] = 0;
  _digits[1] = 0;
  _digits[2] = 0;
  _digits[3] = 0;
}

void FD1642_TVReceiverDisplay::begin() {
  pinMode(_clkPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);
  digitalWrite(_clkPin, LOW);
  digitalWrite(_dataPin, LOW);
}

void FD1642_TVReceiverDisplay::startCondition() {
  digitalWrite(_clkPin, HIGH);
  digitalWrite(_dataPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_dataPin, LOW);
  delayMicroseconds(10);
  digitalWrite(_clkPin, LOW);
  delayMicroseconds(10);
}

void FD1642_TVReceiverDisplay::stopCondition() {
  digitalWrite(_dataPin, LOW);
  delayMicroseconds(10);
  digitalWrite(_clkPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_dataPin, HIGH);
  delayMicroseconds(10);
}

void FD1642_TVReceiverDisplay::send18BitData(uint32_t data) {
  startCondition();
  for (int i = 0; i < 18; i++) {
    bool bitVal = (data >> i) & 0x01;
    digitalWrite(_dataPin, bitVal ? HIGH : LOW);
    delayMicroseconds(10);
    digitalWrite(_clkPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_clkPin, LOW);
    delayMicroseconds(10);
  }
  stopCondition();
}

void FD1642_TVReceiverDisplay::showDigits(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) {
  _digits[0] = d1 % 10;
  _digits[1] = d2 % 10;
  _digits[2] = d3 % 10;
  _digits[3] = d4 % 10;
}

void FD1642_TVReceiverDisplay::setDigit(uint8_t index, uint8_t value) {
  if (index < 4) {
    _digits[index] = value % 10;
  }
}

void FD1642_TVReceiverDisplay::setDigit1(uint8_t value) { setDigit(0, value); }
void FD1642_TVReceiverDisplay::setDigit2(uint8_t value) { setDigit(1, value); }
void FD1642_TVReceiverDisplay::setDigit3(uint8_t value) { setDigit(2, value); }
void FD1642_TVReceiverDisplay::setDigit4(uint8_t value) { setDigit(3, value); }

void FD1642_TVReceiverDisplay::showNumber(uint16_t value) {
  _digits[0] = (value / 1000) % 10;
  _digits[1] = (value / 100) % 10;
  _digits[2] = (value / 10) % 10;
  _digits[3] = value % 10;
}

void FD1642_TVReceiverDisplay::showClock(uint8_t hours, uint8_t minutes) {
  _digits[0] = (hours / 10) % 10;
  _digits[1] = hours % 10;
  _digits[2] = (minutes / 10) % 10;
  _digits[3] = minutes % 10;
}

void FD1642_TVReceiverDisplay::clear() {
  send18BitData(0x7FFUL); // Turn off all grids
}

void FD1642_TVReceiverDisplay::refresh() {
  uint32_t baseGrids = 0x7FFUL;

  for (int i = 0; i < 4; i++) {
    uint8_t gridBit = DIGIT_GRIDS[i];
    uint32_t gridMask = baseGrids & ~(1UL << gridBit);
    uint32_t frame = DIGIT_SEGMENTS[_digits[i]] | gridMask;
    send18BitData(frame);
    delayMicroseconds(1500);
  }
}
