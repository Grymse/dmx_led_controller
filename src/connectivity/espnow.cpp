#include "espnow.h"

// Address for broadcasting
uint8_t global_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/**
 * @brief Construct a new ESPNow object
 * 
 * @param address Must be of length 5
 * @param payload Allows for a custom payload to be set. This is useful for setting the data array to a custom size.
 */
ESPNetwork::ESPNetwork(ConnectivityMode mode) {
    this->mode = mode;
    this->mac = global_mac;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    while (ESPNow.init() != ESP_OK) {
        printf("ESPNow init failed\n");
        delay(200);
    }
    if (mode == ConnectivityMode::READER) {
        ESPNow.set_mac(mac);
    } else {
        ESPNow.add_peer(mac);
    }
}

bool ESPNetwork::write(Payload payload) {
    return ESPNow.send_message(mac, payload.data, payload.length);
}

void ESPNetwork::setOnReceive(esp_now_recv_cb_t callback) {
    ESPNow.reg_recv_cb(callback);
}