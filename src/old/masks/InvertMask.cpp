#include <Arduino.h>
#include <FastLED.h>
#include "IMask.h"

class InvertMask : IMask {
public:
  String getName() {
    return "Invert";
  } 

  CRGB performAction(CRGB color, long tick, u16_t index) {
    return CRGB::White - color;
  }
};