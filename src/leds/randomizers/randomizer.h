#pragma once
#include "../layers/masks/masks.h"
#include "../layers/colors/colors.h"
#include "../sequence_scheduler.h"
#include <optional.h>


template <typename T, size_t N>
T pickOne(T (&arr)[N]) {
    return arr[random(N)];
}

std::vector<u8_t> masks[] = {
    {255, 0},
    {255, 0, 0, 0},
    {255, 0, 0, 0, 0, 0},
    {255, 0, 0, 0, 0, 0, 0, 0},
    {255, 0, 255, 0, 0, 0},
    {255, 0, 255, 0, 255, 0, 0, 0, 0},
    {255, 0, 255, 0, 255, 0, 255, 0, 0, 0, 0, 0},
    {255, 100, 0, 0},
    {255, 100, 0, 0, 0, 0},
    {255, 100, 0},
    {100, 255, 100, 0},
    {100, 255, 100, 0, 0, 0},
};

std::vector<u16_t> starmasks[] = {
    {100, 20, 5},
    {200, 10, 3},
    {400, 30, 1},
    {200, 30, 1},
    {200, 5, 1},
    {250, 5, 1},
};

u16_t wavelengths[] = {50, 100, 150, 300, 600, 900, 1200};
u16_t wavegaps[] = {50, 100, 150, 300, 600, 900, 1200};
u16_t durations[] = {25, 50, 100, 150, 200, 300, 500};
u16_t seq_durations[] = {25, 50, 100, 150, 200, 300, 500, 1000, 1500, 2000, 3000, 5000, 10000};
CRGB myColors[] = {CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Yellow, CRGB::Fuchsia, CRGB::Aqua};
/* std::vector<CRGB> colorSets[] = {
    {CRGB::Red, CRGB::Yellow, CRGB::Blue, CRGB::Lime},    // Set 1
    {CRGB::Fuchsia, CRGB::Aqua, CRGB::Yellow, CRGB::Red}, // Set 2
    {CRGB::Blue, CRGB::Lime, CRGB::Fuchsia, CRGB::Aqua},  // Set 3
    {CRGB::Yellow, CRGB::Red, CRGB::Lime, CRGB::Fuchsia}, // Set 4
    {CRGB::Aqua, CRGB::Blue, CRGB::Red, CRGB::Yellow}     // Set 5
}; */

std::vector<CRGB> colorSets[] = {
    {CRGB::Red, CRGB::Lime, CRGB::Blue},                // Set 1 (3 colors)
    {CRGB::Fuchsia, CRGB::Aqua, CRGB::Yellow, CRGB::Red}, // Set 2 (4 colors)
    {CRGB::Blue, CRGB::Red},                             // Set 3 (2 colors)
    {CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Yellow, CRGB::Fuchsia, CRGB::Aqua}, // Set 4 (6 colors)
    {CRGB::Aqua},                                          // Set 5 (1 color)
    {CRGB::Yellow, CRGB::Red, CRGB::Lime, CRGB::Fuchsia}, // Set 6 (4 colors)
};


class LayerRandomizer {
    std::vector<std::pair<std::function<ILayer*()>, u16_t>> layerGens;
    u16_t totalWeight = 0;

    public:
    void addGen(
        std::function<ILayer*()> gen,
        u16_t weight
    ) {
        layerGens.push_back({gen, weight});
        totalWeight += weight;
    }
    ILayer* getLayer() {
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
};

class SequenceRandomizer {
    std::vector<std::pair<LayerRandomizer*, float>> layerGens;
    std::vector<std::function<bool(std::vector<ILayer *>*)>> rules;
    u16_t minDuration;
    u16_t maxDuration;
    u16_t minCount;
    u16_t maxCount;

