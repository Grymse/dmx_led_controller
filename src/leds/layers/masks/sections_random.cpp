#include "masks.h"
#include "../utils.h"
#include <vector>
#include <algorithm>
#include <random>

// Constructor
SectionsRandomMask::SectionsRandomMask(std::vector<u8_t> sections, u16_t duration)
    : duration(duration), sections(std::move(sections)) {
        this->tick_of_next_update = duration;
        this->current_section = random(0, sections.size());
    }

// Returns the name of the layer
String SectionsRandomMask::getName() {
    return "SectionsRandomMask";
}

// Returns a string representation of the layer
String SectionsRandomMask::toString() {
  String str = "SectionsRandomMask: d: " + String(this->duration) + ", c: ";
  str += LayerUtils::bytes_to_string(this->sections);
  
  return str;
}

// Encodes the layer into a protocol_Layer object
protocol_Layer SectionsRandomMask::toEncodable() {
  return protocol_Layer {
    .type = protocol_LayerType_SectionsWaveMask,
    .duration = this->duration,
    .sections = {
      .arg = &this->sections
    }
  };
}

// Applies the random section mask to the color based on the LED state
CRGB SectionsRandomMask::apply(CRGB color, LEDState* state) {
    // Update new section if required
    if (state->tick == this->tick_of_next_update) {
        this->current_section = random(0, this->sections.size());
        this->tick_of_next_update = state->tick + this->duration;
    }

    u16_t segmentDuration = this->duration / this->sections.size();
    float sectionLength = (float)state->length / this->sections.size();
    u16_t sectionIndex = state->tick / segmentDuration + (state->index / sectionLength);

    if (this->current_section != sectionIndex) {
        return CRGB::Black; // If the current section does not match, return black
    }

    return color.scale8(this->sections[this->current_section]);
}