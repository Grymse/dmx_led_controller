#include "dmx.h"

// Global variable definitions
u16_t DMX_START = 3; // 3, 18
u8_t channels[16] = {0};
u8_t prevChannels[16] = {0};

std::vector<u8_t> * to_sections(u8_t value) {
    std::vector<u8_t> * sections = new std::vector<u8_t>();
    sections->push_back(255);
    for (int i = 1; i < value; i++) sections->push_back(0);
    return sections;
}

std::vector<u8_t> * to_full_sections(u8_t value) {
    std::vector<u8_t> * sections = new std::vector<u8_t>();
    
    for (int i = 0; i < value; i++) sections->push_back(255);
    return sections;
}


u16_t valueScaler(u16_t value) {
    if (value < 32) {
        return value;
    } else if (value < 64) {
        return (value - 32) * 2 + 32;
    } else if (value < 128) {
        return (value - 64) * 4 + 96;
    } else if (value < 192) {
        return (value - 128) * 8 + 352;
    } else {
        return (value - 192) * 16 + 864;
    }
}

ILayer * dmx_to_mask(u8_t* channels) {
    switch(channels[0]) {
        case 1: // WaveMask
            return new WaveMask(valueScaler(channels[1]), valueScaler(channels[2]), valueScaler(channels[3]));
        case 2: // StarsMask
            return new StarsMask(valueScaler(channels[1]), channels[2], channels[3]);
        case 3: 
            return new BlinkMask(*to_sections(channels[1]), valueScaler(channels[2]));
        case 4: // SectionsMask
            return new SectionsMask(*to_sections(channels[1]), valueScaler(channels[2]));
        case 5: // PulseSawtoothMask
            return new PulseSawtoothMask(valueScaler(channels[1]), valueScaler(channels[2]));
        case 6: // PulseMask
            return new PulseMask(valueScaler(channels[1]), valueScaler(channels[2]));
        case 7: // SawtoothMask
            return new SawtoothMask(valueScaler(channels[1]), valueScaler(channels[2]), valueScaler(channels[3]));
        case 8: // SectionsWaveMask
            return new SectionsWaveMask(*to_sections(channels[1]), valueScaler(channels[2]));
        case 9: // SectionsRandomMask
            return new SectionsRandomMask(*to_full_sections(channels[1]), valueScaler(channels[2]));
        default:
            return nullptr; // Unknown layer type
    }
}


bool hasChannelsChanged(u8_t* channels, u8_t from, u8_t to) {
    for (int i = from; i <= to; i++) {
        if (channels[i] != prevChannels[i]) {
            return true;
        }
    }
    return false;
}

SingleColor* color = new SingleColor(CRGB::Black);

/**
 * We check whether the 1-7 has changed and only update color if that is the case
 * We check whether the 8-15 has changed and only update masks if that is the case
 * 
 * If a wave animation is running, and only color updates, it makes sense to keep the current position
 * of the wave, so we do not update the masks, as it would reset tick and reset the wave animation.
 * This allows us to smoothly change color without resetting the wave animation.
 * 
 * @param animator The animator object that renders the animation onto the LED-strip
 * @param channels DMX channels to interpret. Assumes channels 1-7 are for color, and channels 8-15 are for masks.
 */
void dmx_to_animation(Animator* animator, u8_t* channels) {
    // If the first 7 channels has changed, update the animator settings.
    if(hasChannelsChanged(channels, 0, 7)) {
        // Set dimmer
        animator->setBrightness(channels[1]);

        // Set color
        color->setColor(CRGB(channels[2], channels[3], channels[4]));

        // If channel 6 is < 128, direction is FORWARD, otherwise BACKWARD
        animator->setDirection(channels[5] < 128 ? Direction::FORWARD : Direction::BACKWARD);
    }

    if(hasChannelsChanged(channels, 8, 15)) {
        ILayer* mask1 = dmx_to_mask(channels + 8); // Use channels 8-11 to decode effect
        ILayer* mask2 = dmx_to_mask(channels + 12); // Use channels 12-15 to decode effect

        if (mask1 != nullptr && mask2 != nullptr) {
            // if wwo masks, combine them
            animator->setLayers({color, mask1, mask2});

        } else if (mask1 != nullptr) {
            // If one mask
            animator->setLayers({color, mask1});

        } else {
            // Only color layer
            animator->setLayers({color});
        }

        // Start at higher tick if DMX channel 6 is set
        // This is used to offset the animation
        animator->setTick(channels[5] % 128);
    }
    memcpy(prevChannels, channels, sizeof(prevChannels));
}

// ReadDMXProcess implementation
ReadDMXProcess::ReadDMXProcess() : Process() {
    this->animator = nullptr;
    DMX::Initialize(input);
}

ReadDMXProcess::ReadDMXProcess(Animator* animator) : Process() {
    this->animator = animator;
    DMX::Initialize(input);
}

String ReadDMXProcess::getName() {
    return "DMX Reader";
}

void ReadDMXProcess::update() {
    // Given the MAX485 often reads faulty data, I've temporarily added a hack, where
    // the DMX controller outputs 123 and 234 on channels 1 and 2, respectively.
    // If these values are not present, we assume the DMX data is invalid,
    // and we do not update the animation.
    if (DMX::Read(1) != 123 || DMX::Read(2) != 234) {
        return;
    }

    // Depending on addresses, we read the DMX data.
    for (int i = 0; i < 15; ++i) {
        channels[i+1] = DMX::Read(i + DMX_START);
    }

    // Map the DMX channels to the animation.
    if (animator != nullptr) {
        dmx_to_animation(animator, channels);
    }
}
