#include "layer_generator.h"

void LayerGenerator::addGen(
    std::function<ILayer*()> gen,
    u16_t weight
) {
    layerGens.push_back({gen, weight});
    totalWeight += weight;
}
ILayer* LayerGenerator::getLayer() {
    u16_t r = random(totalWeight);
    for (const auto& pair : layerGens) {
        const auto& gen = pair.first;
        const auto& weight = pair.second;
        if (r < weight) {
        return gen();
        }
        r -= weight;
    }
    return nullptr;
}
