#include <vector>
#include "layers/layer.h"

#ifndef LAYER_CONTROLLER_H
#define LAYER_CONTROLLER_H

class ILayerController {
  public:
  virtual ~ILayerController() {}

  /**
   * @brief Clear the layers used in the animation
   *
   */
  virtual void clear() = 0;

  /**
   * @brief Set the direction of the animation
   *
   * @param layers The layers to use
   */
  virtual void set(std::vector<ILayer*> layers) = 0;
};

#endif