#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"

class SingleColor : public ILayer {
  CRGB localColor;

public:
  String getName() {
    return "Single Color";
  }

  SingleColor(CRGB color) {
    this->localColor = color;
  }

  CRGB apply(CRGB color, LEDState *state) {
    return localColor;
  }
};