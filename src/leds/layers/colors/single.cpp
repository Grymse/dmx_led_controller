#include <Arduino.h>
#include <FastLED.h>
#include "colors.h"

String SingleColor::getName() {
  return "Single Color";
}

/**
 * @brief Construct a new Single Color object
 *
 * @param color The color to set the LED to.
 *
 * @example SingleColor(CRGB::Red)
 */
SingleColor::SingleColor(CRGB color) {
  this->localColor = color;
}

/**
 * @brief Overwrites color to the color given by the constructor
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SingleColor::apply(CRGB color, LEDState* state) {
  return localColor;
}