#include <HardwareSerial.h>
#define MAX_FIXTURE_LENGTH 19
#define DMX_BAUD_RATE 250000
#define DMX_UNIVERSE_SIZE 512
#define DMX_RX_PIN 4

class DMXReader {
private:
    uint16_t startAddress;
    uint8_t fixtureLength;
    uint8_t channelData[MAX_FIXTURE_LENGTH + 1]; // +1 for 1-indexed array (index 0 unused)
    uint8_t dmxBuffer[DMX_UNIVERSE_SIZE + 1];    // +1 for start code
    HardwareSerial* dmxSerial;
    bool newDataAvailable;
    unsigned long lastUpdateTime;
    
    void updateChannelData() {
        // Copy fixture-specific channels from DMX buffer to channel data array
        // Index 0 remains unused for 1-indexed access
        for (uint8_t i = 1; i <= fixtureLength; i++) {
            if (startAddress + i - 1 <= DMX_UNIVERSE_SIZE) {
                channelData[i] = dmxBuffer[startAddress + i - 1];
            } else {
                channelData[i] = 0;
            }
        }
        newDataAvailable = true;
        lastUpdateTime = millis();
    }
    
    bool readDMXFrame(uint32_t timeout_ms = 0) {
        unsigned long startTime = millis();
        uint16_t bytesRead = 0;
        bool frameSynced = false;
        
        while (bytesRead < DMX_UNIVERSE_SIZE + 1) {
            if (timeout_ms > 0 && (millis() - startTime) > timeout_ms) {
                return false; // Timeout
            }
            
            if (dmxSerial->available()) {
                uint8_t byte = dmxSerial->read();
                
                if (!frameSynced) {
                    // Look for start code (0x00)
                    if (byte == 0x00) {
                        frameSynced = true;
                        dmxBuffer[0] = byte;
                        bytesRead = 1;
                    }
                } else {
                    dmxBuffer[bytesRead] = byte;
                    bytesRead++;
                    
                    // Early breakout when we have all fixture data
                    if (bytesRead >= startAddress + fixtureLength) {
                        updateChannelData();
                        return true;
                    }
                }
            }
        }
        
        if (frameSynced && bytesRead > startAddress + fixtureLength - 1) {
            updateChannelData();
            return true;
        }
        
        return false;
    }
    
public:
    DMXReader(uint16_t address, uint8_t length) : startAddress(address), fixtureLength(length), newDataAvailable(false), lastUpdateTime(0) {
        if (length > MAX_FIXTURE_LENGTH) {
            fixtureLength = MAX_FIXTURE_LENGTH;
        }
        
        // Initialize channel data array
        for (uint8_t i = 0; i <= MAX_FIXTURE_LENGTH; i++) {
            channelData[i] = 0;
        }
        
        // Initialize DMX buffer
        for (uint16_t i = 0; i <= DMX_UNIVERSE_SIZE; i++) {
            dmxBuffer[i] = 0;
        }
    }

    ~DMXReader() {
        dmxSerial->end(); // Clean up serial port
    }

    void init(HardwareSerial * serial) {
        dmxSerial = serial;
        dmxSerial->begin(DMX_BAUD_RATE, SERIAL_8N1, DMX_RX_PIN, -1);
    }
    
    // Non-blocking method - returns true if new data is available
    bool hasNewData() {
        if (dmxSerial->available()) {
            return readDMXFrame(0);
        }
        return newDataAvailable;
    }
    
    // Blocking method - waits for new DMX data with timeout
    bool read(uint32_t timeout_ms = 25) {
        newDataAvailable = false;
        return readDMXFrame(timeout_ms);
    }
    
    // Get the channel data array (1-indexed, index 0 is unused)
    uint8_t* getChannelData() {
        newDataAvailable = false; // Mark as read
        return channelData;
    }
    
    // Get individual channel value (1-indexed)
    uint8_t getChannel(uint8_t channel) {
        if (channel >= 1 && channel <= fixtureLength) {
            return channelData[channel];
        }
        return 0;
    }
    
    // Get fixture configuration
    uint16_t getStartAddress() const { return startAddress; }
    uint8_t getFixtureLength() const { return fixtureLength; }
    unsigned long getLastUpdateTime() const { return lastUpdateTime; }
};