#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"

class InvertMask : public ILayer {
  public:
  String getName() {
    return "Invert Mask";
  }

  /**
   * @brief Inverts the given color based on the current state.
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    return CRGB::White - color;
  }
};