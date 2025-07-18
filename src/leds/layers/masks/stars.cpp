#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "masks.h"

/**
 * @brief Adjusts the size of the multipliers vector to match the given length.
 *
 * This function ensures that the multipliers vector has the specified length.
 * If the current size of the vector is less than the specified length, it
 * appends zeros to the vector until the sizes match. If the current size is
 * greater than the specified length, it removes elements from the end of the
 * vector until the sizes match.
 *
 * @param length The desired length of the multipliers vector.
 */
void StarsMask::adjustVector(size_t length) {
  if (this->multipliers.size() != length) {
    if (this->multipliers.size() < length) {
      for (u16_t i = 0; i < length - this->multipliers.size(); i++) {
        this->multipliers.push_back(0);
      }
    }
    else {
      for (u16_t i = 0; i < this->multipliers.size() - length; i++) {
        this->multipliers.pop_back();
      }
    }
  }
}

/**
 * @brief Brightens the neighboring LEDs around the current LED.
 *
 * This function increases the brightness of the LEDs adjacent to the current
 * LED based on the starLength. The brightness decreases as the distance from
 * the current LED increases.
 *
 * @param state The current state of the LED, including the index and length of the LED strip.
 */
void StarsMask::brightenNeighbourLEDs(LEDState* state) {
  for (u8_t i = 1; i <= this->starLength / 2; i++) {
    u16_t left = (state->index - i) % state->length;
    u16_t right = (state->index + i) % state->length;
    u8_t scale = 255 / ((float)i + .5f);
    this->multipliers[left] = max(scale, this->multipliers[left]);
    this->multipliers[right] = max(scale, this->multipliers[right]);
  }
}

/**
 * @brief Decays the brightness multiplier of an LED.
 *
 * This function reduces the brightness multiplier of an LED by the decaySpeed.
 * If the multiplier is already zero or less than the decaySpeed, it returns zero.
 *
 * @param multiplier The current brightness multiplier of the LED.
 * @return The new brightness multiplier after applying the decay.
 */
u8_t StarsMask::decay(u8_t multiplier) {
  if (multiplier == 0 || multiplier < this->decaySpeed) return 0;
  return multiplier - this->decaySpeed;
}

String StarsMask::getName() {
  return "Stars Mask";
}

/**
 * @brief Construct a new Stars Mask object
 * @param frequency amount of stars spawning every second
 * @param decaySpeed speed at which a star decays. 0 is never, 255 is instant. It decays once per tick
 * @param starLength length of a star in LEDs
 * @example StarsMask(20, 5, 5)
 */
StarsMask::StarsMask(u16_t frequency, u8_t decaySpeed, u8_t starLength) {
  this->frequency = frequency;
  this->decaySpeed = decaySpeed;
  this->starLength = starLength;
}

String StarsMask::toString() {
  return "StarsMask: f: " + String(this->frequency) + ", s: " + String(this->decaySpeed) + ", l: " + String(this->starLength);
}

/**
 * @brief Applies star-effect based on the current state (tick and index of led)
 * @param color The original color of the LED.
 * @param state The current state of the LED, including the tick count.
 * @return The modified color after applying the blink pattern.
 */
CRGB StarsMask::apply(CRGB color, LEDState* state) {
  u8_t starLength = this->starLength / 2;
  adjustVector(state->length);
  u8_t multiplier = decay(this->multipliers[state->index]);

  bool drawNewStar = random(0, state->length * 50) < this->frequency;
  if (drawNewStar) {
    multiplier = 255;
    brightenNeighbourLEDs(state);
  }

  this->multipliers[state->index] = multiplier;
  return color.scale8(multiplier);
}

protocol_Layer StarsMask::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_StarsMask,
    .length = this->starLength,
    .frequency = this->frequency,
    .speed = this->decaySpeed,
  };
}
