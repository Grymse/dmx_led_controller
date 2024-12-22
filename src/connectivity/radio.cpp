#include "radio.h"

/**
 * @brief Construct a new Radio:: Radio object
 * 
 * @param address Must be of length 5
 * @param payload Allows for a custom payload to be set. This is useful for setting the data array to a custom size.
 */
Radio::Radio(const char* address, RadioPayload payload) {
    this->payload = payload;
    strncpy((char*)this->address, address, 5); // Move the address to the address array
}

bool Radio::setup(u8_t CE_PIN, u8_t CSN_PIN) {
    radio = RF24(CE_PIN, CSN_PIN);

    if (!radio.begin())
    {
        printf("radio hardware is not responding!!\n");
        return false;
    }

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to each other.
    // radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

    radio.setPayloadSize(32); // Here we are setting the size of data to be received as the size of data array.

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address); // using pipe 1

    // radio.setAutoAck(false);
    // radio.setDataRate(RF24_250KBPS); //estimated max is only 64kbps
    radio.startListening();
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

bool Radio::send(RadioPayload payload) {
    return radio.write(payload.data, payload.length);
}

Option<RadioPayload> Radio::receive() {
    if (radio.available())
    {
        radio.read(payload.data, radio.getPayloadSize());
        payload.length = radio.getPayloadSize();
        return Option<RadioPayload>(payload);
    }
    return Option<RadioPayload>().empty();
}