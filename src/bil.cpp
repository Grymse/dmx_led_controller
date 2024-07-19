#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Bil : public CustomImpl {
    
    int id = 30; // only 30
    static const int num_leds =  300; // Declare the 'num_leds' variable as an integer with the value of 150

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

            
            
            if (tick%(50 /* tick */ * 60 /* sekunder */ * 5 /* minutter */) < 80) { // Gimmick hvert 5 minut
               for (int i = 0; i < num_leds; i++) {
                leds[i] = tick % 6 < 2 ? CRGB::White : CRGB::Black;
                }
            } else wave(1, 1, tick, 255, 0, 0);
            
            return leds;
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

        LEDSections getLEDSections() {
            LEDSections sections = {{0, 50, 100, 150, 200, 250}, 6};
            return sections;
        }
        
    void wave(uint8_t length, uint8_t multiplier, long tick, byte r, byte b, byte g) {
        uint8_t reducedBPM = 120 / 30; // divided by 30, to make effect more stable!
        float speed_multiplier = (((float) reducedBPM) / 2) * multiplier;
        uint16_t wavelength = 512 / length;
        uint16_t half_wavelength = wavelength / 2;

        for (uint16_t i = 0; i < num_leds; i++) {
            uint16_t count = ((uint16_t) (i + tick * speed_multiplier)) % wavelength;
            if(half_wavelength < count) {
            count = half_wavelength - count;
            }

            float wave = (float) count / half_wavelength;
            leds[i] = CRGB(r * wave, g * wave, b * wave);
        }
    }

};










