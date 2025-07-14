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
  this->wavegap = wavegap;
  this->duration = duration;
}

String WaveMask::toString() {
  return "WaveMask: d: " + String(this->duration) + ", l: " + String(this->wavelength) + ", g: " + String(this->wavegap);
}

/**
 * @brief Applies a wave to the given color based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB WaveMask::apply(CRGB color, LEDState* state) {
  float len = (float)state->length / this->duration;
  double x = (double)state->tick * len + state->virtual_index;
  if (this->wavelength <= LayerUtils::mod(x, this->wavelength + this->wavegap)) {
    return CRGB::Black;
  }

  float intensity = (LayerUtils::mod(x, this->wavelength + this->wavegap) / this->wavelength) * 512;

  if (255 < intensity) {
    intensity = 511 - intensity;
  }

  // turn 0 - 255 into a curve,
  intensity = intensity / 255.0;
  intensity = intensity * intensity * 255;

  return color.scale8(intensity);
}

protocol_Layer WaveMask::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_WaveMask,
    .duration = this->duration,
    .length = this->wavelength,
    .gap = this->wavegap,
  };
}