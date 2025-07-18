#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "masks.h"
#include "../utils.h"

String SectionsWaveMask::getName() {
  return "Sections Wave Mask";
}

/**
 * @brief Construct a new Sections Mask object
 *
 * @param sections define the sections and their amplitude
 * @param duration The duration of the wave cycle.
 *
 * @example SectionsWaveMask({255, 0, 127, 0}, 10)
 */
SectionsWaveMask::SectionsWaveMask(std::vector<u8_t> sections, u16_t duration) {
  this->duration = duration;
  this->sections = sections;
}

String SectionsWaveMask::toString() {
  String str = "SectionsWaveMask: d: " + String(this->duration) + ", c: ";
  str += LayerUtils::bytes_to_string(this->sections);
  
  return str;
}


/**
 * @brief Applies a wave defined by its sections to the given color based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SectionsWaveMask::apply(CRGB color, LEDState* state) {
  float sectionLength = (float)state->length / this->sections.size();
  u16_t tick = (state->tick % this->duration) * state->length / this->duration;
  u16_t t = (tick + state->virtual_index) / sectionLength;

  return color.scale8(this->sections[t % this->sections.size()]);
}

protocol_Layer SectionsWaveMask::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SectionsWaveMask,
    .duration = this->duration,
    .sections = {
      .arg = &this->sections
    }
  };
}