    public:
    SequenceRandomizer(
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

    void addLayer(
        LayerRandomizer* gen,
        float probability
    ) {
        layerGens.push_back({gen, probability});
    }

    void addRule(
        std::function<bool(std::vector<ILayer *>*)> rule
    ) {
        rules.push_back(rule);
    }

    bool assertRules(std::vector<ILayer *>* layers) {
        for (const auto& rule : rules) {
            if (!rule(layers)) return false;
        }
        return true;
    }

    Sequence * getSequence() {
        u16_t count = random(maxCount - minCount) + minCount;
        Sequence * sequence = new Sequence{
            .brightness = 255
        };

        for (u16_t i = 0; i < count; i++) {
            sequence->animations.push_back(getAnimation());
        }

        return sequence;
    }

    Animation * getAnimation() {
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
};

void buildLayers() {
    LayerRandomizer colorLayer;
    LayerRandomizer maskLayer;
    LayerRandomizer effectLayer;

    colorLayer.addGen([]() -> ILayer* {
        return new SingleColor(pickOne(myColors));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        return new FadeColor(pickOne(colorSets), pickOne(durations));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        return new RainbowColor(pickOne(durations), pickOne(wavelengths));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        return new SectionsColor(pickOne(colorSets), pickOne(durations));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        return new SectionsWaveColor(pickOne(colorSets), pickOne(durations));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        return new SwitchColor(pickOne(colorSets), pickOne(durations));
    }, 1);


    maskLayer.addGen([]() -> ILayer* {
        return new BlinkMask(pickOne(masks), pickOne(durations));
    }, 1);
    maskLayer.addGen([]() -> ILayer* {
        return new PulseSawtoothMask(pickOne(wavegaps), pickOne(durations));
    }, 1);
    maskLayer.addGen([]() -> ILayer* {
        return new PulseMask(pickOne(wavegaps), pickOne(durations));
    }, 1);
    maskLayer.addGen([]() -> ILayer* {
        return new SawtoothMask(pickOne(wavelengths), pickOne(wavegaps), pickOne(durations));
    }, 1);

    maskLayer.addGen([]() -> ILayer* {
        auto mask = pickOne(starmasks);
        return new StarsMask(mask[0], mask[1], mask[2]);
    }, 1);

    maskLayer.addGen([]() -> ILayer* {
        return new SectionsMask(pickOne(masks), pickOne(durations));
    }, 1);

    maskLayer.addGen([]() -> ILayer* {
        return new SectionsWaveMask(pickOne(masks), pickOne(durations));
    }, 1);

    maskLayer.addGen([]() -> ILayer* {
        return new WaveMask(pickOne(wavelengths), pickOne(wavegaps), pickOne(durations));
    }, 1);


    effectLayer.addGen([]() -> ILayer* {
        return new SawtoothMask(pickOne(wavelengths), pickOne(wavegaps), pickOne(durations));
    }, 1);

    effectLayer.addGen([]() -> ILayer* {
        auto mask = pickOne(starmasks);
        return new StarsMask(mask[0], mask[1], mask[2]);
    }, 1);
    effectLayer.addGen([]() -> ILayer* {
        return new BlinkMask(pickOne(masks), pickOne(durations));
    }, 1);


    SequenceRandomizer randomizer(25, 1200, 1, 5);
    randomizer.addLayer(&colorLayer, 1);
    randomizer.addLayer(&maskLayer, 1);
    randomizer.addLayer(&effectLayer, 0.3f);
    randomizer.addRule([](std::vector<ILayer *>* layers) {
        if (layers->size() < 3) return true;
        auto layer1 = layers->at(1)->getName();
        auto layer2 = layers->at(2)->getName();
        return layer1 != layer2;
    });

    while(true) {
        auto s = randomizer.getSequence();
        printf("Sequence: \n");
        for (auto animation : s->animations) {
            printf("  Animation: %d%s\n", animation->tickDuration, animation->direction == Direction::FORWARD ? "F" : "B");
            for (auto layer : animation->layers) {
                printf("    %s\n", layer->getName().c_str());
            }
        }

        printf("\n\n");
        delay(1000);
    }

}

// TODO: Refactor to c++ standards