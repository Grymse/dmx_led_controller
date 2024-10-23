#include <vector>
#include "layers/layer.h"

#ifndef ANIMATOR_H
#define ANIMATOR_H
#define ANIMATION_DURATION_MAX 4320000 // 50 * 3600 * 24

class IAnimator {
  public:
  virtual ~IAnimator() {}

  /**
   * @brief Clear the layers used in the animation
   *
   */
  virtual void clear() = 0;

  /**
   * @brief Set the direction of the animation
   *
   * @param direction The direction of the animation
   */
  virtual void setDirection(Direction direction) = 0;

  /**
   * @brief Set master brightness of LEDs
   *
   * @param brightness The brightness of the animation
   */
  virtual void setBrightness(u8_t brightness) = 0;

  /**
   * @brief Set the direction of the animation
   *
   * @param layers The layers to use
   */
  virtual void setLayers(std::vector<ILayer*> layers) = 0;
};

#endif