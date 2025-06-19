#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "colors.h"
#include "../utils.h"

String SectionsWaveColor::getName() {
  return "Sections Wave Color";
}

/**
 * @brief Construct a new Sections Mask object
 *
 * @param sections define the sections and their color
 * @param duration The duration of the wave cycle.
 *
 * @example SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue}, 50)
 */
SectionsWaveColor::SectionsWaveColor(std::vector<CRGB> colors, u16_t duration) {
  this->duration = duration;
  this->colors = colors;
}

String SectionsWaveColor::toString() {
  String str = "SectionsWaveColor: d: " + String(duration) + ", c: ";
  str += LayerUtils::colors_to_string(colors);
  
  return str;
}

/**
 * @brief Overwrites color to the sectionized colors in wave-form based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SectionsWaveColor::apply(CRGB color, LEDState* state) {
  float sectionLength = (float)state->length / colors.size();
  u16_t tick = (state->virtual_index % duration) * state->length / duration;
  u16_t t = (tick + state->virtual_index) / sectionLength;

  return colors[t % colors.size()];
}


protocol_Layer SectionsWaveColor::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SectionsWaveColor,
    .duration = duration,
    .colors = {
      .arg = &colors
    }
  };
}