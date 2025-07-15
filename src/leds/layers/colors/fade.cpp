#include <Arduino.h>
#include <FastLED.h>
#include <colorutils.h>
#include <vector>
#include "colors.h"
#include "../utils.h"

String FadeColor::getName() {
  return "Fade Color";
}

/**
 * @brief Construct a new Fade Color object
 *
 * @param colors The colors to fade between.
 * @param duration The duration of the fade effect in ticks.
 *
 * @example FadeColor({CRGB::Red, CRGB::Green, CRGB::Blue}, 20)
 */
FadeColor::FadeColor(std::vector<CRGB> colors, u16_t duration) {
  this->colors = colors;
  this->duration = duration;
}

CRGB fadeBetween(CRGB from, CRGB to, float percentage) {
  return CRGB(
    (1 - percentage) * from.r + percentage * to.r,
    (1 - percentage) * from.g + percentage * to.g,
    (1 - percentage) * from.b + percentage * to.b
  );
}

String FadeColor::toString() {
  String str = "FadeColor: d: " + String(this->duration) + ", c: ";
  str += LayerUtils::colors_to_string(this->colors);
  
  return str;
}

/**
 * @brief Overwrites color to fade from one color to another based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB FadeColor::apply(CRGB color, LEDState* state) {
  // Calculate the duration for each individual color segment within the total fade duration.
  // This ensures that the total fade cycle (e.g., Red -> Green -> Blue -> Red)
  // completes within 'this->duration' ticks.
  u16_t segmentDuration = this->duration / this->colors.size();

  // Calculate the percentage of the fade within the current segment.
  // This should be based on the global animation tick, not the LED's index.
  // (state->tick % segmentDuration) gives the current tick within the current segment.
  // Dividing by (float)segmentDuration normalizes it to a 0.0 to <1.0 value.
  float percentage = (float)(state->tick % segmentDuration) / segmentDuration;

  // Determine the 'from' color index.
  // This is based on which segment of the overall fade cycle the current tick falls into.
  u8_t fromIndex = (state->tick / segmentDuration) % this->colors.size();

  // Determine the 'to' color index (the next color in the sequence).
  u8_t toIndex = (fromIndex + 1) % this->colors.size();

  // Perform the linear interpolation between the 'from' and 'to' colors
  // using the calculated percentage.
  return fadeBetween(this->colors[fromIndex], this->colors[toIndex], percentage);
}

protocol_Layer FadeColor::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_FadeColor,
    .duration = this->duration,
    .colors = {
      .arg = &this->colors
    }
  };
}