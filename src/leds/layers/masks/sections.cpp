#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "masks.h"
#include "../utils.h"

String SectionsMask::getName() {
  return "Sections Mask";
}

/**
 * @brief Construct a new Sections Mask object
 *
 * @param sections define the sections and their amplitude
 * @param duration The duration of the wave cycle.
 *
 * @example SectionsMask({255, 0, 255, 0}, 10)
 */
SectionsMask::SectionsMask(std::vector<u8_t> sections, u16_t duration) {
  this->duration = duration;
  this->sections = sections;
}

String SectionsMask::toString() {
  String str = "SectionsMask: d: " + String(duration) + ", c: ";
  str += LayerUtils::bytes_to_string(sections);
  
  return str;
}


/**
 * @brief Static sections switch through amplitude based on the current state (tick and index of led) and sections
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SectionsMask::apply(CRGB color, LEDState* state) {
  u16_t duration = duration / sections.size();
  float sectionLength = (float)state->length / sections.size();
  u16_t sectionIndex = state->tick / duration + (state->index / sectionLength);
  return color.scale8(sections[sectionIndex % sections.size()]);
}

protocol_Layer SectionsMask::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SectionsMask,
    .duration = duration,
    .sections = {
      .arg = &sections
    }
  };
}