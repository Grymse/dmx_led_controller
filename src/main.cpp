// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Local
#include "scheduler/scheduler.h"
#include "leds/animator.h"
#include "leds/sequence_scheduler.h"
#include "debug.h"
#include "leds/sequence_decoder.h"
#include "leds/layers/masks/masks.h"
#include "leds/layers/colors/colors.h"

#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
CRGB* leds = new CRGB[NUM_LEDS];

ProcessScheduler scheduler;
Animator* animator;
SequenceScheduler* sequenceScheduler;

class MyProcess : public Process {
  uint8_t buffer[22] = { 8, 200, 1, 18, 17, 26, 15, 242, 2, 12, 10, 8, 255, 1, 0, 255, 1, 0, 255, 1, 16, 100 };

  public:
  String getName() {
    return "My Process";
  }

  void update() {
    pb_istream_t stream = pb_istream_from_buffer(buffer, sizeof(buffer));

    Sequence* sequence = new Sequence();
    SequenceDecoder::decode(&stream, sequence);
    /* printf("Decoded sequence with %d animations\n", sequence->animations.size());
    for (Animation* animation : sequence->animations) {
      printf("Animation with %d layers\n", animation->layers.size());
      for (ILayer* layer : animation->layers) {
        printf("Layer: %s\n", layer->getName().c_str());

        if (layer->getName() == "Section Mask") {
          SectionsMask* sectionsMask = (SectionsMask*)layer;
          printf("Sections: %d\n", sectionsMask->sections.size());
        }

        if (layer->getName() == "Section Color") {
          SectionsColor* sectionsColor = (SectionsColor*)layer;
          printf("Colors: %d\n", sectionsColor->sections.size());
        }
      }
    } */
  }
};

void setup() {
  // put your setup code here, to run once:
  scheduler = ProcessScheduler();
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  animator = new Animator(leds, NUM_LEDS);
  sequenceScheduler = new SequenceScheduler(animator);

  sequenceScheduler->add({
    /** COLORS */
    /* new SingleColor(CRGB::Green), */
    /* new RainbowColor(500, 0), */
    /* new FadeColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 300), */
    /* new SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100), */
    /* new SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100), */
    /* new SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100), */

    /** MASKS */
    /* new BlinkMask({255, 0, 0, 0}, 50), */
    /* new InvertMask(), */
    /* new PulseSawtoothMask(10, 50), */
    /* new PulseMask(10, 50), */
    /* new SawtoothMask(100, 0, 300), */
    /* new SectionsWaveMask({255, 0}, 150), */
    /* new SectionsMask({255, 0, 0, 255, 0}, 50), */
    /* new StarsMask(200, 10, 3), */

    // Gang
    new SingleColor(CRGB::Green),
    new StarsMask(200, 5, 1),

    // Indgang
    /* new RainbowColor(500, 0),
    new SawtoothMask(100, 0, 300),
    new StarsMask(250, 5, 1), */
    /* new WaveMask(100, 100, 50), */
    }
  , 2000);

  MyProcess* myProcess = new MyProcess();

  scheduler.addProcess(sequenceScheduler, 20); // Update every 20ms
  scheduler.addProcess(animator, 20); // Update every 20ms
  // scheduler.addProcess(myProcess, 20); // Update every 500ms
}

void loop() {
  scheduler.update();
  delay(1); // Stability
}