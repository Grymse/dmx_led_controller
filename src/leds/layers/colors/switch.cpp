#include <Arduino.h>
#include <FastLED.h>
#include "colors.h"

String SwitchColor::getName() {
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
SwitchColor::SwitchColor(std::vector<CRGB> colors, u16_t duration) {
  this->colors = colors;
  this->duration = duration;
}

/**
 * @brief Overwrites color to the colors given by the constructor, and switches color every duration ticks.
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SwitchColor::apply(CRGB color, LEDState* state) {
  u16_t duration = duration / colors.size();
  return colors[state->tick / duration % colors.size()];
}

protocol_Layer SwitchColor::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SwitchColor,
    .duration = duration,
    .colors = {
      .arg = &colors
    }
  };
}