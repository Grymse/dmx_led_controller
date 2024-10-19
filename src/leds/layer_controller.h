#include <vector>
#include "layers/layer.h"

#ifndef LAYER_CONTROLLER_H
#define LAYER_CONTROLLER_H

class ILayerController {
public:
  virtual ~ILayerController() {}

  virtual void clear() = 0;

  virtual void set(std::vector<ILayer*> layers)  = 0;
};

#endif