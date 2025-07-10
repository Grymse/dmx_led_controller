#ifndef DMX_H
#define DMX_H

#include <vector>
#include <Arduino.h>
#include "../scheduler/scheduler.h"
#include "../leds/animator.h"
#include "../leds/layers/layer.h"
#include "../leds/layers/colors/colors.h"
#include "../leds/layers/masks/masks.h"
#include "dmx_lib.h"

// Type definitions
typedef uint8_t u8_t;
typedef uint16_t u16_t;

// Global variables
extern u16_t DMX_START;
extern u8_t channels[16];
extern u8_t prevChannels[16];

// Function declarations
std::vector<u8_t>* to_sections(u8_t value);
std::vector<u8_t>* to_full_sections(u8_t value);
u16_t valueScaler(u16_t value);
ILayer* dmx_to_mask(u8_t* channels);
bool hasChannelsChanged(u8_t* channels, u8_t from, u8_t to);
void dmx_to_animation(Animator* animator, u8_t* channels);

// Process class for reading DMX data
class ReadDMXProcess : public Process {
private:
    Animator* animator;

public:
    ReadDMXProcess(); // Default constructor
    ReadDMXProcess(Animator* animator); // Parameterized constructor
    String getName() override;
    void update() override;
};

#endif // DMX_H