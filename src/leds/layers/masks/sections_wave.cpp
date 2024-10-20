#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class SectionsWaveMask : public ILayer {
  u16_t duration;
  std::vector<u8_t> sections;

  public:
  String getName() {
    return "Section Mask";
  }

  /**
   * @brief Construct a new Sections Mask object
   *
   * @param sections define the sections and their amplitude
   * @param duration The duration of the wave cycle.
   *
   * @example SectionsWaveMask({255, 0, 127, 0}, 10)
   */
  SectionsWaveMask(std::vector<u8_t> sections, u16_t duration) {
    this->duration = duration;
    this->sections = sections;
  }

  /**
   * @brief Applies a wave defined by its sections to the given color based on the current state (tick and index of led)
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    float sectionLength = (float)state->length / sections.size();
    u16_t tick = (state->tick % duration) * state->length / duration;
    u16_t t = (tick + state->index) / sectionLength;

    return color.scale8(sections[t % sections.size()]);
  }
};