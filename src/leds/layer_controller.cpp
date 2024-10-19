#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "layers/layer.h"
#include "utils/LEDstate.h"
#include "../scheduler/process.h"
#include "../debug.cpp"
#include "layer_controller.h"

// TODO: Add direction, such that the effect starts from the opposite side

class LayerController : public Process, public ILayerController {
  CRGB *leds;
  std::vector<ILayer*> layers;
  LEDState* state;

public:
  LayerController(CRGB * leds, size_t size) {
    state = new LEDState{0, 0, size};
    this->leds = leds;
    clear();
  }

  void clear() {
    this->layers = {};
  }
  
  void set(std::vector<ILayer*> layers) {
    state->tick = 0;
    this->layers = layers;
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
    state->tick++;
    FastLED.show();
  }
};