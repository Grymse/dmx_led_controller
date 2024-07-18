#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Podiet : public CustomImpl {
    
    int id = 28; // Declare the 'id' variable as an integer with the value of 8
    static const int num_leds =  50; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150
    public:
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect() override {
            // put your main code here, to run repeatedly:
            debug("Generic custom effect\n",0);
            // Solid color orange
            for (int i = 0; i < num_leds; i++) {
                    leds[i] = CRGB::Magenta;
            }
            
            return leds;
        }

        

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

        LEDSections getLEDSections() {
            uint8_t sectionsStartIndex[] = {0, 50, 100};
            LEDSections sections = {sectionsStartIndex, sizeof(sectionsStartIndex)};
            return sections;
        }
};








