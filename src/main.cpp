#include <HardwareSerial.h>
#include <stdio.h>

// DMX Configuration
#define DMX_CHANNELS 512
#define DMX_BREAK_THRESHOLD 88  // microseconds (DMX break is min 88us)
#define DMX_TIMEOUT 1000        // milliseconds to timeout if no data

// Fixture Configuration - MODIFY THESE VALUESt
const int FIXTURE_START_CHANNEL = 1;    // Starting DMX channel (1-512)
const int FIXTURE_CHANNEL_COUNT = 4;    // Number of channels for your fixture

// DMX data storage
uint8_t dmxData[DMX_CHANNELS + 1];  // +1 for start code
bool dmxFrameComplete = false;
unsigned long lastByteTime = 0;
unsigned long frameStartTime = 0;
int dmxChannelCount = 0;

enum DMXState {
  DMX_IDLE,
  DMX_BREAK,
  DMX_STARTCODE,
  DMX_DATA
};

DMXState dmxState = DMX_IDLE;

void readDMX() {
  unsigned long currentTime = micros();
  
  // State machine for DMX reception
  switch (dmxState) {
    case DMX_IDLE:
      // Wait for a break (no data for break threshold)
      if (Serial1.available() == 0) {
        if ((currentTime - lastByteTime) > DMX_BREAK_THRESHOLD) {
          dmxState = DMX_BREAK;
          dmxChannelCount = 0;
        }
      } else {
        // Clear any garbage data
        while (Serial1.available()) {
          Serial1.read();
          lastByteTime = currentTime;
        }
      }
      break;
      
    case DMX_BREAK:
      // Break detected, wait for start code
      if (Serial1.available()) {
        uint8_t startCode = Serial1.read();
        lastByteTime = micros();
        dmxData[0] = startCode;
        dmxChannelCount = 1;
        dmxState = DMX_DATA;
        frameStartTime = millis();
      }
      break;
      
    case DMX_DATA:
      // Read data bytes
      while (Serial1.available() && dmxChannelCount <= DMX_CHANNELS) {
        uint8_t dataByte = Serial1.read();
        lastByteTime = micros();
        
        dmxData[dmxChannelCount] = dataByte;
        dmxChannelCount++;
        
        // Check if frame is complete
        if (dmxChannelCount > DMX_CHANNELS) {
          dmxFrameComplete = true;
          dmxState = DMX_IDLE;
          break;
        }
      }
      
      // Check for incomplete frame timeout
      if ((millis() - frameStartTime) > 50) { // 50ms timeout for frame
        dmxState = DMX_IDLE;
        dmxChannelCount = 0;
      }
      break;
  }
}

uint8_t* readFixture(int startChannel, int channelCount) {
  // Static array to return fixture data
  static uint8_t fixtureData[32];  // Max 32 channels per fixture
  
  // Validate inputs
  if (startChannel < 1 || startChannel > DMX_CHANNELS || 
      channelCount < 1 || channelCount > 32 ||
      (startChannel + channelCount - 1) > DMX_CHANNELS) {
    // Return zeros if invalid
    memset(fixtureData, 0, channelCount);
    return fixtureData;
  }
  
  // Copy fixture channels from DMX data
  // DMX channels are 1-indexed, but our array is 0-indexed
  // dmxData[0] = start code, dmxData[1] = channel 1, etc.
  for (int i = 0; i < channelCount; i++) {
    fixtureData[i] = dmxData[startChannel + i];
  }
  
  return fixtureData;
}


void setup() {
  // Initialize USB Serial for debugging
  Serial.begin(115200);
  
  // Initialize Serial1 for DMX communication
  Serial1.begin(250000, SERIAL_8N2, 4, -1);
  
  printf("DMX Fixture Reader Starting...\n");
  printf("Reading fixture with %d channels starting at channel %d\n", 
         FIXTURE_CHANNEL_COUNT, FIXTURE_START_CHANNEL);
  
  // Initialize DMX data array
  memset(dmxData, 0, sizeof(dmxData));
  lastByteTime = micros();
}

void loop() {
  // Read DMX data with timing measurement
  unsigned long startTime = micros();
  readDMX();
  unsigned long endTime = micros();
  unsigned long executionTime = endTime - startTime;
  
  // Print individual call times that are unusually long
  if (executionTime > 100) {  // Print if longer than 100 microseconds
    Serial.printf("readDMX took: %lu microseconds\n", executionTime);
  }
  
  // Also track average timing
  static int callCount = 0;
  static unsigned long totalTime = 0;
  callCount++;
  totalTime += executionTime;
  
  if (callCount%20 == 0) {
    Serial.printf("readDMX avg: %lu us over %d calls\n", 
                  totalTime / callCount, callCount);
    callCount = 0;
    totalTime = 0;
  }

  // If we have a complete frame, read the fixture data
  if (dmxFrameComplete) {
    uint8_t* fixtureData = readFixture(FIXTURE_START_CHANNEL, FIXTURE_CHANNEL_COUNT);
    
    // Print fixture data
    for (int i = 0; i < FIXTURE_CHANNEL_COUNT; i++) {
      printf("%d", fixtureData[i]);
      if (i < FIXTURE_CHANNEL_COUNT - 1) {
        printf(" ");
      }
    }
    printf("\n");
    
    dmxFrameComplete = false;  // Reset flag
  }
  
  // Timeout check - reset if no data received
  if (millis() - (lastByteTime / 1000) > DMX_TIMEOUT) {
    dmxState = DMX_IDLE;
    dmxChannelCount = 0;
  }
}
