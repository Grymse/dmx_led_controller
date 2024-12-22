// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Local
#include "scheduler/scheduler.h"
#include "leds/animator.h"
#include "leds/sequence_scheduler.h"
#include "debug.h"
#include "leds/sequence_decoder.h"
#include "leds/sequence_encoder.h"
#include "leds/layers/masks/masks.h"
#include "leds/layers/colors/colors.h"
#include <pb_encode.h>

#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
CRGB* leds = new CRGB[NUM_LEDS];

ProcessScheduler scheduler;
Animator* animator;
SequenceScheduler* sequenceScheduler;
/* 
class MyProcess : public Process {
  uint8_t buffer[512];
  public:
  String getName() {
    return "My Process";
  }

  void update() {
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    SequenceEncoder::encode(&ostream, sequenceScheduler->getSequence());

    // Reset the stream for reading
    pb_istream_t stream = pb_istream_from_buffer(buffer, ostream.bytes_written);

    Sequence* sequence = new Sequence();
    SequenceDecoder::decode(&stream, sequence);
    sequenceScheduler->set(sequence);
  } 
};
 */
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
    new SingleColor(CRGB::Blue),
    /* new RainbowColor(500, 0),
    new FadeColor({CRGB(0,0,255),CRGB(0,255,0) ,CRGB(255,0,0)}, 50),
    new SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
    new SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
    new SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
 */
    /** MASKS */
    /* new BlinkMask({255, 0, 0, 0}, 50),
    new InvertMask(),
    new PulseSawtoothMask(10, 50),
    new PulseMask(10, 50),
    new SawtoothMask(100, 0, 300),
    new SectionsWaveMask({0}, 50),
    new SectionsMask({255, 0, 0, 255, 0}, 50),
    new StarsMask(200, 10, 3), */

    // Gang
    /* new SingleColor(CRGB::Red),
    new StarsMask(200, 5, 1), */

    // Indgang
    /* new RainbowColor(500, 0),
    new SawtoothMask(100, 0, 300),
    new StarsMask(250, 5, 1), */
    /* new WaveMask(100, 100, 50), */
    }
  , 2000);

  // 8, 255, 1, 18, 39, 16, 208, 15, 26, 27, 8, 55, 16, 150, 1, 74, 20, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 26, 5, 32, 128, 128, 252, 7
  // 8, 255, 1, 18, 39, 16, 208, 15, 26, 27, 8, 55, 16, 150, 1, 74, 20, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 255, 1, 26, 5, 32, 128, 128, 252, 7

  scheduler.addProcess(sequenceScheduler, 20); // Update every 20ms
  scheduler.addProcess(animator, 20); // Update every 20ms */
  /* scheduler.addProcess(myProcess, 20); // Update every 2000ms */
}

void loop() {
  scheduler.update();
  delay(1); // Stability
}