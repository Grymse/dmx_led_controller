#include "dmx.h"

// Global variable definitions
u16_t DMX_START = 3; // 3, 18
u8_t channels[16] = {0};
u8_t prevChannels[16] = {0};

// NOTE: The DMX protocol expects 123, 234 on channels 1 and 2, respectively.
// This is a hack to ensure that the DMX controller is working correctly.
// This hack is done as the MAX485 often reads faulty data, and this
// ensures that we only update the animation if the DMX controller is working correctly.

/**
 * DMX PROTOCOL
 * 1: Dimmer
 * 2: Red
 * 3: Green
 * 4: Blue
 * 5: Direction (0-127 = FORWARD, 128-255 = BACKWARD)
 * 6: UNUSED (Intended for rainbow effect, but not implemented)
 * 7: UNUSED (Intended for rainbow effect, but not implemented)
 * 8: Mask 1 type (1-9)
 * 9: Mask 1 parameter 1
 * 10: Mask 1 parameter 2
 * 11: Mask 1 parameter 3
 * 12: Mask 2 type (1-9)
 * 13: Mask 2 parameter 1
 * 14: Mask 2 parameter 2
 * 15: Mask 2 parameter 3
**/

/**
 * @brief As sections are defined by an array of bytes, and I wanted a simple way to interact with it, I simply create an array of X size, and
 * fill it with 255 for the first byte, and 0 for the rest.
 * 
 * @param value 
 * @return std::vector<u8_t>* 
 */
std::vector<u8_t> * to_sections(u8_t value) {
    std::vector<u8_t> * sections = new std::vector<u8_t>();
    sections->push_back(255);
    for (int i = 1; i < value; i++) sections->push_back(0);
    return sections;
}

/**
 * @brief Creates a vector of sections, where each section is filled with 255.
 * @param value 
 * @return std::vector<u8_t>* 
 */
std::vector<u8_t> * to_full_sections(u8_t value) {
    std::vector<u8_t> * sections = new std::vector<u8_t>();
    
    for (int i = 0; i < value; i++) sections->push_back(255);
    return sections;
}

/** 
 * @brief Scales the value to a range of 0-255, where:
 * - 0-31 is unchanged
 * - 32-63 is scaled to 32-96
 * - 64-127 is scaled to 96-352
 * - 128-191 is scaled to 352-864
 * - 192-255 is scaled to 864-2048
 * 
 * This scaling is used to create a more gradual transition between values,
 * as DMX only supports up to 255, but we want to use a wider range for effects.
 */
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

/**
 * @brief Checks whether the channels from `from` to `to` has changed compared to the previous channels.
 * 
 * @param channels channels to check
 * @param from first channel to check
 * @param to last channel to check
 * @return true 
 * @return false 
 */
bool hasChannelsChanged(u8_t* channels, u8_t from, u8_t to) {
    for (int i = from; i <= to; i++) {
        if (channels[i] != prevChannels[i]) {
            return true;
        }
    }
    return false;
}

// Create a color that will be used as the base color for the animation.
// We create an object, as we sometimes wish to use the existing color
// without resetting the wave animation
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
    // TODO: Change to DMX::ReadAll(uint8_t * data, uint16_t start, size_t size).
    // Do not that this will copy to the same indexes, so the channels array have to
    // be larger. Otherwise, we can also use DMX_IGNORE_THREADSAFETY = 1, which makes
    // Read quicker, as no semaphore is used.
    for (int i = 0; i < 15; ++i) {
        channels[i+1] = DMX::Read(i + DMX_START);
    }

    // Map the DMX channels to the animation.
    if (animator != nullptr) {
        dmx_to_animation(animator, channels);
    }
}
