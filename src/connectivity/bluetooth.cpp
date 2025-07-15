#include "bluetooth.h"
#include "debug.h"

// UUIDs for service and characteristics
#define SERVICE_UUID        "b2085082-e29b-4d4e-b868-f782458dec9a"
#define CHARACTERISTIC_UUID_RX "b2085082-e29b-4d4e-b868-f782458dec9b"
#define CHARACTERISTIC_UUID_TX "b2085082-e29b-4d4e-b868-f782458dec9c"

BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic;
BLECharacteristic* pRxCharacteristic;
enum BluetoothState {
  UNINITIALIZED,
  ADVERTISING,
  CONNECTED,
  DISCONNECTED,
};

BluetoothState bluetoothServiceState = BluetoothState::UNINITIALIZED;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      bluetoothServiceState = BluetoothState::CONNECTED;
      debug("Device connected\n",0);
    };

    void onDisconnect(BLEServer* pServer) {
      bluetoothServiceState = BluetoothState::DISCONNECTED;
      debug("Device disconnected\n",0);
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      printf("%s\n",rxValue.c_str());
      printf("%i\n",rxValue.length());
    }
};

BluetoothService::BluetoothService() {
  // Create the BLE Device
  BLEDevice::init("ESP32-C3-BLE");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  pRxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_RX,
                    BLECharacteristic::PROPERTY_WRITE
                  );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Set MTU size for larger payloads
  BLEDevice::setMTU(517);

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  bluetoothServiceState = BluetoothState::ADVERTISING;
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  
  BLEDevice::startAdvertising();
  printf("Waiting for a client connection to notify...\n");
};

void BluetoothService::update() {
  // Handle disconnection
  if (bluetoothServiceState == BluetoothState::DISCONNECTED) {
    pServer->startAdvertising(); // Restart advertising
    bluetoothServiceState == BluetoothState::ADVERTISING;
    printf("Start advertising\n");
  }
}

void BluetoothService::send(uint8_t* data, size_t length) {
  printf("Sending data: ");
  for (size_t i = 0; i < length; i++) {
    printf("%02x ", data[i]);
  }
  printf("\n");
  printf("state: %d\n", bluetoothServiceState);
    if (bluetoothServiceState == BluetoothState::CONNECTED) {
        pTxCharacteristic->setValue(data, length);
        pTxCharacteristic->notify();
    }
}

void BluetoothService::setOnReceive(BLECharacteristicCallbacks* callbacks) {
  pRxCharacteristic->setCallbacks(callbacks);
}

String BluetoothService::getName() {
  return "Bluetooth Service";
}
