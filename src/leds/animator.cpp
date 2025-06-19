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


void Animator::setVirtualOffset(u16_t virtual_offset) {
  this->virtual_offset = virtual_offset;
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

  if (layers.size() == 0) {
    for (u16_t i = 0; i < state->length; i++) {
      leds[i] = CRGB::Black;
    }
  }

  for (ILayer* layer : layers) {
    /* auto before = millis(); */
    for (u16_t i = 0; i < state->length; i++) {
        state->index = i;
        state->virtual_index = (i + virtual_offset);
        leds[i] = layer->apply(leds[i], state);
    }
    /* auto after = millis();
    printf("Layer %s took %d ms\n", layer->getName().c_str(), after - before); */
  }

  // Tick should not exceed max
  state->tick = (state->tick + state->direction) % ANIMATION_DURATION_MAX;
  FastLED.show(brightness);
}