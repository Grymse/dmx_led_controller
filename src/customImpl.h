#ifndef CUSTOMIMPL_H
#define CUSTOMIMPL_H
#include <Arduino.h>
#include <FastLED.h>


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

struct LEDSections {
  uint8_t sectionsStartIndex[12]; // Pointer to the start index of each section
  uint8_t noOfSections; // Number of sections
};

class CustomImpl {
    
    public:
        
        virtual int getId() = 0;
        virtual int getNumLeds() = 0;
        int num_leds_in_strip;
        //const int split;
        virtual CRGB* customEffect(long tick) = 0;
        virtual void setBrightness(uint8_t brightness) = 0;
        virtual LEDSections getLEDSections() = 0;
};


#endif
