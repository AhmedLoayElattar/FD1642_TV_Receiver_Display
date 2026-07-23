/*
  FD1642_TVReceiverDisplay.cpp - Advanced Implementation for FD1642 / CT1642 Display Library.
*/

#include "FD1642_TVReceiverDisplay.h"
#include <string.h>

// Segment Bit Definitions (Bits 11..17, Active-HIGH)
#define SEG_A (1UL << 11)
#define SEG_B (1UL << 12)
#define SEG_C (1UL << 13)
#define SEG_D (1UL << 14)
#define SEG_E (1UL << 15)
#define SEG_F (1UL << 16)
#define SEG_G (1UL << 17)

// Standard 7-Segment Bit Positions (0b0GFEDCBA)
#define S_A 0x01
#define S_B 0x02
#define S_C 0x04
#define S_D 0x08
#define S_E 0x10
#define S_F 0x20
#define S_G 0x40

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

static const uint8_t ASCII_FONT[128] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0-15
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 16-31
  0x00, 0x86, 0x22, 0x76, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x04, 0x40, 0x08, 0x52, // 32-47
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53, // 48-63
  0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x06, 0x1E, 0x75, 0x38, 0x79, 0x54, 0x3F, // 64-79 (77='M' -> 0x79 rotated)
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x1C, 0x4F, 0x76, 0x6E, 0x5B, 0x39, 0x00, 0x0F, 0x23, 0x08, // 80-95 (87='W' -> 0x4F rotated)
  0x20, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x04, 0x0E, 0x75, 0x30, 0x79, 0x54, 0x5C, // 96-111 (109='m' -> 0x79 rotated)
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x1C, 0x4F, 0x76, 0x6E, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00  // 112-127 (119='w' -> 0x4F rotated)
};

