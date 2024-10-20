#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "layers/layer.h"
#include "utils/LEDstate.h"
#include "../scheduler/process.h"
#include "../debug.cpp"
#include "layer_controller.h"


class LayerController : public Process, public ILayerController {
  CRGB* leds;
  std::vector<ILayer*> layers;
  LEDState* state;

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
   * @brief Construct a new Layer Controller object
   *
   * @param leds The LED strip to control
   * @param size The size of the LED strip
   */
  LayerController(CRGB* leds, size_t size) {
    state = new LEDState{ 0, 0, size, Direction::FORWARD };
    this->leds = leds;
    clear();
  }

  String getName() override {
    return "Layer Controller";
  }

  /**
   * @brief Clear the layers
   */
  void clear() {
    this->layers = {};
  }

  /**
   * @brief Set the direction of the animation
   *
   * @param direction The direction of the animation
   */
  void setDirection(Direction direction) {
    if (state->direction == direction) return;

    state->direction = direction;
    resetTick();
  }

  /**
   * @brief Set the layers to be used in the animation
   *
   * @param layers The layers to use
   */
  void set(std::vector<ILayer*> layers) {
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

    }

    // Tick should not exceed: 50 ticks * 3600 seconds * 24 hours
    state->tick = (state->tick + state->direction) % 4320000;
    FastLED.show();
  }
};