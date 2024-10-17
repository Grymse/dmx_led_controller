#include <Arduino.h>
#include <FastLED.h>
#include "IColor.h"
#include <vector>

class SwitchColor : IColor {
public:
  std::vector<CRGB> colors;
  u16_t duration;

  String getName() {
    return "Switch";
  }

  SwitchColor( std::vector<CRGB> colors, u16_t duration ) {
    this->colors = colors;
    this->duration = duration;
  }

  CRGB getColor(long tick, u16_t index) {
    return colors[tick / duration % colors.size()];
  }
};