// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Local
#include "scheduler/scheduler.h"
#include "leds/animator.h"
#include "leds/sequence_scheduler.h"
#include "debug.h"
#include "leds/serialization/sequence_decoder.h"
#include "leds/serialization/sequence_encoder.h"
#include "leds/layers/masks/masks.h"
#include "leds/layers/colors/colors.h"
#include <pb_encode.h>
#include "connectivity/radio.h"
#include "connectivity/espnow.h"
#include "leds/randomizers/randomizer.h"

#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
CRGB* leds = new CRGB[NUM_LEDS];
RF24 radio = RF24(CE_PIN, CSN_PIN);

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

/* 
class SenderRadioProcess : public Process {
  Radio radio = Radio("1Node", "2Node");

  public:
  SenderRadioProcess() {
    if (radio.setup(CE_PIN, CSN_PIN)) {
      printf("Radio setup successful\n");
    } else {
      printf("Radio setup failed\n");
    }
  }

  String getName() {
    return "Radio Process";
  }

  void update() {
    char str[128] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. lol";
    RadioPayload payload = {(u8_t*)str, 128, 0};
    radio.write(payload);
    printf("Sent: %s\n", str);
  } 
};


class ReceiverRadioProcess : public Process {
  Radio radio = Radio("2Node", "1Node");

  public:
  ReceiverRadioProcess() {
    if (radio.setup(CE_PIN, CSN_PIN)) {
      printf("Radio setup successful\n");
    } else {
      printf("Radio setup failed\n");
    }
  }

  String getName() {
    return "Radio Process";
  }

  void update() {
    Option<RadioPayload> optional = radio.read();

    if (optional.isEmpty()) {
      printf("x");
      return;
    }

    RadioPayload payload = optional.getValue();
    printf("Received %d: ", payload.length);
    for (int i = 0; i < payload.length; i++) {
      printf("%c", payload.data[i]);
    }
    printf("\n");
  } 
};
 */


uint32_t getESP32ChipID() {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i++) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i * 8)) & 0xff) << (i * 8);
  }
  return chipId;
}


 //{0x34, 0xB7, 0xDA, 0xF8, 0x9C, 0x10};
bool isReader = getESP32ChipID() == 0xDAF89C10;
ESPNetwork * reader;

class ProgramController : public Process {
  ESPNetwork espnow = ESPNetwork(ConnectivityMode::WRITER);
  u8_t program = 0;
  
  public:
  String getName() {
    return "Esp Now Process";
  }

  void update() {
    sequenceScheduler->clear();

    if (program % 2 == 0) {
        sequenceScheduler->add({
          new SingleColor(CRGB::Blue),
        }, 2000);
    } else {
        sequenceScheduler->add({
          new RainbowColor(500, 0),
        }, 2000);
    }

    program++;

    Sequence* sequence = sequenceScheduler->getSequence();
    uint8_t buffer[128];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    SequenceEncoder::encode(&ostream, sequence);
    espnow.write({buffer, ostream.bytes_written});
  } 
};


void onRecv(const uint8_t *receiver_mac_addr, const uint8_t *data, int data_len) {
  printf("received %d\n", data_len);
  pb_istream_t stream = pb_istream_from_buffer(data, data_len);

  Sequence* sequence = new Sequence();
  SequenceDecoder::decode(&stream, sequence);
  sequenceScheduler->set(sequence);
  for (Animation* animation : sequence->animations) {
    printf("Animation with %d layers\n", animation->layers.size());
  }
}

void setup() {
  // put your setup code here, to run once:
  scheduler = ProcessScheduler();
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  animator = new Animator(leds, NUM_LEDS);
  sequenceScheduler = new SequenceScheduler(animator);

  sequenceScheduler->add({
    // COLORS
    new SingleColor(CRGB::Blue),
    /* new RainbowColor(500, 0),
        new FadeColor({CRGB(0,0,255),CRGB(0,255,0) ,CRGB(255,0,0)}, 50),
        new SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
        new SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
        new SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
    */
    // MASKS
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


  // SenderRadioProcess * sender = 
  scheduler.addProcess(sequenceScheduler, 20); // Update every 20ms
  scheduler.addProcess(animator, 20); // Update every 20ms */
  
  if (isReader) {
    reader = new ESPNetwork(ConnectivityMode::READER);
    reader->setOnReceive(onRecv);
  } else {
    scheduler.addProcess(new ProgramController(), 1000); // Update every 1000ms
  }
 /* if (getESP32ChipID() == 0xDAF89C10) {
    scheduler.addProcess(new ReceiverRadioProcess(), 20); // Update every 20ms
  } else {
    scheduler.addProcess(new SenderRadioProcess(), 500); // Update every 500ms
  } */
  /* scheduler.addProcess(myProcess, 20); // Update every 2000ms */

}

void loop() {
  buildLayers();


  scheduler.update();
  delay(1); // Stability
}