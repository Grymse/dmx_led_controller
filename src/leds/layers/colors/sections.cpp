#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class SectionsColor : public ILayer {
  u16_t duration;
  std::vector<CRGB> sections;

  public:
  String getName() {
    return "Section Color";
  }

  /**
   * @brief Construct a new Sections Mask object
   *
   * @param sections define the sections and their color
   * @param duration The duration in ticks, it takes to go through all colors
   *
   * @example SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue}, 50)
   */
  SectionsColor(std::vector<CRGB> sections, u16_t duration) {
    this->duration = duration / sections.size();
    this->sections = sections;
  }

  /**
   * @brief Overwrites color to the sectionized colors based on the current state (tick and index of led)
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    float sectionLength = (float)state->length / sections.size();
    u16_t sectionIndex = state->tick / duration + (state->index / sectionLength);
    return sections[sectionIndex % sections.size()];
  }
};