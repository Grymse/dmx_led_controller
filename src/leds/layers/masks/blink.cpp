#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

/**
 * @class BlinkMask
 * @brief A class to create a blinking effect on an LED strip.
 *
 * The BlinkMask class inherits from the ILayer interface and provides functionality to apply
 * a blinking pattern to an LED strip. The pattern and duration of the blink effect can be specified.
 */
class BlinkMask : public ILayer {
  u16_t duration;
  std::vector<u8_t> pattern;

  public:
  String getName() {
    return "Blink Mask";
  }

  /**
   * @brief Constructs a BlinkMask object with the specified pattern and duration.
   * @param pattern A vector of intensity values representing the blink pattern.
   * @param duration The duration of the blink effect in ticks.
   *
   * @example BlinkMask({255, 0, 0, 0}, 20)
   */
  BlinkMask(std::vector<u8_t> pattern, u16_t duration) {
    this->duration = duration;
    this->pattern = pattern;
  }

  /**
   * @brief Applies the blink pattern to the given color based on the current state.
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    u16_t patternIndex = (state->tick % duration) / ((float)duration / pattern.size());
    return color.scale8(pattern[patternIndex % pattern.size()]);
  }
};