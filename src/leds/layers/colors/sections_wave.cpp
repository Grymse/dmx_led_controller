#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "colors.h"

String SectionsWaveColor::getName() {
  return "Section Color";
}

/**
 * @brief Construct a new Sections Mask object
 *
 * @param sections define the sections and their color
 * @param duration The duration of the wave cycle.
 *
 * @example SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue}, 50)
 */
SectionsWaveColor::SectionsWaveColor(std::vector<CRGB> sections, u16_t duration) {
  this->duration = duration;
  this->sections = sections;
}

/**
 * @brief Overwrites color to the sectionized colors in wave-form based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SectionsWaveColor::apply(CRGB color, LEDState* state) {
  float sectionLength = (float)state->length / sections.size();
  u16_t tick = (state->tick % duration) * state->length / duration;
  u16_t t = (tick + state->index) / sectionLength;

  return sections[t % sections.size()];
}