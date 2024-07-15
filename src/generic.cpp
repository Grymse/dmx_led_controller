#include <Arduino.h>
#include "customImpl.h"

#define dmxID 3
#define num_leds 150

#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Generic : public CustomImpl {
    public:
        int id = dmxID;
        int num_leds_in_strip = num_leds;

        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);


        }

        void customEffect(CRGB *leds) override {
            // put your main code here, to run repeatedly:
            debug("Custom effect\n",0);
            // alternate blue and red
            for (int i = 0; i < num_leds_in_strip; i++) {
                if (i % 2 == 0) {
                    leds[i] = CRGB::Blue;
                } else {
                    leds[i] = CRGB::Red;
                }
            }
            
        }

};








