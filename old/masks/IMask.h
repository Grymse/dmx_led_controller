#include <Arduino.h>
#include <FastLED.h>

enum Direction {
  FORWARD,
  BACKWARD
};

class IMask {
public:
  virtual ~IMask() {}

  virtual String getName() = 0;

  virtual CRGB performAction(CRGB color, long tick, u16_t index) = 0;
};