#include <Arduino.h>
#include <FastLED.h>
#include "../utils/LEDstate.h"
#ifndef LAYER_H
#define LAYER_H

class ILayer {
  public:
  virtual ~ILayer() {}

  /**
   * @brief Get the Name object
   *
   * @return String
   */
  virtual String getName() = 0;

  /**
   * @brief Apply the layer to the given color.
   *
   * @param color current color of the LED
   * @param state current state of the LED
   * @return modified color
   */
  virtual CRGB apply(CRGB color, LEDState* state) = 0;
};

#endif

