#include <vector>
#include "layers/layer.h"

#ifndef LAYER_SCHEDULER_H
#define LAYER_SCHEDULER_H

struct Animation {
  std::vector<ILayer*> layers;
  u16_t tickDuration;
  Direction direction;
};

struct Sequence {
  std::vector<Animation*> animations;
  u8_t brightness;
};

class ISequenceScheduler {
  public:
  virtual ~ISequenceScheduler() {}

  /**
   * @brief Clear the scheduler
   *
   */
  virtual void clear() = 0;

  /**
   * @brief Add an animation to the scheduler
   *
   * @param layers The layers to add
   * @param tickDuration The duration of the animation
   * @param direction The direction of the animation
   */
  virtual void add(std::vector<ILayer*> layers, u16_t tickDuration, Direction direction) = 0;

  /**
   * @brief Add an animation to the scheduler
   *
   * @param animation The animation to add
   */
  virtual void add(Animation* animation) = 0;

  /**
   * @brief Set the Brightness of the LED strip
   *
   * @param brightness new brightness 0-255
   */
  virtual void setBrightness(u8_t brightness) = 0;

  /**
   * @brief Set sequence of animations in the scheduler
   *
   * @param animations The new sequence of animations
   * @param brightness The brightness of the sequence (Optional)
   */
  virtual void set(std::vector<Animation*> animations, u8_t brightness = 255) = 0;

  /**
   * @brief Set sequence of animations in the scheduler
   *
   * @param sequence The new sequence of animations
   */
  virtual void set(Sequence* sequence) = 0;
};
#endif