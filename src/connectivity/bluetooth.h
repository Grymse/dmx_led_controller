#pragma once
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "../scheduler/scheduler.h"

class BluetoothService : public Process {
public:
    BluetoothService();
    void send(uint8_t* data, size_t length);
    void setOnReceive(BLECharacteristicCallbacks* callbacks);

    String getName() override;
    void update() override;
};
