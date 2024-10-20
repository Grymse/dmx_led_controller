#include <Arduino.h>
#include <FastLED.h>
#include "../layer.h"
#include <vector>

class StarsMask : public ILayer {
  u16_t frequency;
  u8_t decaySpeed;
  u8_t starLength;
  std::vector<u8_t> multipliers = {};


  void adjustVector(size_t length) {
    if (multipliers.size() != length) {
      if (multipliers.size() < length) {
        for (u16_t i = 0; i < length - multipliers.size(); i++) {
          multipliers.push_back(0);
        }
      }
      else {
        for (u16_t i = 0; i < multipliers.size() - length; i++) {
          multipliers.pop_back();
        }
      }
    }
  }

  void brightenNeighbourLEDs(LEDState* state) {
    for (u8_t i = 1; i <= starLength; i++) {
      u16_t left = (state->index - i) % state->length;
      u16_t right = (state->index + i) % state->length;
      u8_t scale = 255 / ((float)i + .5f);
      multipliers[left] = max(scale, multipliers[left]);
      multipliers[right] = max(scale, multipliers[right]);
    }
  }

  u8_t decay(u8_t multiplier) {
    if (multiplier == 0 || multiplier < decaySpeed) return 0;
    return multiplier - decaySpeed;
  }

  public:
  String getName() {
    return "Stars Mask";
  }

  /**
   * @brief Construct a new Stars Mask object
   * @param frequency amount of stars spawning every second
   * @param decaySpeed speed at which a star decays. 0 is never, 255 is instant. It decays once per tick
   * @param starLength length of a star in LEDs
   * @example StarsMask(20, 5, 5)
   */
  StarsMask(u16_t frequency, u8_t decaySpeed, u8_t starLength) {
    this->frequency = frequency;
    this->decaySpeed = decaySpeed;
    this->starLength = starLength / 2;
  }

  /**
   * @brief Applies star-effect based on the current state (tick and index of led)
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the blink pattern.
   */
  CRGB apply(CRGB color, LEDState* state) {
    adjustVector(state->length);
    u8_t multiplier = decay(multipliers[state->index]);

    bool drawNewStar = random(0, state->length * 50) < frequency;
    if (drawNewStar) {
      multiplier = 255;
      brightenNeighbourLEDs(state);
    }

    multipliers[state->index] = multiplier;
    return color.scale8(multiplier);
  }
};