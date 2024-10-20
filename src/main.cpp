// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Local
#include "scheduler/scheduler.cpp"
#include "scheduler/process.h"
#include <leds/layer_controller.cpp>
#include <leds/layer_scheduler.cpp>
#include "debug.cpp"
// Masks
#include <leds/layers/masks/blink.cpp>
#include <leds/layers/masks/invert.cpp>
#include <leds/layers/masks/pulse.cpp>
#include <leds/layers/masks/pulse_sawtooth.cpp>
#include <leds/layers/masks/sawtooth.cpp>
#include <leds/layers/masks/sections.cpp>
#include <leds/layers/masks/sections_wave.cpp>
#include <leds/layers/masks/stars.cpp>
#include <leds/layers/masks/wave.cpp>
// Colors
#include <leds/layers/colors/fade.cpp>
#include <leds/layers/colors/rainbow.cpp>
#include <leds/layers/colors/sections.cpp>
#include <leds/layers/colors/sections_wave.cpp>
#include <leds/layers/colors/single.cpp>
#include <leds/layers/colors/switch.cpp>


#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
CRGB* leds = new CRGB[NUM_LEDS];

ProcessScheduler scheduler = ProcessScheduler();
LayerController* layerController;
LayerScheduler* layerScheduler;

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
    /** COLORS */
    /* new SingleColor(CRGB::Red), */
    /* new FadeColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 300), */
    new RainbowColor(500, 0),
    new RainbowColor(500, 0),
    new RainbowColor(500, 0),
    new RainbowColor(500, 0),
    new RainbowColor(500, 0),
    /* new SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100), */
    /* new SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100), */
    /* new SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100), */

    /** MASKS */
    /* new BlinkMask({255, 0, 0, 0}, 50), */
    /* new InvertMask(), */
    /* new PulseSawtoothMask(10, 50), */
    /* new PulseMask(10, 50), */
    /* new SawtoothMask(100, 100, 50), */
    /* new SectionsWaveMask({255, 0}, 150), */
    /* new SectionsMask({255, 0, 0, 255, 0}, 50), */
    /* new StarsMask(200, 10, 3), */
    /* new WaveMask(100, 100, 50), */
    }
  , 2000);

  scheduler.addProcess(layerScheduler, 20); // Update every 20ms
  scheduler.addProcess(layerController, 20); // Update every 20ms

  layerController->setDirection(Direction::BACKWARD);

}

void loop() {
  scheduler.update();
  delay(1); // Stability
}