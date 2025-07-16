#pragma once

#include <Arduino.h>
#include <Preferences.h>

/**
 * @brief BinaryStore provides an interface to save and load binary data 
 *        (uint8_t*) into non-volatile storage (NVS) on the ESP32.
 * 
 * This class uses the Preferences library to store data persistently across reboots.
 * It is ideal for saving small binary blobs such as configuration data, raw bytes,
 * or serialized structs.
 */
class BinaryStore {
private:
    Preferences preferences;   ///< Preferences object used for NVS access
    String key;                ///< Key name under which data is stored
    String namespaceName;      ///< Namespace name used in NVS

public:
    /**
     * @brief Constructor for BinaryStore
     * 
     * @param ns The namespace to use in NVS (default: "storage")
     * @param keyName The key under which binary data will be saved (default: "binaryData")
     */
    BinaryStore(const String& ns = "storage", const String& keyName = "binaryData");

    /**
     * @brief Save binary data to NVS
     * 
     * @param data Pointer to the data buffer to save
     * @param length Length of the data in bytes
     * @return true if the data was saved successfully, false otherwise
     */
    bool saveData(const uint8_t* data, uint32_t length);

    /**
     * @brief Load binary data from NVS
     * 
     * @param buffer Pointer to the buffer where the data will be written
     * @param maxLength Maximum number of bytes to read into the buffer
     * @return Number of bytes actually read from storage
     */
    uint32_t loadData(uint8_t* buffer, uint32_t maxLength);

    /**
     * @brief Clear the stored binary data from NVS
     * 
     * @return true if the key was removed successfully, false otherwise
     */
    bool clearData();

    /**
     * @brief Save default data only if no data is already stored
     * 
     * @param defaultData Pointer to default binary data
     * @param defaultLength Length of the default data
     * @return true if default was saved (or already present), false if save failed
     */
    bool saveDefaultIfEmpty(const uint8_t* defaultData, uint32_t defaultLength);

};


