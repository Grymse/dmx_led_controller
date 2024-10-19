#include <Arduino.h>
#include <FastLED.h>
#include "../utils/LEDstate.h"
#ifndef LAYER_H
#define LAYER_H

class ILayer {
public:
  virtual ~ILayer() {}

  virtual String getName() = 0;

  virtual CRGB apply(CRGB color, LEDState *state) = 0;
};

#endif

