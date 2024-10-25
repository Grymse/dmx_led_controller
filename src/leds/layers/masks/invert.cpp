#include <Arduino.h>
#include <FastLED.h>
#include "masks.h"

String InvertMask::getName() {
  return "Invert Mask";
}

/**
 * @brief Inverts the given color based on the current state.
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB InvertMask::apply(CRGB color, LEDState* state) {
  return CRGB::White - color;
}