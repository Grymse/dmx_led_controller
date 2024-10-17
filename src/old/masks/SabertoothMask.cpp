#include <Arduino.h>
#include <FastLED.h>
#include "IMask.h"

class SabertoothMask : IMask {
public:
  Direction direction;
  String getName() {
    return "Sabertooth";
  }

  SabertoothMask(Direction direction) {
    this->direction = direction;
  }

  CRGB performAction(CRGB color, long tick, u16_t index) {
    // TODO: Not Implemented
    return color;
  }
};