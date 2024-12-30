#pragma once

#include <vector>
#include <functional>
#include <stdexcept>
#include "layer_generator.h"
#include "../layers/layer.h"
#include "../sequence_scheduler.h"

class SequenceGenerator {
    std::vector<std::pair<LayerGenerator*, float>> layerGens;
    std::vector<std::function<bool(std::vector<ILayer *>*)>> rules;
    u16_t minDuration;
    u16_t maxDuration;
    u16_t minCount;
    u16_t maxCount;

    public:
    SequenceGenerator(
        u16_t minDuration,
        u16_t maxDuration,
        u16_t minCount,
        u16_t maxCount
    );

    void addLayer(
        LayerGenerator* gen,
        float probability
    );

    void addRule(
        std::function<bool(std::vector<ILayer *>*)> rule
    );

    bool assertRules(std::vector<ILayer *>* layers);

    Sequence * getSequence();

    Animation * getAnimation();
};