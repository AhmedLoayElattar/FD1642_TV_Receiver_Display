/*
  FD1642_TVReceiverDisplay.cpp - Advanced Implementation for FD1642 / CT1642 Display Library.
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
  for (int i = 0; i < 4; i++) {
    _rawFrames[i] = DIGIT_SEGMENTS[0];
  }
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

// -------------------------------------------------------------------
// Complete Alphanumeric & ASCII Symbol Pattern Generator (A-Z, 0-9)
// -------------------------------------------------------------------
uint32_t FD1642_TVReceiverDisplay::getCharPattern(char c) {
  if (c >= '0' && c <= '9') {
    return DIGIT_SEGMENTS[c - '0'];
  }

  // Convert lowercase to uppercase for uniform matching
  if (c >= 'a' && c <= 'z') {
    c = c - 'a' + 'A';
  }

  switch (c) {
    case 'A': return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'B': return SEG_C | SEG_D | SEG_E | SEG_F | SEG_G; // 'b'
    case 'C': return SEG_A | SEG_D | SEG_E | SEG_F;
    case 'D': return SEG_B | SEG_C | SEG_D | SEG_E | SEG_G; // 'd'
    case 'E': return SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;
    case 'F': return SEG_A | SEG_E | SEG_F | SEG_G;
    case 'G': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    case 'H': return SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'I': return SEG_B | SEG_C;
    case 'J': return SEG_B | SEG_C | SEG_D | SEG_E;
    case 'K': return SEG_A | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'L': return SEG_D | SEG_E | SEG_F;
    case 'M': return SEG_A | SEG_C | SEG_E;
    case 'N': return SEG_C | SEG_E | SEG_G; // 'n'
    case 'O': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case 'P': return SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;
    case 'Q': return SEG_A | SEG_B | SEG_C | SEG_F | SEG_G;
    case 'R': return SEG_E | SEG_G; // 'r'
    case 'S': return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
    case 'T': return SEG_D | SEG_E | SEG_F | SEG_G; // 't'
    case 'U': return SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case 'V': return SEG_C | SEG_D | SEG_E; // 'v'
    case 'W': return SEG_B | SEG_C | SEG_D | SEG_E | SEG_F; // 'W' (rendered as U-shaped 7-segment font)
    case 'X': return SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'Y': return SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    case 'Z': return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
    case '-': return SEG_G;
    case '_': return SEG_D;
    case '=': return SEG_D | SEG_G;
    case '[': return SEG_A | SEG_D | SEG_E | SEG_F;
    case ']': return SEG_A | SEG_B | SEG_C | SEG_D;
    case ' ': default: return 0UL;
  }
}

// -------------------------------------------------------------------
// Digit & String Setters
// -------------------------------------------------------------------
void FD1642_TVReceiverDisplay::showDigits(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) {
  _rawFrames[0] = DIGIT_SEGMENTS[d1 % 10];
  _rawFrames[1] = DIGIT_SEGMENTS[d2 % 10];
  _rawFrames[2] = DIGIT_SEGMENTS[d3 % 10];
  _rawFrames[3] = DIGIT_SEGMENTS[d4 % 10];
}

void FD1642_TVReceiverDisplay::setDigit(uint8_t index, uint8_t value) {
  if (index < 4) {
    _rawFrames[index] = DIGIT_SEGMENTS[value % 10];
  }
}

void FD1642_TVReceiverDisplay::setDigit1(uint8_t value) { setDigit(0, value); }
void FD1642_TVReceiverDisplay::setDigit2(uint8_t value) { setDigit(1, value); }
void FD1642_TVReceiverDisplay::setDigit3(uint8_t value) { setDigit(2, value); }
void FD1642_TVReceiverDisplay::setDigit4(uint8_t value) { setDigit(3, value); }

void FD1642_TVReceiverDisplay::showNumber(uint16_t value) {
  _rawFrames[0] = DIGIT_SEGMENTS[(value / 1000) % 10];
  _rawFrames[1] = DIGIT_SEGMENTS[(value / 100) % 10];
  _rawFrames[2] = DIGIT_SEGMENTS[(value / 10) % 10];
  _rawFrames[3] = DIGIT_SEGMENTS[value % 10];
}

void FD1642_TVReceiverDisplay::showClock(uint8_t hours, uint8_t minutes) {
  _rawFrames[0] = DIGIT_SEGMENTS[(hours / 10) % 10];
  _rawFrames[1] = DIGIT_SEGMENTS[hours % 10];
  _rawFrames[2] = DIGIT_SEGMENTS[(minutes / 10) % 10];
  _rawFrames[3] = DIGIT_SEGMENTS[minutes % 10];
}

void FD1642_TVReceiverDisplay::showString(const char* str) {
  uint8_t len = strlen(str);
  for (int i = 0; i < 4; i++) {
    if (i < len) {
      _rawFrames[i] = getCharPattern(str[i]);
    } else {
      _rawFrames[i] = 0UL;
    }
  }
}

void FD1642_TVReceiverDisplay::setChar(uint8_t index, char c) {
  if (index < 4) {
    _rawFrames[index] = getCharPattern(c);
  }
}

void FD1642_TVReceiverDisplay::scrollText(const char* text, uint16_t speedMs) {
  uint8_t len = strlen(text);
  if (len == 0) return;

  for (int i = 0; i < len; i++) {
    char buf[5] = {' ', ' ', ' ', ' ', '\0'};
    for (int j = 0; j < 4; j++) {
      if (i + j < len) {
        buf[j] = text[i + j];
      }
    }
    showString(buf);
    unsigned long stepStart = millis();
    while (millis() - stepStart < speedMs) {
      refresh();
    }
  }
}

// -------------------------------------------------------------------
// ANIMATIONS & LOADING SCREENS
// -------------------------------------------------------------------

// Rotating Spinner Animation (A->B->C->D->E->F clockwise around each digit)
void FD1642_TVReceiverDisplay::animLoadingSpinner(uint16_t durationMs, uint16_t speedMs) {
  uint32_t spinnerSegs[6] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F};
  unsigned long start = millis();
  uint8_t step = 0;

  while (millis() - start < durationMs) {
    for (int i = 0; i < 4; i++) {
      _rawFrames[i] = spinnerSegs[step % 6];
    }
    step++;

    unsigned long stepStart = millis();
    while (millis() - stepStart < speedMs) {
      refresh();
    }
  }
}

// Chasing Outer Wave Animation
void FD1642_TVReceiverDisplay::animWave(uint16_t cycles, uint16_t speedMs) {
  struct WaveStep { uint8_t digit; uint32_t seg; };
  WaveStep path[] = {
    {0, SEG_A}, {1, SEG_A}, {2, SEG_A}, {3, SEG_A},
    {3, SEG_B}, {3, SEG_C},
    {3, SEG_D}, {2, SEG_D}, {1, SEG_D}, {0, SEG_D},
    {0, SEG_E}, {0, SEG_F}
  };
  uint8_t pathLen = sizeof(path) / sizeof(path[0]);

  for (uint16_t c = 0; c < cycles; c++) {
    for (uint8_t p = 0; p < pathLen; p++) {
      for (int d = 0; d < 4; d++) _rawFrames[d] = 0UL;
      _rawFrames[path[p].digit] = path[p].seg;

      unsigned long stepStart = millis();
      while (millis() - stepStart < speedMs) {
        refresh();
      }
    }
  }
}

// Bouncing Dash Animation
void FD1642_TVReceiverDisplay::animBounce(uint16_t bounces, uint16_t speedMs) {
  for (uint16_t b = 0; b < bounces; b++) {
    // Left to Right
    for (int i = 0; i < 4; i++) {
      for (int d = 0; d < 4; d++) _rawFrames[d] = 0UL;
      _rawFrames[i] = SEG_G; // Dash
      unsigned long stepStart = millis();
      while (millis() - stepStart < speedMs) refresh();
    }
    // Right to Left
    for (int i = 2; i > 0; i--) {
      for (int d = 0; d < 4; d++) _rawFrames[d] = 0UL;
      _rawFrames[i] = SEG_G;
      unsigned long stepStart = millis();
      while (millis() - stepStart < speedMs) refresh();
    }
  }
}

// Slot Machine Roll Animation
void FD1642_TVReceiverDisplay::animSlotMachine(uint16_t targetNumber, uint16_t durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    for (int i = 0; i < 4; i++) {
      _rawFrames[i] = DIGIT_SEGMENTS[random(0, 10)];
    }
    unsigned long stepStart = millis();
    while (millis() - stepStart < 60) {
      refresh();
    }
  }
  showNumber(targetNumber);
  unsigned long landStart = millis();
  while (millis() - landStart < 1000) {
    refresh();
  }
}

// Fill and Wipe Animation
void FD1642_TVReceiverDisplay::animFillWipe(uint16_t speedMs) {
  uint32_t ALL_SEGS = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
  
  // Fill Left to Right
  for (int i = 0; i < 4; i++) {
    _rawFrames[i] = ALL_SEGS;
    unsigned long stepStart = millis();
    while (millis() - stepStart < speedMs) refresh();
  }
  // Wipe Left to Right
  for (int i = 0; i < 4; i++) {
    _rawFrames[i] = 0UL;
    unsigned long stepStart = millis();
    while (millis() - stepStart < speedMs) refresh();
  }
}

// -------------------------------------------------------------------
// System Utilities
// -------------------------------------------------------------------
void FD1642_TVReceiverDisplay::clear() {
  for (int i = 0; i < 4; i++) {
    _rawFrames[i] = 0UL;
  }
  send18BitData(0x7FFUL); // Turn off grids
}

void FD1642_TVReceiverDisplay::refresh() {
  uint32_t baseGrids = 0x7FFUL;

  for (int i = 0; i < 4; i++) {
    uint8_t gridBit = DIGIT_GRIDS[i];
    uint32_t gridMask = baseGrids & ~(1UL << gridBit);
    uint32_t frame = _rawFrames[i] | gridMask;
    send18BitData(frame);
    delayMicroseconds(1500);
  }
}
