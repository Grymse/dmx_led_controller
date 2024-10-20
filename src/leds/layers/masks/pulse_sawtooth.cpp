#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <math.h>
#include "../../../debug.cpp"

/**
 * @brief Constructs a new PulseSawtoothMask object.
 *
 * @param pulse_gap The gap between pulses in ticks.
 * @param duration The duration of each pulse in ticks.
 *
 * @example
 * PulseSawtoothMask mask(10, 50);
 * // Creates a PulseSawtoothMask with a pulse gap of 10 ticks and a duration of 50 ticks.
 */
 // PulseSawtoothMask(u16_t pulse_gap, u16_t duration);

CRGB apply(CRGB color, LEDState* state);
class PulseSawtoothMask : public ILayer {
  u16_t duration;
  u16_t pulse_gap;

  public:
  String getName() {
    return "Pulse Sawtooth Mask";
  }

  /**
   * @brief Construct a new Pulse Sawtooth Mask object
   *
   * @param pulse_gap The gap between pulses in ticks.
   * @param duration The duration of each pulse in ticks.
   *
   * @example PulseSawtoothMask(50, 50)
   */
  PulseSawtoothMask(u16_t pulse_gap, u16_t duration) {
    this->pulse_gap = pulse_gap;
    this->duration = duration;
  }

  /**
   * @brief Applies a sawtooth pulse wave to the given color based on the current state (tick and index of led)
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    float intensity = (float)(state->tick % (duration + pulse_gap)) / duration;
    if (1.f < intensity) {
      return CRGB::Black;
    }

    return color.scale8(intensity * 255);
  }
};