FD1642_TVReceiverDisplay::FD1642_TVReceiverDisplay(uint8_t clkPin, uint8_t dataPin) {
  _clkPin = clkPin;
  _dataPin = dataPin;
  for (int i = 0; i < 4; i++) {
    _rawFrames[i] = DIGIT_SEGMENTS[0];
  }
  memset(_customFont, 0xFF, sizeof(_customFont));
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
// Standard Segment to Raw 18-bit Mapping
// -------------------------------------------------------------------
uint32_t FD1642_TVReceiverDisplay::mapStandardToRaw(uint8_t standardMask) {
  uint32_t raw = 0;
  if (standardMask & S_A) raw |= SEG_A;
  if (standardMask & S_B) raw |= SEG_B;
  if (standardMask & S_C) raw |= SEG_C;
  if (standardMask & S_D) raw |= SEG_D;
  if (standardMask & S_E) raw |= SEG_E;
  if (standardMask & S_F) raw |= SEG_F;
  if (standardMask & S_G) raw |= SEG_G;
  return raw;
}

// -------------------------------------------------------------------
// Complete Alphanumeric & ASCII Symbol Pattern Generator (A-Z, 0-9)
// -------------------------------------------------------------------
uint32_t FD1642_TVReceiverDisplay::getCharPattern(char c) {
  if (c < 0 || c > 127) return 0UL;
  uint8_t mask = _customFont[(uint8_t)c];
  if (mask == 0xFF) {
    mask = ASCII_FONT[(uint8_t)c];
  }
  return mapStandardToRaw(mask);
}

void FD1642_TVReceiverDisplay::setSegments(uint8_t index, uint8_t segmentMask) {
  if (index < 4) {
    _rawFrames[index] = mapStandardToRaw(segmentMask);
  }
}

void FD1642_TVReceiverDisplay::setCustomChar(char c, uint8_t segmentMask) {
  if (c >= 0 && c < 128) {
    _customFont[(uint8_t)c] = segmentMask;
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
  renderStringToFrames(str, _rawFrames, 4);
}

void FD1642_TVReceiverDisplay::setChar(uint8_t index, char c) {
  if (index < 4) {
    _rawFrames[index] = getCharPattern(c);
  }
}

void FD1642_TVReceiverDisplay::scrollText(const char* text, uint16_t speedMs) {
  uint8_t len = strlen(text);
  if (len == 0) return;

  // We need to scroll through the expanded string.
  // To handle wide chars properly in a scrolling window, we pre-expand
  // the entire string into a digit-position array.
  // Max expansion: each char could be 2 digits wide, so worst case 2*len.
  // We use dynamic-ish approach with a reasonable cap.
  uint8_t expandedLen = 0;
  uint32_t expanded[128]; // Support up to ~64 char strings (128 digit positions)
  
  for (uint8_t i = 0; i < len && expandedLen < 126; i++) {
    char c = text[i];
    char upper = c;
    if (upper >= 'a' && upper <= 'z') upper = upper - 'a' + 'A';
    
    if (isWideChar(upper)) {
      uint32_t left, right;
      getWideCharPatterns(upper, left, right);
      expanded[expandedLen++] = left;
      expanded[expandedLen++] = right;
    } else {
      expanded[expandedLen++] = getCharPattern(c);
    }
  }

  // Now scroll through the expanded array, showing 4 positions at a time
  for (uint8_t i = 0; i < expandedLen; i++) {
    for (uint8_t d = 0; d < 4; d++) {
      if (i + d < expandedLen) {
        _rawFrames[d] = expanded[i + d];
      } else {
        _rawFrames[d] = 0UL;
      }
    }
    unsigned long stepStart = millis();
    while (millis() - stepStart < speedMs) {
      refresh();
    }
  }
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// Vertical Text Scrolling (Roll Up / Roll Down)
// -------------------------------------------------------------------
void FD1642_TVReceiverDisplay::scrollTextVertical(const char* text, uint16_t speedMs, bool rollUp) {
  uint8_t len = strlen(text);
  if (len == 0) return;

  uint8_t lineCount = (len + 3) / 4;
  uint32_t currentFrames[4] = {0, 0, 0, 0};
  uint32_t nextFrames[4] = {0, 0, 0, 0};

  for (uint8_t line = 0; line < lineCount; line++) {
    for (uint8_t d = 0; d < 4; d++) {
      uint8_t charIdx = line * 4 + d;
      if (charIdx < len) {
        nextFrames[d] = getCharPattern(text[charIdx]);
      } else {
        nextFrames[d] = 0UL;
      }
    }

    for (uint8_t step = 1; step <= 3; step++) {
      for (uint8_t d = 0; d < 4; d++) {
        if (rollUp) {
          if (step == 1) {
            _rawFrames[d] = (currentFrames[d] & (SEG_A | SEG_B | SEG_F | SEG_G));
          } else if (step == 2) {
            _rawFrames[d] = (currentFrames[d] & SEG_A) | (nextFrames[d] & (SEG_D | SEG_E | SEG_C | SEG_G));
          } else {
            _rawFrames[d] = nextFrames[d];
          }
        } else {
          if (step == 1) {
            _rawFrames[d] = (currentFrames[d] & (SEG_D | SEG_E | SEG_C | SEG_G));
          } else if (step == 2) {
            _rawFrames[d] = (currentFrames[d] & SEG_D) | (nextFrames[d] & (SEG_A | SEG_B | SEG_F | SEG_G));
          } else {
            _rawFrames[d] = nextFrames[d];
          }
        }
      }

      unsigned long stepStart = millis();
      while (millis() - stepStart < speedMs) {
        refresh();
      }
    }

    for (uint8_t d = 0; d < 4; d++) {
      currentFrames[d] = nextFrames[d];
      _rawFrames[d] = nextFrames[d];
    }
    unsigned long holdStart = millis();
    while (millis() - holdStart < speedMs * 4) {
      refresh();
    }
  }

  for (uint8_t step = 1; step <= 3; step++) {
    for (uint8_t d = 0; d < 4; d++) {
      if (rollUp) {
        if (step == 1) _rawFrames[d] = (currentFrames[d] & (SEG_A | SEG_B | SEG_F | SEG_G));
        else if (step == 2) _rawFrames[d] = (currentFrames[d] & SEG_A);
        else _rawFrames[d] = 0UL;
      } else {
        if (step == 1) _rawFrames[d] = (currentFrames[d] & (SEG_D | SEG_E | SEG_C | SEG_G));
        else if (step == 2) _rawFrames[d] = (currentFrames[d] & SEG_D);
        else _rawFrames[d] = 0UL;
      }
    }
    unsigned long stepStart = millis();
    while (millis() - stepStart < speedMs) {
      refresh();
    }
  }
}

// -------------------------------------------------------------------
// Wide Character Support (M, W span 2 digit positions)
// -------------------------------------------------------------------
bool FD1642_TVReceiverDisplay::isWideChar(char c) {
  // Use single-digit rotated 90-degree M and W for clean horizontal alignment
  return false;
}

void FD1642_TVReceiverDisplay::getWideCharPatterns(char c, uint32_t &left, uint32_t &right) {
  if (c == 'M' || c == 'm') {
    // M across 2 digits:
    //  _   _
    // |_  _|
    // |    |
    left  = mapStandardToRaw(S_A | S_F | S_E | S_G); // Top + Left verticals + Middle
    right = mapStandardToRaw(S_A | S_B | S_C | S_G); // Top + Right verticals + Middle
  } else { // W or w
    // W across 2 digits (inverted M):
    // |    |
    // |_  _|
    //  _   _
    left  = mapStandardToRaw(S_F | S_E | S_D | S_G); // Left verticals + Bottom + Middle
    right = mapStandardToRaw(S_B | S_C | S_D | S_G); // Right verticals + Bottom + Middle
  }
}

uint8_t FD1642_TVReceiverDisplay::renderStringToFrames(const char* str, uint32_t* frames, uint8_t maxDigits) {
  uint8_t len = strlen(str);
  uint8_t digitPos = 0;

  // Clear all frames first
  for (uint8_t i = 0; i < maxDigits; i++) {
    frames[i] = 0UL;
  }

  for (uint8_t i = 0; i < len && digitPos < maxDigits; i++) {
    char c = str[i];
    char upper = c;
    if (upper >= 'a' && upper <= 'z') upper = upper - 'a' + 'A';

    if (isWideChar(upper) && (digitPos + 1) < maxDigits) {
      // Wide character: takes 2 digit positions
      uint32_t left, right;
      getWideCharPatterns(upper, left, right);
      frames[digitPos]     = left;
      frames[digitPos + 1] = right;
      digitPos += 2;
    } else if (isWideChar(upper) && (digitPos + 1) >= maxDigits) {
      // Not enough room for wide char, show single-digit fallback
      frames[digitPos] = getCharPattern(c);
      digitPos++;
    } else {
      frames[digitPos] = getCharPattern(c);
      digitPos++;
    }
  }
  return digitPos;
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
