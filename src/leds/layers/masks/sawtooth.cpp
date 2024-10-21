#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <math.h>

class SawtoothMask : public ILayer {
  u16_t wavelength;
  u16_t duration;
  u16_t wave_gap;

  double mod(double a, double b) {
    return a - b * floor(a / b);
  }

  public:
  String getName() {
    return "Sawtooth Mask";
  }

  /**
   * @brief Construct a new Sawtooth Mask object
   *
   * @param wavelength The wavelength of the wave pattern.
   * @param wavegap The gap between waves.
   * @param duration The duration of the wave cycle.
   *
   * @example SawtoothMask(100, 200, 50)
   */
  SawtoothMask(u16_t wavelength, u16_t wavegap, u16_t duration) {
    this->wavelength = wavelength;
    this->wave_gap = wavegap;
    this->duration = duration;
  }

  /**
   * @brief Applies a sawtooth wave to the given color based on the current state (tick and index of led)
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    float totalLength = wave_gap + wavelength;
    float len = (float)state->length / duration;
    double x = ((double)state->tick * len + state->index) * state->direction;

    if (wavelength <= mod(x, totalLength)) {
      return CRGB::Black;
    }

    float intensity = 255 - (mod(x, totalLength) / wavelength) * 255;

    // turn 0 - 255 into a curve,
    intensity = intensity / 255.0;
    intensity = intensity * intensity * 255;

    return color.scale8(intensity);
  }
};