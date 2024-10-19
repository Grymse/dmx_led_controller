#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class SectionsMask : public ILayer {
  u16_t duration;
  std::vector<bool> sections;

public:
  String getName() {
    return "Section Mask";
  }

  SectionsMask(std::vector<bool> sections, u16_t duration) {
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
    return sections[sectionIndex % sections.size()] ? color : color.Black;
  }
};