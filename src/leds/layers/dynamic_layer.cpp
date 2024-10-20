#include "layer.h"
#include <FastLED.h>

class DynamicLayer : public ILayer {
  private:
  ILayer* currentLayer;

  public:
  /**
   * @brief A dynamic layer that can be changed at runtime.
   *
   * @param initialLayer The initial layer to use.
   */
  DynamicLayer(ILayer* initialLayer = nullptr) : currentLayer(initialLayer) {}

  String getName() override {
    if (currentLayer) {
      return currentLayer->getName();
    }
    else {
      return "Empty Dynamic Layer";
    }
  }

  /**
   * @brief Set the current layer.
   *
   * @param newLayer The new layer to use.
   */
  void setLayer(ILayer* newLayer) {
    currentLayer = newLayer;
  }

  /**
   * @brief Deactivate the current layer.
   */
  void removeLayer() {
    currentLayer = nullptr;
  }

  /**
   * @brief Applies the current layer to the given color. If no layer is set, the original color is returned.
   *
   * @param color The original color of the LED.
   * @param state The current state of the LED, including the tick count.
   * @return The modified color after applying the current layer.
   */
  CRGB apply(CRGB color, LEDState* state) override {
    if (currentLayer) {
      return currentLayer->apply(color, state);
    }
    else {
      return color;
    }
  }
};