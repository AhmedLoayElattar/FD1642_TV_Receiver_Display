/*
  FD1642_TVReceiverDisplay.h - Arduino Library for Salvaged TV Receiver / 
  Set-Top Box 4-Digit 7-Segment LED Displays (FD1642 / CT1642 driver ICs).

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

  /**
   * @brief Set all 4 digits simultaneously (0..9)
   * 
   * @param d1 Leftmost Digit
   * @param d2 Middle-Left Digit
   * @param d3 Middle-Right Digit
   * @param d4 Rightmost Digit
   */
  void showDigits(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);

  /**
   * @brief Set a single digit by index (0=Leftmost/Digit 1 ... 3=Rightmost/Digit 4)
   * 
   * @param index Digit index (0 to 3)
   * @param value Number to display (0 to 9)
   */
  void setDigit(uint8_t index, uint8_t value);

  // Direct single-digit setters
  void setDigit1(uint8_t value); // Leftmost Digit
  void setDigit2(uint8_t value); // Middle-Left Digit
  void setDigit3(uint8_t value); // Middle-Right Digit
  void setDigit4(uint8_t value); // Rightmost Digit

  /**
   * @brief Set a 4-digit integer (0 to 9999)
   * 
   * @param value Integer value
   */
  void showNumber(uint16_t value);

  /**
   * @brief Set digital clock format HH:MM
   * 
   * @param hours Hours (0 to 99)
   * @param minutes Minutes (0 to 59)
   */
  void showClock(uint8_t hours, uint8_t minutes);

  /**
   * @brief Clear/turn off all segments
   */
  void clear();

  /**
   * @brief Refresh the display. Call inside loop() or timer interrupt.
   */
  void refresh();

private:
  uint8_t _clkPin;
  uint8_t _dataPin;
  uint8_t _digits[4];

  void startCondition();
  void stopCondition();
  void send18BitData(uint32_t data);
};

// Alias for backwards compatibility
typedef FD1642_TVReceiverDisplay TVReceiverDisplay;

#endif
