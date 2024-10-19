#include "scheduler/scheduler.cpp"
#include "scheduler/process.h"
#include <Arduino.h>
#include <leds/layer_controller.cpp>
#include <leds/layer_scheduler.cpp>
#include <leds/layers/masks/sections.cpp>
#include <leds/layers/masks/invert.cpp>
#include <leds/layers/masks/wave.cpp>
#include <leds/layers/colors/single.cpp>
#include <FastLED.h>
#include "debug.cpp"


#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
CRGB *leds = new CRGB[NUM_LEDS];

Scheduler scheduler = Scheduler();
LayerController *layerController;
LayerScheduler *layerScheduler;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  layerController = new LayerController(leds, NUM_LEDS);
  layerScheduler = new LayerScheduler(layerController);


  /* layerScheduler->add({
    new SingleColor(CRGB::Blue),
    new WaveMask(20, 2000),
    }
  ,2000); */

  layerScheduler->add({
    new SingleColor(CRGB::Blue),
    /* new SectionsMask({true, false, true, false, true, false, true, false, true, false}, 10),
    new SectionsMask({true, false, false}, 40), */
    /* new WaveMask(25, 275, 200), */
    }
  ,2000);


  /* layerScheduler->add({
    new SingleColor(CRGB::Red)
    }
  ,25); */

  scheduler.addProcess(layerScheduler, 20); // Update every 20ms
  scheduler.addProcess(layerController, 20); // Update every 20ms
  
}

void loop() {
  scheduler.run();
  delay(1); // Stability
}