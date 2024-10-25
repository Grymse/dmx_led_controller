#include <Arduino.h>
#include <FastLED.h>
#include <math.h>
#include "masks.h"

String PulseMask::getName() {
  return "Pulse Mask";
}

/**
 * @brief Construct a new Pulse Mask object
 *
 * @param pulse_gap The gap between pulses in ticks.
 * @param duration The duration of each pulse in ticks.
 *
 * @example PulseMask(50, 50)
 */
PulseMask::PulseMask(u16_t pulse_gap, u16_t duration) {
  this->pulse_gap = pulse_gap;
  this->duration = duration;
}

/**
 * @brief Applies a pulse wave to the given color based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB PulseMask::apply(CRGB color, LEDState* state) {
  float intensity = (float)(state->tick % (duration + pulse_gap)) / duration;
  if (1.f < intensity) {
    return CRGB::Black;
  }

  if (0.5f < intensity) {
    intensity = 1 - intensity;
  }

  intensity = 4 * intensity * intensity * 255;

  return color.scale8(intensity);
}