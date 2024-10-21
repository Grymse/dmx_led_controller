#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "layers/layer.h"
#include "utils/LEDstate.h"
#include "../scheduler/process.h"
#include "animator.h"


class Animator : public Process, public IAnimator {
  CRGB* leds;
  std::vector<ILayer*> layers;
  LEDState* state;
  u8_t brightness = 255;

  /**
   * @brief Reset the tick to 0 or 4320000 depending on the direction
   *
   */
  void resetTick() {
    if (state->direction == Direction::FORWARD) {
      state->tick = 0;
    }
    else {
      state->tick = 4320000; // 50 ticks * 3600 seconds * 24 hours
    }
  }

  public:
  /**
   * @brief Construct a new Animator object
   *
   * @param leds The LED strip to control
   * @param size The size of the LED strip
   */
  Animator(CRGB* leds, size_t size) {
    state = new LEDState{ 0, 0, size, Direction::FORWARD };
    this->leds = leds;
    clear();
  }

  String getName() override {
    return "Animator";
  }

  /**
   * @brief Clear the layers
   */
  void clear() override {
    this->layers = {};
  }

  /**
   * @brief Set master brightness of LEDs
   *
   * @param brightness The brightness of the animation
   */
  virtual void setBrightness(u8_t brightness) override {
    this->brightness = brightness;
  }

  /**
   * @brief Set the direction of the animation
   *
   * @param direction The direction of the animation
   */
  void setDirection(Direction direction) override {
    if (state->direction == direction) return;

    state->direction = direction;
    resetTick();
  }

  /**
   * @brief Set the layers to be used in the animation
   *
   * @param layers The layers to use
   */
  void setLayers(std::vector<ILayer*> layers) override {
    this->layers = layers;
    resetTick();
  }

  /**
   * @brief A method to update the LED strip with the current layers
   * It should be called every 20ms
   */
  void update() override {
    for (u16_t i = 0; i < state->length; i++) {
      state->index = i;

      if (layers.size() == 0) {
        leds[i] = CRGB::Black;
        continue;
      }

      for (ILayer* layer : layers) {
        leds[i] = layer->apply(leds[i], state);
      }

      leds[i] = leds[i].scale8(brightness);
    }

    // Tick should not exceed: 50 ticks * 3600 seconds * 24 hours
    state->tick = (state->tick + state->direction) % 4320000;
    FastLED.show();
  }
};