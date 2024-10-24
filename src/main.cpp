// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Local
#include "scheduler/scheduler.cpp"
#include "scheduler/process.h"
#include "leds/layer_controller.cpp"
#include "leds/layer_scheduler.cpp"
#include "debug.cpp"
#include "leds/sequence_decoder.cpp"
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
SequenceDecoder* sequenceDecoder;

class MyProcess : public Process {
  uint8_t buffer[79] = { 8, 255, 1, 18, 36, 8, 1, 26, 16, 8, 1, 16, 1, 32, 3, 42, 8, 255, 133, 244, 7, 255, 133, 244, 7, 26, 14, 8, 1, 16, 1, 24, 2, 42, 6, 255, 1, 255, 1, 255, 1, 18, 36, 8, 1, 26, 16, 8, 1, 16, 1, 32, 3, 42, 8, 255, 133, 244, 7, 255, 133, 244, 7, 26, 14, 8, 1, 16, 1, 24, 2, 42, 6, 255, 1, 255, 1, 255, 1 };

  public:
  String getName() {
    return "My Process";
  }

  void update() {
    // create pb_istream_t
    pb_istream_t stream = pb_istream_from_buffer(buffer, sizeof(buffer));

    sequenceDecoder->decode(&stream);
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  layerController = new LayerController(leds, NUM_LEDS);
  layerScheduler = new LayerScheduler(layerController);

  sequenceDecoder = new SequenceDecoder(layerScheduler);


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

  MyProcess* myProcess = new MyProcess();

  scheduler.addProcess(layerScheduler, 20); // Update every 20ms
  scheduler.addProcess(layerController, 20); // Update every 20ms
  scheduler.addProcess(myProcess, 500); // Update every 500ms

  layerController->setDirection(Direction::BACKWARD);

}

void loop() {
  scheduler.update();
  delay(1); // Stability
}