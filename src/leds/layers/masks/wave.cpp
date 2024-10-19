#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <math.h>
#include "../../../debug.cpp"

class WaveMask : public ILayer {
  u16_t wavelength;
  u16_t duration;
  u16_t wave_gap;

  float mod(float a, float b) {
    return a - b * floor(a / b);
  }

  public:
  String getName() {
    return "Wave Mask";
  }

  WaveMask(u16_t wavelength, u16_t wavegap, u16_t duration) {
    this->wavelength = wavelength;
    this->wave_gap = wavegap;
    this->duration = duration;
  }

  CRGB apply(CRGB color, LEDState* state) {
    float len = (float)state->length / duration;
    float x = (state->tick % duration) * len + state->index;
    if (wavelength <= mod(x, wavelength + wave_gap)) {
      return CRGB::Black;
    }

    float intensity = (mod(x, wavelength + wave_gap) / wavelength) * 512;

    if (255 < intensity) {
      intensity = 511 - intensity;
    }

    // turn 0 - 255 into a curve,
    intensity = intensity / 255.0;
    intensity = intensity * intensity * 255;

    return color.scale8(intensity);
  }
};