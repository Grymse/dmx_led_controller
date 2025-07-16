// Libraries
#include <Arduino.h>
#include <FastLED.h>
// Lib
#include <pb_encode.h>
// Local
#include "connectivity/radio.h"
#include "debug.h"
#include "leds/animator.h"
#include "leds/layers/colors/colors.h"
#include "leds/layers/masks/masks.h"
#include "leds/sequence_scheduler.h"
#include "leds/serialization/sequence_decoder.h"
#include "leds/serialization/sequence_encoder.h"
#include "scheduler/scheduler.h"
/* #include "connectivity/espnow.h" */
#include "leds/generators/generators.h"
/* #include "dmx/dmx.h" */
#include "connectivity/serialization/message_decoder.h"
#include "state/binary_store.h"
#include "connectivity/name_generator.h"

#define CE_PIN 0
#define CSN_PIN 10
#define LED_PIN 7
#define NUM_LEDS 300
#define BUILTIN_LED 8
const uint16_t MAX_BUFFER_SIZE = 1028;
CRGB *leds = new CRGB[NUM_LEDS];
RF24 radio = RF24(CE_PIN, CSN_PIN);
u8_t frames_per_second = 40;

ProcessScheduler scheduler;
Animator *animator;
SequenceScheduler *sequenceScheduler;
MessageDecoder* messageDecoder;
BinaryStore store("config", "program");

/**
 * @brief Reads a data packet from Serial based on a length prefix.
 *
 * The protocol expects a 2-byte uint16_t length, followed by 'length' bytes of data.
 *
 * @param dataBuffer A pointer to the buffer where the received data will be stored.
 * This buffer must be pre-allocated by the caller and be at least
 * MAX_BUFFER_SIZE bytes long.
 * @return The number of bytes successfully read into dataBuffer (excluding the 2-byte length prefix),
 * or a negative error code:
 * - -1: No data available to read (timeout or empty buffer).
 * - -2: Not enough data available for the 2-byte length header within timeout.
 * - -3: Announced data length exceeds MAX_BUFFER_SIZE.
 * - -4: Not enough data available for the full payload within timeout after header.
 */
int16_t readSerialPacket(byte* dataBuffer) {
    uint16_t expectedLength = 0;
    size_t bytesRead = 0;

    // Set a timeout for reading. Adjust as needed based on your application's requirements.
    // This is important to prevent the function from blocking indefinitely.
    // For example, if you expect data quickly, set a short timeout.
    // Serial.setTimeout(100); // Example: 100 milliseconds timeout for all read operations in this function

    // --- Read the 2-byte length prefix ---
    if (Serial.available() < 2) {
      return 0;
    }

    // Read the two bytes for the length
    byte lengthBytes[2];
    bytesRead = Serial.readBytes(lengthBytes, 2);

    if (bytesRead != 2) {
        return -2;
    }

    // Reconstruct uint16_t from bytes (assuming little-endian as is common)
    // If your sender is big-endian, swap lengthBytes[0] and lengthBytes[1]
    expectedLength = (uint16_t)lengthBytes[0] | ((uint16_t)lengthBytes[1] << 8);

    printf("to-read: %d\n",expectedLength);

    // --- Validate expected length ---
    if (expectedLength > MAX_BUFFER_SIZE) {

        // Discard the remaining bytes to clear the buffer for the next packet
        while (Serial.available() && expectedLength > 0) {
            Serial.read(); // Read and discard
            expectedLength--;
        }
        return -3; // Length exceeds buffer capacity
    }

    // --- Read the actual data payload ---
    if (expectedLength == 0) {
        return 0; // No payload to read
    }

    // Wait until enough data is available or timeout
    unsigned long startTime = millis();
    while (Serial.available() < expectedLength && (millis() - startTime) < 1000) { // Wait up to 100ms for payload
        delay(1);
    }

    if (Serial.available() < expectedLength) {
        // Discard any partial data that might have arrived
        while (Serial.available()) {
            Serial.read();
        }
        return -4; // Not enough data for payload
    }

    // Read the actual data into the buffer
    bytesRead = Serial.readBytes(dataBuffer, expectedLength);

    if (bytesRead != expectedLength) {
        return -4;
    }

    return bytesRead; // Return the number of bytes successfully read
}

void onReceiveSequence(Sequence *sequence) {
  sequenceScheduler->set(sequence);
}


u8_t buffer[1028];
u32_t buffer_length;

void onReceiveSaveState(Sequence *sequence, protocol_Settings *settings) {
  buffer_length = store.saveData(buffer, buffer_length);
  sequenceScheduler->set(sequence);
}

class ReadFromPC : public Process {
public:
  ReadFromPC() {
    // Constructor can be used to initialize anything if needed
    buffer_length = store.loadData(buffer, sizeof(buffer));

    // Only if contains program
    if (5 < buffer_length) {
      pb_istream_t stream = pb_istream_from_buffer(buffer,buffer_length);
      messageDecoder->decode(&stream);
    }
  }

  void update() override {
      buffer_length = readSerialPacket(buffer);
      if (buffer_length < 1) {
        if (buffer_length != 0) printf("length-return: %d\n",buffer_length);
        return;
      }

      pb_istream_t stream = pb_istream_from_buffer(buffer,buffer_length);
      messageDecoder->decode(&stream);
    }

  String getName() override { return "ReadFromPC"; }
};

void setup() {
  // put your setup code here, to run once:
  scheduler = ProcessScheduler();
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);

  animator = new Animator(leds, NUM_LEDS);
  sequenceScheduler = new SequenceScheduler(animator);
  messageDecoder = new MessageDecoder();

  messageDecoder->setOnSequenceReceived(onReceiveSequence);
  messageDecoder->setOnSaveStateReceived(onReceiveSaveState);

  scheduler.addProcess(animator, 1000 / frames_per_second);
  // scheduler.addProcess(new ReadDMXProcess(animator), 1000 /
  // frames_per_second); // Update every 25ms

  // Set virtual offset for the animator. This is used when multiple LED strips
  // are chained together, and second device needs to act as if it's leds are
  // offset by e.g. 300 leds. If set to 300, then the controller will display
  // leds 300-599
  animator->setVirtualOffset(0);

  scheduler.addProcess(sequenceScheduler, 1000 / frames_per_second);

  sequenceScheduler->add({
    new FadeColor({CRGB(255, 0, 0),CRGB(0, 255, 0),CRGB(0, 0, 255)}, 1200),
    new StarsMask(300, 5, 1),
  }, 10000);
  
  scheduler.addProcess(new ReadFromPC(), 20);

/* 
  const uint8_t defaultProgram[] = { 0xAA, 0xBB, 0xCC, 0xDD };
  store.saveDefaultIfEmpty(defaultProgram, sizeof(defaultProgram)); */
}

void loop() {
  scheduler.update();
  delay(1); // Stability //TODO: Check if this actually introduces stability, or
            // whether we should remove it for more compute time.
}
