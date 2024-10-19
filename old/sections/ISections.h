#include <Arduino.h>
#include <FastLED.h>



class ISections {
public:
  virtual ~ISections() {}

  virtual String getName() = 0;

  virtual size_t count() = 0;

  virtual CRGB getSection(long tick, u16_t sectionIndex) = 0;
};