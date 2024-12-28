#pragma once

#include <RF24.h>
#include <optional.h>

struct RadioPayload {
    u8_t* data;
    u16_t length;
    long receivedAt;
};

enum class RadioMode {
    NOT_INITIALIZED,
    READER,
    WRITER,
};

class Radio {
    RF24 radio;
    u8_t writerAddress[6];
    u8_t readerAddress[6];
    RadioPayload payload;
    RadioMode mode = RadioMode::NOT_INITIALIZED;

    public:
    Radio(const char* writer, const char* reader, RadioPayload payload = {new u8_t[256], 0, 0});
    bool setup(u8_t CE_PIN, u8_t CSN_PIN);
    String toString();
    bool write(RadioPayload payload);
    Option<RadioPayload> read();
};