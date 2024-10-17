#include <Arduino.h>
#include <FastLED.h>
#include "IColor.h"

class SingleColor : IColor {
public:
  CRGB color;

  String getName() {
    return "Single";
  }

  SingleColor(CRGB color) {
    this->color = color;
  }

  CRGB getColor(long tick, u16_t index) {
    return color;
  }
};