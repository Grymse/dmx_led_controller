#include <Arduino.h>
#include <FastLED.h>
#include <math.h>
#include "masks.h"
#include "../utils.h"


String WaveMask::getName() {
  return "Wave Mask";
}

/**
 * @brief Construct a new Wave Mask object
 *
 * @param wavelength The wavelength of the wave pattern.
 * @param wavegap The gap between waves.
 * @param duration The duration of the wave cycle.
 *
 * @example WaveMask(100, 100, 50)
 */
WaveMask::WaveMask(u16_t wavelength, u16_t wavegap, u16_t duration) {
  this->wavelength = wavelength;
  this->wave_gap = wavegap;
  this->duration = duration;
}


/**
 * @brief Applies a wave to the given color based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB WaveMask::apply(CRGB color, LEDState* state) {
  float len = (float)state->length / duration;
  double x = (double)state->tick * len + state->index;
  if (wavelength <= LayerUtils::mod(x, wavelength + wave_gap)) {
    return CRGB::Black;
  }

  float intensity = (LayerUtils::mod(x, wavelength + wave_gap) / wavelength) * 512;

  if (255 < intensity) {
    intensity = 511 - intensity;
  }

  // turn 0 - 255 into a curve,
  intensity = intensity / 255.0;
  intensity = intensity * intensity * 255;

  return color.scale8(intensity);
}