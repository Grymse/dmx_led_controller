#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Generic : public CustomImpl {
    
    int id = 8; // Declare the 'id' variable as an integer with the value of 8
    static const int num_leds =  60; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150
    public:
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect(long tick) override {
            // put your main code here, to run repeatedly:
            debug("Generic custom effect\n",0);
            // alternate blue and red
            int localTick = tick % (50 * 320);
            
            if(localTick < 50 * 60) // 40 seconds
                wave(1, 1, tick, 255, 0, 0);
            else if(localTick < 50 * 120) // 80 seconds
                wave(1, 1, tick, 0, 255, 0);
            else if(localTick < 50 * 80) // 120 seconds
                wave(1, 1, tick, 0, 0, 255);
            else // 120-240 seconds
                rainbow(1, tick);
            
            return leds;
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

        LEDSections getLEDSections() {
            LEDSections sections = {{0, 6, 12, 18, 24, 30, 36, 42, 48, 54}, 10};
            return sections;
        }
    
void wave(uint8_t length, uint8_t multiplier, long tick, byte r, byte b, byte g) {
  uint8_t reducedBPM = 120 / 30; // divided by 30, to make effect more stable!
  float speed_multiplier = (((float) reducedBPM) / 2) * multiplier;
  uint16_t wavelength = 512 / length;
  uint16_t half_wavelength = wavelength / 2;

  int num_leds_in_strip = 150;

  for (uint16_t i = 0; i < num_leds_in_strip; i++) {
    uint16_t count = ((uint16_t) (i + tick * speed_multiplier)) % wavelength;
    if(half_wavelength < count) {
      count = half_wavelength - count;
    }

    float wave = (float) count / half_wavelength;
    leds[i] = CRGB(r * wave, g * wave, b * wave);
  }
}


    void rainbow(uint8_t length, long tick) {
    uint8_t speed_multiplier = 120 / 120; // divided by 30, to make effect more stable!

    num_leds_in_strip = 150;
    
    for (int i = 0; i < num_leds_in_strip; i++) {
        leds[i] = CHSV(i - ((tick * speed_multiplier) % 255 * length), 255, 255);
    }
    }
};










