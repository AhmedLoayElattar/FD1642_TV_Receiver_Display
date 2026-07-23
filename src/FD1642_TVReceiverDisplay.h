/*
  FD1642_TVReceiverDisplay.h - Advanced C++ Arduino Library for Salvaged 
  TV Receiver & Set-Top Box LED Displays (FD1642 / CT1642 driver ICs).

  Includes Alphanumeric Character Rendering, Text Scrolling, and Pre-built Animations.
  Copyright (c) 2026. Released under the MIT License.
*/

#ifndef FD1642_TV_RECEIVER_DISPLAY_H
#define FD1642_TV_RECEIVER_DISPLAY_H

#include <Arduino.h>

class FD1642_TVReceiverDisplay {
public:
  /**
   * @brief Construct a new FD1642_TVReceiverDisplay object
   * 
   * @param clkPin Arduino digital pin connected to display CLK
   * @param dataPin Arduino digital pin connected to display DATA (DIN)
   */
  FD1642_TVReceiverDisplay(uint8_t clkPin, uint8_t dataPin);

  /**
   * @brief Initialize pin modes (CLK and DATA as OUTPUT)
   */
  void begin();

  // -------------------------------------------------------------------
  // DIGIT & NUMBER SETTERS
  // -------------------------------------------------------------------
  
  /** Set all 4 digits at once (0..9) */
  void showDigits(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);

  /** Set single digit by index (0=Leftmost, 3=Rightmost) */
  void setDigit(uint8_t index, uint8_t value);

  // Direct single-digit setters
  void setDigit1(uint8_t value); // Leftmost Digit
  void setDigit2(uint8_t value); // Middle-Left Digit
  void setDigit3(uint8_t value); // Middle-Right Digit
  void setDigit4(uint8_t value); // Rightmost Digit

  /** Set 4-digit integer (0 to 9999) */
  void showNumber(uint16_t value);

  /** Set digital clock format HH:MM */
  void showClock(uint8_t hours, uint8_t minutes);

  // -------------------------------------------------------------------
  // TEXT & ALPHANUMERIC CHARACTERS
  // -------------------------------------------------------------------

  /**
   * @brief Display up to 4 characters on the display (e.g. "PLAY", "LOAD", "COOL", "OFF ")
   * 
   * @param str C-string with up to 4 characters
   */
  void showString(const char* str);

  /**
   * @brief Set a single ASCII character at specified digit position
   * 
   * @param index Digit index (0 to 3)
   * @param c ASCII character ('A'..'Z', '0'..'9', '-', '_', ' ')
   */
  void setChar(uint8_t index, char c);

  /**
   * @brief Scroll a string across the 4 digits (Blocking animation)
   * 
   * @param text Long text string to scroll
   * @param speedMs Delay in milliseconds per scroll step (default: 250ms)
   */
  void scrollText(const char* text, uint16_t speedMs = 250);

  // -------------------------------------------------------------------
  // ANIMATIONS & LOADING SCREENS
  // -------------------------------------------------------------------

  /**
   * @brief Rotating Ring Loading Animation (Clockwise segment spinner)
   * 
   * @param durationMs Total duration of loading animation in milliseconds
   * @param speedMs Delay per rotation step in milliseconds (default: 80ms)
   */
  void animLoadingSpinner(uint16_t durationMs = 3000, uint16_t speedMs = 80);

  /**
   * @brief Chasing Outer Wave Animation (Circles the perimeter of digits)
   * 
   * @param cycles Number of full wave cycles
   * @param speedMs Speed per step in milliseconds (default: 70ms)
   */
  void animWave(uint16_t cycles = 3, uint16_t speedMs = 70);

  /**
   * @brief Bouncing Dash / Dot Animation (Moves back and forth left-to-right)
   * 
   * @param bounces Number of bounces
   * @param speedMs Speed per move in milliseconds (default: 100ms)
   */
  void animBounce(uint16_t bounces = 4, uint16_t speedMs = 100);

  /**
   * @brief Slot Machine Reel Roll Animation (Spins digits until landing on target)
   * 
   * @param targetNumber Number to land on (0 to 9999)
   * @param durationMs Duration of roll in milliseconds (default: 2000ms)
   */
  void animSlotMachine(uint16_t targetNumber, uint16_t durationMs = 2000);

  /**
   * @brief Fill & Wipe Animation (Fills left-to-right, clears left-to-right)
   * 
   * @param speedMs Delay per column in milliseconds (default: 150ms)
   */
  void animFillWipe(uint16_t speedMs = 150);

  // -------------------------------------------------------------------
  // CUSTOM CHARACTERS & DIRECT SEGMENT CONTROL
  // -------------------------------------------------------------------

  /**
   * @brief Directly set the segments of a digit using standard 7-segment encoding.
   * Standard encoding: 0b0GFEDCBA (Bit 0=A, Bit 1=B, ..., Bit 6=G)
   * 
   * @param index Digit index (0 to 3)
   * @param segmentMask Standard 7-segment bitmask (e.g. 0b01110111 for 'A')
   */
  void setSegments(uint8_t index, uint8_t segmentMask);

  /**
   * @brief Override or define a new standard 7-segment pattern for an ASCII character.
   * This affects showString, setChar, and scrollText.
   * 
   * @param c The ASCII character to override (e.g. 'M', 'W')
   * @param segmentMask Standard 7-segment bitmask (0b0GFEDCBA)
   */
  void setCustomChar(char c, uint8_t segmentMask);

  // -------------------------------------------------------------------
  // SYSTEM UTILITIES
  // -------------------------------------------------------------------

  /** Clear/turn off all segments */
  void clear();

  /** Refresh the display multiplexer. Must be called repeatedly in loop(). */
  void refresh();

private:
  uint8_t _clkPin;
  uint8_t _dataPin;
  uint32_t _rawFrames[4]; // Raw 18-bit segment frames for 4 digits
  uint8_t _customFont[128]; // User-overridden characters

  void startCondition();
  void stopCondition();
  void send18BitData(uint32_t data);
  uint32_t mapStandardToRaw(uint8_t standardMask);
  uint32_t getCharPattern(char c);
  bool isWideChar(char c);
  void getWideCharPatterns(char c, uint32_t &left, uint32_t &right);
  uint8_t renderStringToFrames(const char* str, uint32_t* frames, uint8_t maxDigits);
};

// Alias for backwards compatibility
typedef FD1642_TVReceiverDisplay TVReceiverDisplay;

#endif
