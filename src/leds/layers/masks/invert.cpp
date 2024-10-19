#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"

class InvertMask : public ILayer {
public:
  String getName() {
    return "Invert";
  } 

  CRGB apply(CRGB color, LEDState *state) {
    return CRGB::White - color;
  }
};