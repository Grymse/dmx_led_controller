#pragma once

#include <string>
#include <array>
#include <stdint.h>

#ifdef ESP_PLATFORM
#include "esp_system.h"
#endif

/**
 * @brief Generates a cute, deterministic 5-character name in the format CVCVC
 *        (Consonant-Vowel-Consonant-Vowel-Consonant) using the device's MAC address.
 * 
 * The generated name is designed to be readable and "cute", using only soft-sounding
 * consonants (e.g., B, M, L, P) and light vowels (A, E, I).
 * 
 * Example output: "LUMBI", "MELIN", "BEMIL"
 * 
 * @return std::string - A 5-character name based on the MAC address.
 */
static std::string getCuteName() {
    // Cute/soft consonants and vowels
    const char softConsonants[] = "BDLMNPRSTY"; // 10
    const char cuteVowels[]     = "AEI";        // 3

    // Get MAC address from ESP32
    std::array<uint8_t, 6> mac = {0};
#ifdef ESP_PLATFORM
    esp_efuse_mac_get_default(mac.data()); // ESP32 built-in MAC
#else
    // Fallback MAC for desktop/testing
    mac = {0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x34};
#endif

    // Simple deterministic hash
    uint32_t hash = 0;
    for (uint8_t b : mac) {
        hash = (hash * 31) ^ b;
    }

    // Generate name in CVCVC format
    std::string name;
    name += softConsonants[hash % 10]; hash /= 10;
    name += cuteVowels[hash % 3];      hash /= 3;
    name += softConsonants[hash % 10]; hash /= 10;
    name += cuteVowels[hash % 3];      hash /= 3;
    name += softConsonants[hash % 10]; hash /= 10;

    return name;
}