#include <Arduino.h>
#include <FastLED.h>
#include <math.h>
#include "masks.h"
#include "../utils.h"

String SawtoothMask::getName() {
  return "Sawtooth Mask";
}

/**
 * @brief Construct a new Sawtooth Mask object
 *
 * @param wavelength The wavelength of the wave pattern.
 * @param wavegap The gap between waves.
 * @param duration The duration of the wave cycle.
 *
 * @example SawtoothMask(100, 200, 50)
 */
SawtoothMask::SawtoothMask(u16_t wavelength, u16_t wavegap, u16_t duration) {
  this->wavelength = wavelength;
  this->wavegap = wavegap;
  this->duration = duration;
}

String SawtoothMask::toString() {
  return "SawtoothMask: d: " + String(duration) + ", l: " + String(wavelength) + ", g: " + String(wavegap);
}


/**
 * @brief Applies a sawtooth wave to the given color based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SawtoothMask::apply(CRGB color, LEDState* state) {
  float totalLength = wavegap + wavelength;
  float len = (float)state->length / duration;
  double x = ((double)state->tick * len + state->index) * state->direction;

  if (wavelength <= LayerUtils::mod(x, totalLength)) {
    return CRGB::Black;
  }

  float intensity = 255 - (LayerUtils::mod(x, totalLength) / wavelength) * 255;

  // turn 0 - 255 into a curve,
  intensity = intensity / 255.0;
  intensity = intensity * intensity * 255;

  return color.scale8(intensity);
}

protocol_Layer SawtoothMask::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SawtoothMask,
    .duration = duration,
    .length = wavelength,
    .gap = wavegap
  };
}