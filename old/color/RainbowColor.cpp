#include <Arduino.h>
#include <FastLED.h>
#include "IColor.h"
#include <vector>

class RainbowColor : IColor {
public:
  float length;
  float speed;

  String getName() {
    return "Rainbow";
  }

  RainbowColor( u16_t length, int16_t speed ) {
    this->length = ((float)length) / 10.f;
    this->speed = 10.f / speed;
  }

  CRGB getColor(long tick, u16_t index) {
    long newTick = tick * speed;

    return CHSV(index * length - (newTick % 255), 255, 255);
  }
};