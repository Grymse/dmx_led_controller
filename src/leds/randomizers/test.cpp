#include "../layers/masks/masks.h"
#include "../layers/colors/colors.h"
#include <optional.h>


template <typename T, size_t N>
T pickOne(T (&arr)[N]) {
    return arr[random(N)];
}


u16_t wavelengths[] = {50, 100, 150, 300, 600, 900, 1200};
u16_t wavegaps[] = {50, 100, 150, 300, 600, 900, 1200};
u16_t durations[] = {25, 50, 100, 150, 200, 300, 500};
CRGB myColors[] = {CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Yellow, CRGB::Fuchsia, CRGB::Aqua};

class LayerRandomizer {
    float probability;
    std::vector<std::pair<std::function<ILayer*()>, u16_t>> layerGens;
    u16_t totalWeight = 0;

    public:
    LayerRandomizer(float probability) {
        this->probability = probability;
    };
    void addGen(
        std::function<ILayer*()> gen,
        u16_t weight
    ) {
        layerGens.push_back({gen, weight});
        totalWeight += weight;
    }
    Option<ILayer*> getLayer() {
        if (random(10000) >= probability * 10000) return Option<ILayer*>::empty();

        u16_t r = random(totalWeight);
        for (const auto& pair : layerGens) {
            const auto& gen = pair.first;
            const auto& weight = pair.second;
            if (r < weight) {
            return gen();
            }
            r -= weight;
        }
        return Option<ILayer*>::empty();
    }
};

void buildLayers() {
    LayerRandomizer colorLayer(1);
    LayerRandomizer maskLayer(1);

    colorLayer.addGen([]() -> ILayer* {
        auto c = pickOne(myColors);
        return new SingleColor(c);
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        // TODO: Fix random chooser
        return new FadeColor({CRGB::Red, CRGB::Green, CRGB::Blue}, pickOne(durations));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        return new RainbowColor(pickOne(durations), pickOne(wavelengths));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        // TODO: Fix random chooser
        return new SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, pickOne(durations));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        // TODO: Fix random chooser
        return new SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, pickOne(durations));
    }, 1);

    colorLayer.addGen([]() -> ILayer* {
        // TODO: Fix random chooser
        return new SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, pickOne(durations));
    }, 1);


    maskLayer.addGen([]() -> ILayer* {
        // TODO: Fix byte-array generator
        return new BlinkMask({255, 0, 0, 0}, pickOne(durations));
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
        // TODO: Fix numbers
        return new StarsMask(200, 10, 3);
    }, 1);

    maskLayer.addGen([]() -> ILayer* {
        return new SectionsMask({255, 0, 0, 255, 0}, pickOne(durations));
    }, 1);

    maskLayer.addGen([]() -> ILayer* {
        return new SectionsWaveMask({0}, pickOne(durations));
    }, 1);

    maskLayer.addGen([]() -> ILayer* {
        return new WaveMask(pickOne(wavelengths), pickOne(wavegaps), pickOne(durations));
    }, 1);
}

// TODO: Refactor to use a more generic randomizer class.
// TODO: Build a sequence randomizer.
// TODO: Add ability for rules, such that certain things cannot be next to each other.
// TODO: Refactor to c++ standards