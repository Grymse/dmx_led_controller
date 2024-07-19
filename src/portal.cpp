#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Portal : public CustomImpl {
    
    int id = 32; // 32
    static const int num_leds =  50; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150
    public:
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect(long tick) override {
            // put your main code here, to run repeatedly:
            debug("Generic custom effect\n",0);
            rainbow(2, tick);
            
            return leds;
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

        LEDSections getLEDSections() {
            LEDSections sections = {{0, 10, 20, 30, 40}, 5};
            return sections;
        }

    void rainbow(uint8_t length, long tick) {
        for (int i = 0; i < num_leds; i++) {
            leds[i] = CHSV(i * length - (tick % 255), 255, 255);
        }
    }
};










