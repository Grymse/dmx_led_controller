#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class BlinkMask : public ILayer {
  u16_t duration;
  std::vector<u8_t> pattern;

public:
  String getName() {
    return "Blink Mask";
  }

  BlinkMask(std::vector<u8_t> pattern, u16_t duration) {
    this->duration = duration;
    this->pattern = pattern;
  }

  /**
   * @param color 
   * @param state 
   * @return CRGB 
   */
  CRGB apply(CRGB color, LEDState *state) {
    u16_t patternIndex = (state->tick % duration) / ((float)duration / pattern.size());
    return color.scale8(pattern[patternIndex % pattern.size()]);
  }
};