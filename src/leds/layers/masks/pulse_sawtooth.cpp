#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <math.h>
#include "../../../debug.cpp"

class PulseSawtoothMask : public ILayer {
  u16_t duration;
  u16_t pulse_gap;

public:
  String getName() {
    return "Pulse Sawtooth Mask";
  }

  PulseSawtoothMask(u16_t pulse_gap, u16_t duration) {
    this->pulse_gap = pulse_gap;
    this->duration = duration;
  }

  float mod(float a, float b) {
    return a - b * floor(a / b);
  }

  CRGB apply(CRGB color, LEDState *state) {
    float intensity = (float)(state->tick % (duration + pulse_gap)) / duration;
    if(1.f < intensity) {
      return CRGB::Black;
    }

    return color.scale8(intensity);
  }
};