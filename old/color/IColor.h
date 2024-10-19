#include <Arduino.h>
#include <FastLED.h>

class IColor {
public:
  virtual ~IColor() {}

  virtual String getName() = 0;

  virtual CRGB getColor(long tick, u16_t index) = 0;
};