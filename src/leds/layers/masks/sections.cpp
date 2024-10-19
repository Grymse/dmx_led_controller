#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class SectionsMask : public ILayer {
  u16_t duration;
  std::vector<u8_t> sections;

public:
  String getName() {
    return "Section Mask";
  }

  SectionsMask(std::vector<u8_t> sections, u16_t duration) {
    this->duration = duration;
    this->sections = sections;
  }

  /**
   * @param color 
   * @param state 
   * @return CRGB 
   */
  CRGB apply(CRGB color, LEDState *state) {
    float sectionLength = (float) state->length / sections.size();
    u16_t sectionIndex = state->tick / duration + (state->index / sectionLength);
    return color.scale8(sections[sectionIndex % sections.size()]);
  }
};