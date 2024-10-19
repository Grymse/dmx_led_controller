#include <Arduino.h>
#include <FastLED.h>
#include "ISections.h"
#include <vector>

class StaticSections : ISections {
public:
  std::vector<CRGB> sections;
  
  StaticSections(std::vector<CRGB> sections) {
    this->sections = sections;
  }


  String getName() {
    return "Static";
  }

  size_t count() {
    return sections.size();
  }

  CRGB getSection(long tick, u16_t sectionIndex) {
    return sections[sectionIndex % sections.size()];
  }
};