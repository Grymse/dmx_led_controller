#include <Arduino.h>
#include <FastLED.h>
#include "IMask.h"

class WaveMask : IMask {
public:
  Direction direction;
  String getName() {
    return "Wave";
  }

  WaveMask(Direction direction) {
    this->direction = direction;
  }

  CRGB performAction(CRGB color, long tick, u16_t index) {
    uint8_t reducedBPM = 120 / 30; // divided by 30, to make effect more stable!
    float speed_multiplier = (((float) reducedBPM) / 2);
    uint16_t wavelength = 512 / 10;
    uint16_t half_wavelength = wavelength / 2;

    uint16_t count = ((uint16_t) (index + tick * speed_multiplier)) % wavelength;
    if(half_wavelength < count) {
      count = half_wavelength - count;
    }

    float wave = (float) count / half_wavelength;
    return color * wave;
  }
};