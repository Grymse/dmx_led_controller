#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <math.h>
#include "../../../debug.cpp"

class SawtoothMask : public ILayer {
  u16_t wavelength;
  u16_t duration;
  u16_t wave_gap;

public:
  String getName() {
    return "Sawtooth Mask";
  }

  SawtoothMask(u16_t wavelength, u16_t wavegap, u16_t duration) {
    this->wavelength = wavelength;
    this->wave_gap = wavegap;
    this->duration = duration;
  }

  float mod(float a, float b) {
    return a - b * floor(a / b);
  }

  CRGB apply(CRGB color, LEDState *state) {
    float len = (float)state->length / duration;
    float x = (state->tick % duration) * len + state->index;
    if(wavelength <= mod(x, wavelength + wave_gap) ) {
      return CRGB::Black;
    }

    float intensity = 255 - (mod(x, wavelength + wave_gap) / wavelength) * 255;

    // turn 0 - 255 into a curve,
    intensity = intensity / 255.0;
    intensity = intensity * intensity * 255;

    return color.scale8(intensity);
  }
};