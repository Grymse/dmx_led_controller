#include <Arduino.h>
#include <FastLED.h>
#include "colors.h"
#include "../utils.h"

String SingleColor::getName() {
  return "Single Color";
}

/**
 * @brief Construct a new Single Color object
 *
 * @param color The color to set the LED to.
 *
 * @example SingleColor(CRGB::Red)
 */
SingleColor::SingleColor(CRGB color) {
  this->localColor = color;
}

String SingleColor::toString() {
  String str = "SingleColor: c: " + LayerUtils::color_to_string(this->localColor);
  
  return str;
}

/**
 * @brief Overwrites color to the color given by the constructor
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB SingleColor::apply(CRGB color, LEDState* state) {
  return this->localColor;
}

void SingleColor::setColor(CRGB color) {
  this->localColor = color;
}

protocol_Layer SingleColor::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SingleColor,
    .color = (uint32_t)0 | this->localColor.r << 16 | this->localColor.g << 8 | this->localColor.b
  };
}