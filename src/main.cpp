// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Lib
#include <pb_encode.h>
// Local
#include "dmx.h"
#include "scheduler/scheduler.h"
#include "leds/animator.h"
#include "leds/sequence_scheduler.h"
#include "debug.h"
#include "leds/serialization/sequence_decoder.h"
#include "leds/serialization/sequence_encoder.h"
#include "leds/layers/masks/masks.h"
#include "leds/layers/colors/colors.h"
#include "connectivity/radio.h"
#include "connectivity/espnow.h"
#include "leds/generators/generators.h"

#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
CRGB* leds = new CRGB[NUM_LEDS];
RF24 radio = RF24(CE_PIN, CSN_PIN);
u8_t frames_per_second = 40;


u16_t LED_OFFSET = 300; // 300$
u16_t DMX_START = 3; // 3, 18

ProcessScheduler scheduler;
Animator* animator;
SequenceScheduler* sequenceScheduler;

u8_t channels[16];
u8_t prevChannels[16];

std::vector<u8_t> * to_sections(u8_t value) {
    std::vector<u8_t> * sections = new std::vector<u8_t>();
    sections->resize(value + 1);
    sections->push_back(255);
    return sections;
}


u16_t valueScaler(u16_t value) {
    if (value < 32) {
        return value;
    } else if (value < 64) {
        return (value - 32) * 2 + 32;
    } else if (value < 128) {
        return (value - 64) * 4 + 96;
    } else if (value < 192) {
        return (value - 128) * 8 + 352;
    } else {
        return (value - 192) * 16 + 864;
    }
}

ILayer * dmx_to_animation(u8_t* channels) {
    switch(channels[0]) {
        case 1: // WaveMask
            return new WaveMask(valueScaler(channels[1]), valueScaler(channels[2]), valueScaler(channels[3]));
        case 2: // StarsMask
            return new StarsMask(valueScaler(channels[1]), channels[2], channels[3]);
        case 3: 
            return new BlinkMask(*to_sections(channels[1]), valueScaler(channels[2]));
        case 4: // SectionsMask
            return new SectionsMask(*to_sections(channels[1]), valueScaler(channels[2]));
        case 5: // PulseSawtoothMask
            return new PulseSawtoothMask(valueScaler(channels[1]), valueScaler(channels[2]));
        case 6: // PulseMask
            return new PulseMask(valueScaler(channels[1]), valueScaler(channels[2]));
        case 7: // SawtoothMask
            return new SawtoothMask(valueScaler(channels[1]), valueScaler(channels[2]), valueScaler(channels[3]));
        case 8: // SectionsWaveMask
            return new SectionsWaveMask(*to_sections(channels[1]), valueScaler(channels[2]));
        default:
            return nullptr; // Unknown layer type
    }
}


bool hasChannelsChanged(u8_t* channels, u8_t from, u8_t to) {
    for (int i = from; i <= to; i++) {
        if (channels[i] != prevChannels[i]) {
            return true;
        }
    }
    return false;
}

SingleColor* color = new SingleColor(CRGB::Black);

void dmx_to_animation(Animator* animator, u8_t* channels) {
    if(hasChannelsChanged(channels, 0, 7)) {
        animator->setBrightness(channels[1]);
        color->setColor(CRGB(channels[2], channels[3], channels[4]));
        animator->setDirection(channels[5] < 128 ? Direction::FORWARD : Direction::BACKWARD);
    }
    if(hasChannelsChanged(channels, 8, 15)) {
        ILayer* mask1 = dmx_to_animation(channels + 8);
        ILayer* mask2 = dmx_to_animation(channels + 12);
        if (mask1 != nullptr && mask2 != nullptr) {
            // Two masks, combine them
            animator->setLayers({color, mask1, mask2});
            /* printf(color->toString().c_str());
            printf(mask1->toString().c_str());
            printf(mask2->toString().c_str()); */
        } else if (mask1 != nullptr) {
            // One mask
            animator->setLayers({color, mask1});
            /* printf(color->toString().c_str());
            printf(mask1->toString().c_str()); */
        } else {
            // Only color layer
            animator->setLayers({color});
            /* printf(color->toString().c_str()); */
        }
        animator->setTick(channels[5] % 128);
    }
    memcpy(prevChannels, channels, sizeof(prevChannels));
}

class ReadDMXProcess : public Process {

  public:
  String getName() {
    return "DMX Reader";
  }

  void update() {
    if (DMX::Read(1) != 123 || DMX::Read(2) != 234) {
      //printf("DMX not ready, skipping read\n");
      return;
    }

    for (int i = 0; i < 15; ++i) {
        /* printf("%d,", DMX::Read(i + DMX_START)); */
        channels[i+1] = DMX::Read(i + DMX_START);
    }
    /* printf("\n"); */
    dmx_to_animation(animator, channels);

  }

  ReadDMXProcess() {
    DMX::Initialize(input);
  }
};


void setup() {
  // put your setup code here, to run once:
  scheduler = ProcessScheduler();
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  animator = new Animator(leds, NUM_LEDS);

  scheduler.addProcess(animator, 1000 / frames_per_second); // Update every 20ms */
  scheduler.addProcess(new ReadDMXProcess(), 25); // Update every 1000ms

  animator->setVirtualOffset(LED_OFFSET);
}

void loop() {
  scheduler.update();
  delay(1); // Stability
}