#include <HardwareSerial.h>
#include <stdio.h>

// DMX Configuration
#define MARK_BEFORE_BREAK 1000 // microseconds to wait before break detection
#define MARK_AFTER_BREAK 12  // microseconds to wait after break detection
#define MARK_BETWEEN_SLOTS 4 // Max bytes to read per loop iteration
#define DMX_BREAK 176



void setup() {
  // Initialize Serial1 for DMX communication
  Serial1.begin(250000, SERIAL_8N2, 4, -1);
}

uint8_t * readDMX(int startChannel, int channelCount) {
    // Static array to return fixture data
    static uint8_t fixtureData[16];  // Max 16 channels per fixture

    while(Serial1.available()) {
        Serial1.read();
    }
    
    unsigned long lastByteTime = micros();
    // Wait for a break (no data for break threshold)
    while (Serial1.available() == 0) {
        if ((micros() - lastByteTime) > DMX_BREAK) {
            break;
        }
    }
    
    delayMicroseconds(MARK_AFTER_BREAK); // Wait after break
    while(Serial1.available() == 0) {
        // Wait for the first byte after break
    }
    Serial1.read();


    int bytesRead = 0;
    while (bytesRead <= channelCount) {
        if (Serial1.available()) {
            fixtureData[bytesRead] = Serial1.read();
            bytesRead++;
        }
    }

    return fixtureData;
}


void loop() {
    
    // Read DMX data
    uint8_t *fixtureData = readDMX(1, 15); // Read channels 1 to 32
    
    // Print fixture data
    printf("Fixture Data: ");
    for (int i = 0; i <= 20; i++) {
        printf("%d ", fixtureData[i]);
    }
    printf("\n");
}