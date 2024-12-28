#pragma once

#include <optional.h>
#include <Arduino.h>
#include <WiFi.h>
#include "ESPNowW.h"

struct Payload {
    u8_t* data;
    u32_t length;
};

enum class ConnectivityMode {
    NOT_INITIALIZED,
    READER,
    WRITER,
};

class ESPNetwork {
    uint8_t* mac;
    ConnectivityMode mode = ConnectivityMode::NOT_INITIALIZED;
    
    void (*onReceiveCallback)(Payload*) = nullptr;

    public:
    ESPNetwork(ConnectivityMode mode);
    bool write(Payload payload);
    void setOnReceive(esp_now_recv_cb_t callback);
};