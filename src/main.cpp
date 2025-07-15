// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Lib
#include <pb_encode.h>
// Local
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
#include "dmx/dmx.h"
#include "connectivity/serialization/message_decoder.h"
#include "state/binary_store.h"

#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
CRGB* leds = new CRGB[NUM_LEDS];
RF24 radio = RF24(CE_PIN, CSN_PIN);
u8_t frames_per_second = 40;

ProcessScheduler scheduler;
Animator* animator;
SequenceScheduler* sequenceScheduler;

void setup() {
  // put your setup code here, to run once:
  scheduler = ProcessScheduler();
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);

  animator = new Animator(leds, NUM_LEDS);
  sequenceScheduler = new SequenceScheduler(animator);

  scheduler.addProcess(animator, 1000 / frames_per_second);
  //scheduler.addProcess(new ReadDMXProcess(animator), 1000 / frames_per_second); // Update every 25ms

  // Set virtual offset for the animator. This is used when multiple LED strips are
  // chained together, and second device needs to act as if it's leds are offset by
  // e.g. 300 leds. If set to 300, then the controller will display leds 300-599
  animator->setVirtualOffset(0);

  
  /**
   * @brief Scheduler is commented out for DMX controller.
   * As the DMX controller directly controls the current layers, there is no need for a sequence scheduler.
   * The sequence scheduler is used for animations that are not directly controlled by DMX. Depending on
   * how we set it up, we need to setup a mechanism that safely switches between these two modes.
   */

  scheduler.addProcess(sequenceScheduler, 1000 / frames_per_second);
/* 
  sequenceScheduler->add({
    // COLORS
    new SingleColor(CRGB::Green),
    // new SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
    // new SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
    // new SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
   
    // MASKS
    //new BlinkMask({255, 0, 0, 0}, 50),
    // new InvertMask(),
    // new PulseSawtoothMask(10, 50),
    new SawtoothMask(100, 0, 300),
    // new SectionsWaveMask({0}, 50),
    // new SectionsMask({255, 0, 0, 255, 0}, 50),
    // new StarsMask(200, 10, 3),
  }, 10000); */

  sequenceScheduler->add({
    new FadeColor({CRGB(255, 0, 0),CRGB(255, 255, 255)}, 1200),
    new StarsMask(300, 5, 1),
  }, 10000);
}

void loop() {
  scheduler.update();
  delay(1); // Stability //TODO: Check if this actually introduces stability, or whether we should remove it for more compute time.
}
