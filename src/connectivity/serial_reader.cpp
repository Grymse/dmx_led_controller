#include <Arduino.h>
#include "../scheduler/scheduler.h"

#include <Arduino.h>
#include <HardwareSerial.h>

// Alternative implementation using Hardware Serial
// This is useful if you want to use GPIO pins instead of USB Serial/JTAG

class SerialProtocol : public Process {
private:
    HardwareSerial* serial;
    uint8_t* readBuffer;
    size_t bufferSize;
    size_t bufferPos;
    uint16_t expectedLength;
    uint8_t headerBytesRead;
    bool headerComplete;
    
    void (*dataCallback)(const uint8_t* data, size_t length);
    
    static const size_t MAX_BUFFER_SIZE = 1024;
    
public:
    SerialProtocol(HardwareSerial* serialPort) : 
        serial(serialPort),
        readBuffer(nullptr),
        bufferSize(MAX_BUFFER_SIZE),
        bufferPos(0),
        expectedLength(0),
        headerBytesRead(0),
        headerComplete(false),
        dataCallback(nullptr) {
        
        readBuffer = (uint8_t*)malloc(bufferSize);
        if (!readBuffer) {
            serial->println("Failed to allocate read buffer");
        }
    }
    
    ~SerialProtocol() {
        if (readBuffer) {
            free(readBuffer);
        }
    }
    
    // For ESP32-C3 using custom pins (if not using USB Serial/JTAG)
    void begin(unsigned long baudRate = 115200, int8_t rxPin = -1, int8_t txPin = -1) {
        if (rxPin != -1 && txPin != -1) {
            // Use custom pins for Hardware Serial
            serial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
        } else {
            // Use default pins or USB Serial/JTAG
            serial->begin(baudRate);
        }
        
        // Wait for serial connection
        unsigned long startTime = millis();
        while (!(*serial) && (millis() - startTime < 5000)) {
            delay(100);
        }
        
        delay(100);
        serial->println("ESP32-C3 Hardware Serial Protocol initialized");
        serial->flush();
    }
    
    void sendBytes(const uint8_t* data, size_t length) {
        if (length > 65535) {
            serial->println("Error: Data length exceeds maximum (65535 bytes)");
            return;
        }
        
        // Send length header (2 bytes, little-endian)
        serial->write((uint8_t)(length & 0xFF));
        serial->write((uint8_t)((length >> 8) & 0xFF));
        
        // Send data
        serial->write(data, length);
        serial->flush();
    }
    
    void sendBytes(const String& str) {
        sendBytes((const uint8_t*)str.c_str(), str.length());
    }
    
    void sendBytes(const char* str) {
        sendBytes((const uint8_t*)str, strlen(str));
    }
    
    void onData(void (*callback)(const uint8_t* data, size_t length)) {
        dataCallback = callback;
    }
    
    void update() override {
        while (serial->available() > 0) {
            uint8_t byte = serial->read();
            processIncomingByte(byte);
        }
    }

    String getName() override {
        return "Serial Protocol";
    }
    
private:
    void processIncomingByte(uint8_t byte) {
        if (!readBuffer) return;
        
        if (!headerComplete) {
            if (headerBytesRead == 0) {
                expectedLength = byte;
                headerBytesRead = 1;
            } else if (headerBytesRead == 1) {
                expectedLength |= (byte << 8);
                headerBytesRead = 2;
                headerComplete = true;
                bufferPos = 0;
                
                if (expectedLength > bufferSize) {
                    serial->print("Error: Expected length (");
                    serial->print(expectedLength);
                    serial->print(") exceeds buffer size (");
                    serial->print(bufferSize);
                    serial->println(")");
                    resetPacketState();
                    return;
                }
            }
        } else {
            if (bufferPos < expectedLength && bufferPos < bufferSize) {
                readBuffer[bufferPos++] = byte;
                
                if (bufferPos >= expectedLength) {
                    if (dataCallback) {
                        dataCallback(readBuffer, expectedLength);
                    }
                    resetPacketState();
                }
            } else {
                serial->println("Error: Buffer overflow or unexpected data");
                resetPacketState();
            }
        }
    }
    
    void resetPacketState() {
        headerComplete = false;
        headerBytesRead = 0;
        expectedLength = 0;
        bufferPos = 0;
    }
};