#include <Arduino.h>
#include <FastLED.h>
#include "utility.cpp"
#include <vector>
#include "../layer.h"

class FadeColor : public ILayer {
  std::vector<CRGB> colors;
  u16_t duration;

  public:
  String getName() {
    return "Fade Color";
  }

  /**
   * @brief Construct a new Fade Color object
   *
   * @param colors The colors to fade between.
   * @param duration The duration of the fade effect in ticks.
   *
   * @example FadeColor({CRGB::Red, CRGB::Green, CRGB::Blue}, 20)
   */
  FadeColor(std::vector<CRGB> colors, u16_t duration) {
    this->colors = colors;
    this->duration = duration / colors.size();
  }

  /**
   * @brief Overwrites color to fade from one color to another based on the current state (tick and index of led)
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    float percentage = (state->tick % duration) / (float)duration;
    u8_t fromIndex = (state->tick / duration) % colors.size();
    u8_t toIndex = (fromIndex + 1) % colors.size();

    return ColorUtility().FadeFromTo(colors[fromIndex], colors[toIndex], percentage);
  }
};
