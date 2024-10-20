#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class SwitchColor : public ILayer {
  std::vector<CRGB> colors;
  u16_t duration;

  public:
  String getName() {
    return "Switch Color";
  }

  /**
   * @brief Construct a new Switch Color object
   *
   * @param colors The colors to switch between.
   * @param duration The duration of the switch effect in ticks.
   *
   * @example SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue}, 20)
   */
  SwitchColor(std::vector<CRGB> colors, u16_t duration) {
    this->colors = colors;
    this->duration = duration / colors.size();
  }

  /**
   * @brief Overwrites color to the colors given by the constructor, and switches color every duration ticks.
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    return colors[state->tick / duration % colors.size()];
  }
};