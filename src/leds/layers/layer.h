#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "protocol.pb.h"
#include "../state.h"

class ILayer {
  public:
  virtual ~ILayer() {}

  /**
   * @brief Get the Name object
   *
   * @return String
   */
  virtual String getName() = 0;

  /**
   * @brief To String
   * 
   * @return String
   */
  virtual String toString() = 0;

  /**
   * @brief Encode the layer into a protocol buffer.
   *
   * @return protocol_Layer
   */
  virtual protocol_Layer toEncodable() = 0;

  /**
   * @brief Apply the layer to the given color.
   *
   * @param color current color of the LED
   * @param state current state of the LED
   * @return modified color
   */
  virtual CRGB apply(CRGB color, LEDState* state) = 0;
};


class DynamicLayer : public ILayer {
  private:
  ILayer* currentLayer;

  public:
  DynamicLayer(ILayer* initialLayer = nullptr);

  String getName() override;
  void setLayer(ILayer* newLayer);
  void removeLayer();
  CRGB apply(CRGB color, LEDState* state) override;
};