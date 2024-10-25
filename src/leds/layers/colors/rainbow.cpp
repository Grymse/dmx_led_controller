#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "colors.h"

String RainbowColor::getName() {
  return "Rainbow Color";
}

/**
 * @brief Construct a new Rainbow Color object
 *
 * @param duration The duration of the rainbow cycle.
 * @param length The length of the rainbow cycle.
 *
 * @example RainbowColor(100, 100)
 */
RainbowColor::RainbowColor(u16_t duration, u16_t length) {
  this->duration = duration;
  this->length = length;
}

/**
 * @brief Overwrites color to a rainbow based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB RainbowColor::apply(CRGB color, LEDState* state) {
  uint8_t hueFromIndex = (255.0 / length) * state->index;
  uint8_t hueFromTick = (255.0 / duration) * state->tick;

  return CHSV(hueFromIndex + hueFromTick, 255, 255);
}