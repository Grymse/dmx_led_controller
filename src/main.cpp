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
#include "leds/generators/generators.h"

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

uint32_t getESP32ChipID() {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i++) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i * 8)) & 0xff) << (i * 8);
  }
  return chipId;
}


class SenderRadioProcess : public Process {
  Radio radio = Radio("1Node", "2Node");
  int counter = 0;

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
    char str[32] = "The current number is:         ";
    counter++;
    snprintf(str + 28, 4, "%04d", counter);
    RadioPayload payload = {(u8_t*)str, 32, 0};
    radio.write(payload);
    printf("Sent: %s\n", str);
  } 
};


class SayMyNameProcess : public Process {
  public:
  String getName() {
    return "Say My Name";
  }

  void update() {
    printf("My name is: %d\n", getESP32ChipID());
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

 //{0x34, 0xB7, 0xDA, 0xF8, 0x9C, 0x10};
// ESPNetwork * reader;


class ProgramController : public Process {
  // ESPNetwork espnow = ESPNetwork(ConnectivityMode::WRITER);
  SequenceGenerator * generator = getTechnoGenerator();
  uint8_t buffer[256];
  
  public:
  String getName() {
    return "Program Generator";
  }

  void update() {
    auto nextSequence = generator->getSequence();
    printf("\n\nGenerated sequence with %d animations\n", nextSequence->animations.size());
    for (Animation* animation : nextSequence->animations) {
      printf("Animation with %d layers\n", animation->layers.size());

      for (ILayer* layer : animation->layers) {
        printf("Layer: %s\n", layer->toString().c_str());
      }
    }
    
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    SequenceEncoder::encode(&ostream, nextSequence);
    // espnow.write({buffer, ostream.bytes_written});

    sequenceScheduler->set(nextSequence);
  } 
};


void onRecv(const uint8_t *receiver_mac_addr, const uint8_t *data, int data_len) {
  pb_istream_t stream = pb_istream_from_buffer(data, data_len);

  Sequence* sequence = new Sequence();
  SequenceDecoder::decode(&stream, sequence);
  sequenceScheduler->set(sequence);
  /* 
  for (Animation* animation : sequence->animations) {
    printf("Animation with %d layers\n", animation->layers.size());
  } */
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
  //  new FadeColor({CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Yellow, CRGB::Fuchsia, CRGB::Aqua}, 300),
    /* new WaveMask(1200, 100, 300),
    new SawtoothMask(300, 150, 500), */

    //Layer: WaveMask: d: 500, l: 600, g: 50
    /* new WaveMask(500, 50, 600), */
  //Layer: SawtoothMask: d: 50, l: 300, g: 300
    /* new SawtoothMask(50, 300, 300), */
  //}
    // COLORS
    // new SingleColor(CRGB::Blue),
    new RainbowColor(500, 0),
    // new FadeColor({CRGB(0,0,255),CRGB(0,255,0) ,CRGB(255,0,0)}, 50),
    // new SectionsWaveColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
    // new SectionsColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
    // new SwitchColor({CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White}, 100),
   
    // MASKS
    // new BlinkMask({255, 0, 0, 0}, 50),
    // new InvertMask(),
    // new PulseSawtoothMask(10, 50),
    new PulseMask(10, 50),
    // new SawtoothMask(100, 0, 300),
    // new SectionsWaveMask({0}, 50),
    // new SectionsMask({255, 0, 0, 255, 0}, 50),
    // new StarsMask(200, 10, 3),

    // Gang
    // new SingleColor(CRGB::Red),
    // new StarsMask(200, 5, 1),

    // Indgang
    /* new RainbowColor(500, 0),
    new SawtoothMask(100, 0, 300),
    new StarsMask(250, 5, 1), */
    /* new WaveMask(100, 100, 50), */
  }
  , 2000);


  // SenderRadioProcess * sender = 
  scheduler.addProcess(sequenceScheduler, 1000 / frames_per_second); // Update every 20ms
  scheduler.addProcess(animator, 1000 / frames_per_second); // Update every 20ms */
  scheduler.addProcess(new SayMyNameProcess(), 1000); // Update every 1000ms
  
  /* if (isReader) {
    reader = new ESPNetwork(ConnectivityMode::READER);
    reader->setOnReceive(onRecv);
  } else {
    scheduler.addProcess(new ProgramController(), 30000); // Update every 1000ms
  } */

 /* if (getESP32ChipID() != 989929244) { // TODO: Get ID of one of the ESP32
    scheduler.addProcess(new ReceiverRadioProcess(), 20); // Update every 20ms
  } else {
    scheduler.addProcess(new SenderRadioProcess(), 100); // Update every 100ms
  } */
}

void loop() {
  scheduler.update();
  delay(1); // Stability
}