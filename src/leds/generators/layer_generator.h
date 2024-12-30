#pragma once

#include <vector>
#include <functional>
#include <utility>
#include <cstdint>
#include "../layers/layer.h"
#include "Arduino.h"

class LayerGenerator {
    std::vector<std::pair<std::function<ILayer*()>, u16_t>> layerGens;
    u16_t totalWeight = 0;

public:
    void addGen(std::function<ILayer*()> gen, u16_t weight);
    ILayer* getLayer();
};
