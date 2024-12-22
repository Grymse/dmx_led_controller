#pragma once

#include <RF24.h>
#include <optional.h>

struct RadioPayload {
    u8_t* data;
    u8_t length;
    long receivedAt;
};

class Radio {
    RF24 radio;
    u8_t address[6];
    RadioPayload payload;

    public:
    Radio(const char* address, RadioPayload payload = {new u8_t[512], 0, 0});
    bool setup(u8_t CE_PIN, u8_t CSN_PIN);
    String toString();
    bool send(RadioPayload payload);
    Option<RadioPayload> receive();
};