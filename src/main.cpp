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

  scheduler.addProcess(animator, 1000 / frames_per_second); // Update every 20ms */
  scheduler.addProcess(new ReadDMXProcess(animator), 25); // Update every 1000ms

  // Set virtual offset for the animator. This is used when multiple LED strips are
  // chained together, and second device needs to act as if it's leds are offset by
  // e.g. 300 leds. If set to 300, then the controller will display leds 300-599
  animator->setVirtualOffset(0);
}

void loop() {
  scheduler.update();
  delay(1); // Stability
}
