#include "sequence_generator.h"
#include "utils.h"

u16_t seq_durations[] = {100, 150, 200, 300/* , 500, 1000, 1500, 2000, 3000, 5000, 10000 */};

SequenceGenerator::SequenceGenerator(
    u16_t minDuration,
    u16_t maxDuration,
    u16_t minCount,
    u16_t maxCount
) : minDuration(minDuration), maxDuration(maxDuration), minCount(minCount), maxCount(maxCount) {
    // check if any seq_duration is within the min and max duration
    for (const auto& duration : seq_durations) {
        if (duration >= minDuration && duration <= maxDuration) {
            return;
        }
    }
    throw std::runtime_error("No valid sequence duration found");
};

void SequenceGenerator::addLayer(
    LayerGenerator* gen,
    float probability
) {
    layerGens.push_back({gen, probability});
}

void SequenceGenerator::addRule(
    std::function<bool(std::vector<ILayer *>*)> rule
) {
    rules.push_back(rule);
}

bool SequenceGenerator::assertRules(std::vector<ILayer *>* layers) {
    for (const auto& rule : rules) {
        if (!rule(layers)) return false;
    }
    return true;
}

Sequence * SequenceGenerator::getSequence() {
    u16_t count = random(maxCount - minCount) + minCount;
    Sequence * sequence = new Sequence{
        .brightness = 175
    };

    for (u16_t i = 0; i < count; i++) {
        sequence->animations.push_back(getAnimation());
    }

    return sequence;
}

Animation * SequenceGenerator::getAnimation() {
    u16_t duration;
    do {
        duration = pickOne(seq_durations);
    } while (duration < minDuration || duration > maxDuration);

    Direction direction = random(2) == 0 ? Direction::FORWARD : Direction::BACKWARD;

    Animation * animation = new Animation{
        .tickDuration = duration,
        .direction = direction
    };

    do {
        animation->layers.clear();

        for (const auto& pair : layerGens) {
            const auto& gen = pair.first;
            const auto& probability = pair.second;

            if (random(10000) >= probability * 10000) continue;

            animation->layers.push_back(gen->getLayer());
        }
    } while (!assertRules(&animation->layers));

    return animation;
}