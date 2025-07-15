#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "colors.h"
#include "../utils.h"

String SectionsColor::getName() {
  return "Sections Color";
}

/**
 * @brief Construct a new Sections Mask object
 *
 * @param sections define the sections and their color
 * @param duration The duration in ticks, it takes to go through all colors
 *
 * @example SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue}, 50)
 */
SectionsColor::SectionsColor(std::vector<CRGB> colors, u16_t duration) {
  this->duration = duration;
  this->colors = colors;
}

String SectionsColor::toString() {
  String str = "SectionsColor: d: " + String(this->duration) + ", c: ";
  str += LayerUtils::colors_to_string(this->colors);
  
  return str;
}

/**
 * @brief Overwrites color to the sectionized colors based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SectionsColor::apply(CRGB color, LEDState* state) {
  u16_t segmentDuration = this->duration / this->colors.size();
  float sectionLength = (float)state->length / this->colors.size();
  u16_t sectionIndex = state->tick / segmentDuration + (state->virtual_index / sectionLength);
  return this->colors[sectionIndex % this->colors.size()];
}

protocol_Layer SectionsColor::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SectionsColor,
    .duration = this->duration,
    .colors = {
      .arg = &this->colors
    }
  };
}