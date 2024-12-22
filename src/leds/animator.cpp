#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "animator.h"
#include "layers/layer.h"
#include "../scheduler/scheduler.h"


/**
 * @brief Reset the tick to 0 or 4320000 depending on the direction
 *
 */
void Animator::resetTick() {
  if (state->direction == Direction::FORWARD) {
    state->tick = 0;
  }
  else {
    state->tick = ANIMATION_DURATION_MAX;
  }
}

/**
 * @brief Construct a new Animator object
 *
 * @param leds The LED strip to control
 * @param size The size of the LED strip
 */
Animator::Animator(CRGB* leds, size_t size) {
  state = new LEDState{ 0, 0, size, Direction::FORWARD };
  this->leds = leds;
  clear();
}

String Animator::getName() {
  return "Animator";
}

/**
 * @brief Clear the layers
 */
void Animator::clear() {
  this->layers = {};
}

/**
 * @brief Set master brightness of LEDs
 *
 * @param brightness The brightness of the animation
 */
void Animator::setBrightness(u8_t brightness) {
  this->brightness = brightness;
}

/**
 * @brief Set the direction of the animation
 *
 * @param direction The direction of the animation
 */
void Animator::setDirection(Direction direction) {
  if (state->direction == direction) return;

  state->direction = direction;
  resetTick();
}

/**
 * @brief Set the layers to be used in the animation
 *
 * @param layers The layers to use
 */
void Animator::setLayers(std::vector<ILayer*> layers) {
  this->layers = layers;

  resetTick();
}

/**
 * @brief Get the current brightness of the animation
 *
 * @return u8_t
 */
u8_t Animator::getBrightness() {
  return brightness;
}

/**
 * @brief A method to update the LED strip with the current layers
 * It should be called every 20ms
 */
void Animator::update() {
  for (u16_t i = 0; i < state->length; i++) {
    state->index = i;

    if (layers.size() == 0) {
      leds[i] = CRGB::Black;
      continue;
    }

    for (ILayer* layer : layers) {
      leds[i] = layer->apply(leds[i], state);
    }
  }

  // Tick should not exceed max
  state->tick = (state->tick + state->direction) % ANIMATION_DURATION_MAX;
  FastLED.show(brightness);
}