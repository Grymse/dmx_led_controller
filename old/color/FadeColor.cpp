#include <Arduino.h>
#include <FastLED.h>
#include "IColor.h"
#include "ColorUtility.cpp"
#include <vector>

class FadeColor : IColor {
public:
  std::vector<CRGB> colors;
  u16_t speed;

  String getName() {
    return "Fade";
  }

  FadeColor( std::vector<CRGB> colors, u16_t speed ) {
    this->colors = colors;
    this->speed = 1000 / speed;
  }

  CRGB getColor(long tick, u16_t index) {
    float percentage = (tick % speed) / (float)speed;
    u8_t fromIndex = (tick / speed) % colors.size();
    u8_t toIndex = (fromIndex + 1) % colors.size();

    return ColorUtility().FadeFromTo(colors[fromIndex], colors[toIndex], percentage);
  }
};