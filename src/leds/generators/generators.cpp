#include "sequence_generator.h"
#include "../layers/masks/masks.h"
#include "../layers/colors/colors.h"
#include "../sequence_scheduler.h"
#include <optional.h>
#include "utils.h"
#include "generators.h"

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
    {100, 20, 5},
    {150, 40, 1},
    {200, 10, 3},
    {400, 30, 1},
    {200, 30, 1},
    {200, 5, 1},
    {250, 5, 1},
};

u16_t wavelengths[] = {50, 100, 150, 300};
u16_t wavegaps[] = {150, 300, 600, 900, 1200};
u16_t durations[] = {25, 50, 100, 150};
std::vector<CRGB> myColors = {CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Yellow, CRGB::Fuchsia};
std::vector<CRGB> colorSets[] = {
    {CRGB::Red, CRGB::Lime, CRGB::Blue},                // Set 1 (3 colors)
    {CRGB::Fuchsia, CRGB::Aqua, CRGB::Yellow, CRGB::Red}, // Set 2 (4 colors)
    {CRGB::Blue, CRGB::Red},                             // Set 3 (2 colors)
    {CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Yellow}, // Set 4 (6 colors)
    {CRGB::Yellow, CRGB::Red, CRGB::Lime, CRGB::Fuchsia}, // Set 6 (4 colors)
};

SequenceGenerator * getTechnoGenerator() {
    auto colorLayer = new LayerGenerator();
    auto maskLayer = new LayerGenerator();
    auto effectLayer = new LayerGenerator();

    colorLayer->addGen([]() -> ILayer* {
        return new SingleColor(pickOne(myColors));
    }, 1);

    colorLayer->addGen([]() -> ILayer* {
        return new FadeColor(pickOne(colorSets), pickOne(durations));
    }, 1);

    colorLayer->addGen([]() -> ILayer* {
        return new RainbowColor(pickOne(durations), pickOne(wavelengths));
    }, 1);

    /* colorLayer->addGen([]() -> ILayer* {
        return new SectionsColor(pickOne(colorSets), pickOne(durations));
    }, 1);

    colorLayer->addGen([]() -> ILayer* {
        return new SectionsWaveColor(pickOne(colorSets), pickOne(durations));
    }, 1); */

    colorLayer->addGen([]() -> ILayer* {
        return new SwitchColor(pickOne(colorSets), pickOne(durations) / 3);
    }, 1);


    maskLayer->addGen([]() -> ILayer* {
        return new BlinkMask(pickOne(masks), pickOne(durations) / 5);
    }, 1);
    
    maskLayer->addGen([]() -> ILayer* {
        return new PulseSawtoothMask(pickOne(wavegaps), pickOne(durations));
    }, 1);

    maskLayer->addGen([]() -> ILayer* {
        return new PulseMask(pickOne(wavegaps), pickOne(durations));
    }, 1);

    maskLayer->addGen([]() -> ILayer* {
        return new SawtoothMask(pickOne(wavelengths), pickOne(wavegaps), pickOne(durations));
    }, 2);

    maskLayer->addGen([]() -> ILayer* {
        auto mask = pickOne(starmasks);
        return new StarsMask(mask[0] / 2, mask[1], mask[2]);
    }, 2);

    maskLayer->addGen([]() -> ILayer* {
        return new SectionsMask(pickOne(masks), pickOne(durations) / 2);
    }, 1);

    maskLayer->addGen([]() -> ILayer* {
        return new SectionsWaveMask(pickOne(masks), pickOne(durations));
    }, 1);

    maskLayer->addGen([]() -> ILayer* {
        return new WaveMask(pickOne(wavelengths), pickOne(wavegaps), pickOne(durations));
    }, 3);

    effectLayer->addGen([]() -> ILayer* {
        return new SawtoothMask(pickOne(wavelengths), pickOne(wavegaps), pickOne(durations));
    }, 1);

    effectLayer->addGen([]() -> ILayer* {
        auto mask = pickOne(starmasks);
        return new StarsMask(mask[0], mask[1], mask[2]);
    }, 5);
    effectLayer->addGen([]() -> ILayer* {
        return new BlinkMask(pickOne(masks), pickOne(durations));
    }, 1);

    SequenceGenerator * randomizer = new SequenceGenerator(100, 1200, 1, 5);
    randomizer->addLayer(colorLayer, 1);
    
    randomizer->addLayer(maskLayer, 1);
    randomizer->addLayer(effectLayer, 0.3f);
    randomizer->addRule([](std::vector<ILayer *>* layers) {
        if (layers->size() < 3) return true;
        auto layer1 = layers->at(1)->getName();
        auto layer2 = layers->at(2)->getName();
        return layer1 != layer2;
    });

    return randomizer;
}