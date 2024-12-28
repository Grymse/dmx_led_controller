#include "radio.h"

// TODO: WORK IN PROGRESS
// This is a work in progress. The current implementation is not working as expected.
// Only sends 32 bytes of data at a time. This is not sufficient for the protocol
// The implementation should support larger messages and handle the splitting of the message into smaller parts.
// Implement a protocol for sending these larger messages.

/**
 * @brief Construct a new Radio object
 * 
 * @param address Must be of length 5
 * @param payload Allows for a custom payload to be set. This is useful for setting the data array to a custom size.
 */
Radio::Radio(const char* writer, const char* reader, RadioPayload payload) {
    this->payload = payload;

    if (strlen(writer) != 5 || strlen(reader) != 5) {
        printf("Address must be of length 5\n");
        return;
    }

    strncpy((char*)this->writerAddress, writer, 5); // Move the address to the address array
    strncpy((char*)this->readerAddress, reader, 5); // Move the address to the address array

    this->writerAddress[5] = '\0';
    this->readerAddress[5] = '\0';
}

bool Radio::setup(u8_t CE_PIN, u8_t CSN_PIN) {
    radio = RF24(CE_PIN, CSN_PIN);

    while (!radio.begin())
    {
        printf("radio hardware is not responding!!\n");
        delay(200);
    }

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to each other.
    // radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

    radio.setPayloadSize(32); // Here we are setting the size of data to be received as the size of data array.
    
    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(writerAddress);  // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, readerAddress);  // using pipe 1

    // radio.setAutoAck(false);
    // radio.setDataRate(RF24_250KBPS); //estimated max is only 64kbps
    /* radio.startListening();
    mode = RadioMode::READER; */
    
    return true;
}

String Radio::toString() {
        String result = "Radio Status:\n";
        result += "Millis since last receive: " + String(payload.receivedAt) + "\n";
        result += "Package size: " + String(payload.length) + "\n";
        result += "Data: ";
        for (int i = 0; i < payload.length; i++) {
            result += String(payload.data[i]) + " ";
        }
        result += "\nEnd\n";
        return result;
}

bool Radio::write(RadioPayload payload) {
    if (mode != RadioMode::WRITER) {
        radio.stopListening();
        mode = RadioMode::WRITER;
    }
    // copy the data into the local payload
/*     size_t block_index = 0;
    while(block_index < payload.length) {
        u8_t end_of_block_index = block_index + 31;
        u8_t end_of_block = payload.data[end_of_block_index];

        payload.data[end_of_block_index] = payload.length - block_index;
        radio.write(payload.data + block_index, 32);
        
        payload.data[end_of_block_index] = end_of_block;
        block_index += 31;
    } */

    return radio.write(payload.data, payload.length);
}

Option<RadioPayload> Radio::read() {
    if (mode != RadioMode::READER) {
        radio.startListening();
        mode = RadioMode::READER;
    }
    payload.length = 0;
    uint8_t pipe = 0;
    while (radio.available(&pipe))
    {
        radio.read(payload.data + payload.length, 32);
        payload.length += 32;
    }
    payload.receivedAt = millis();

    if (payload.length > 0) {
        return Option<RadioPayload>(payload);
    }
    return Option<RadioPayload>().empty();
}