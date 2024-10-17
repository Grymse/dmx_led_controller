#include <Arduino.h>
#include <FastLED.h>
#include "IColor.h"
#include <vector>

/**
 * NOT FINISHED
 */

/**
class SectionsColor : IColor {
public:
  std::vector<CRGB> colors;
  u16_t duration;

  String getName() {
    return "Sections";
  }

  SectionsColor( std::vector<CRGB> colors, u16_t duration ) {
    this->colors = colors;
    this->duration = duration;
  }

  CRGB getColor(long tick, u16_t index) {
    return colors[tick / duration % colors.size()];
  }
}; */