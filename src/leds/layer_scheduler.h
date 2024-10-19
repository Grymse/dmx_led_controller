#include <vector>
#include "layers/layer.h"

#ifndef LAYER_SCHEDULER_H
#define LAYER_SCHEDULER_H

class ILayerScheduler {
public:
  virtual ~ILayerScheduler() {}

  virtual void clear() = 0;

  virtual void add(std::vector<ILayer*> pattern, u16_t tickDuration) = 0;
};
#endif