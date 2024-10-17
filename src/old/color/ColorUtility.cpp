#include <Arduino.h>
#include <FastLED.h>

class ColorUtility {
  public:
    static CRGB FadeFromTo(CRGB from, CRGB to, float percent) {
      return from * (1.f - percent) + to * percent;
    }
};