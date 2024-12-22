#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "masks.h"
#include "../utils.h"


/**
 * @brief Constructs a BlinkMask object with the specified pattern and duration.
 * @param pattern A vector of intensity values representing the blink pattern.
 * @param duration The duration of the blink effect in ticks.
 *
 * @example BlinkMask({255, 0, 0, 0}, 20)
 */
BlinkMask::BlinkMask(std::vector<u8_t> pattern, u16_t duration)
  : duration(duration), pattern(pattern) {}

String BlinkMask::getName() {
  return "Blink Mask";
}

String BlinkMask::toString() {
  String str = "BlinkMask: d: " + String(duration) + ", c: ";
  str += LayerUtils::bytes_to_string(pattern);
  
  return str;
}

/**
 * @brief Applies the blink pattern to the given color based on the current state.
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB BlinkMask::apply(CRGB color, LEDState* state) {
  u16_t patternIndex = (state->tick % duration) / ((float)duration / pattern.size());
  return color.scale8(pattern[patternIndex % pattern.size()]);
}

protocol_Layer BlinkMask::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_BlinkMask,
    .duration = duration,
    .sections = {
      .arg = &pattern
    }
  };
}