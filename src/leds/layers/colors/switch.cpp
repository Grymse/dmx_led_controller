#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class SwitchColor : public ILayer {
  std::vector<CRGB> colors;
  u16_t duration;

public:
  String getName() {
    return "Switch Color";
  }

  SwitchColor( std::vector<CRGB> colors, u16_t duration ) {
    this->colors = colors;
    this->duration = duration;
  }

  CRGB apply(CRGB color, LEDState *state) {
    return colors[state->tick / duration % colors.size()];
  }
};