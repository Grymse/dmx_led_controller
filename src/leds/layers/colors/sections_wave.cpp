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
  String str = "SectionsWaveColor: d: " + String(this->duration) + ", c: ";
  str += LayerUtils::colors_to_string(this->colors);
  
  return str;
}

/**
 * @brief Overwrites color to the sectionized colors in wave-form based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SectionsWaveColor::apply(CRGB color, LEDState* state) {
  // 1. Calculate the length of each color section on the strip.
  // This determines how many physical LEDs each color in 'this->colors' covers.
  float sectionLength = (float)state->length / this->colors.size();

  // 2. Calculate a time-based offset for the wave.
  // This offset determines how much the pattern "shifts" along the strip over time.
  // The modulo 'this->duration' ensures the wave pattern repeats cleanly over time.
  // We scale 'state->tick' to match the 'sectionLength' for a smooth shift.
  // The 256.0f is a common scaling factor to make the wave move at a reasonable speed
  // relative to the duration and strip length. You might need to tune this.
  // A simple way to think about it: how many 'sectionLength' units does the wave move per 'duration' cycle?
  // Let's make it simpler: the offset should be proportional to the tick and wrap around.
  // The 'offsetInSections' determines how many 'sections' the pattern has shifted.
  float offsetInSections = (float)state->tick / this->duration * this->colors.size();

  // 3. Calculate the effective index for the current LED into the 'colors' array.
  // This combines the LED's physical position with the time-based offset.
  // We cast to int to get an integer index for array access.
  // The modulo 'this->colors.size()' handles wrapping the pattern around the strip.
  int combinedIndex = (int)(state->index / sectionLength + offsetInSections);

  // Return the color from the 'colors' vector based on the calculated index.
  // The final modulo ensures the index is always valid for the 'colors' vector.
  return this->colors[combinedIndex % this->colors.size()];
}


protocol_Layer SectionsWaveColor::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SectionsWaveColor,
    .duration = this->duration,
    .colors = {
      .arg = &this->colors
    }
  };
}