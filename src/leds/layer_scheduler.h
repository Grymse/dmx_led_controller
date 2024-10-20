#include <vector>
#include "layers/layer.h"

#ifndef LAYER_SCHEDULER_H
#define LAYER_SCHEDULER_H

class ILayerScheduler {
  public:
  virtual ~ILayerScheduler() {}

  /**
   * @brief Clear the scheduler
   *
   */
  virtual void clear() = 0;

  /**
   * @brief Add a pattern to the scheduler
   *
   * @param pattern The pattern to add
   * @param tickDuration The duration of the pattern
   */
  virtual void add(std::vector<ILayer*> pattern, u16_t tickDuration) = 0;
};
#endif