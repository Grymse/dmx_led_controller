#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Generic : public CustomImpl {
    
    int id = 3; // Declare the 'id' variable as an integer with the value of 3
    const int num_leds = 150; // Declare the 'num_leds' variable as an integer with the value of 150

    public:
        
       
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);


        }

        void customEffect(CRGB *leds) override {
            // put your main code here, to run repeatedly:
            debug("Generic custom effect\n",0);
            // alternate blue and red
            for (int i = 0; i < num_leds_in_strip; i++) {
                if (i % 2 == 0) {
                    leds[i] = CRGB::Blue;
                } else {
                    leds[i] = CRGB::Red;
                }
            }
            
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

};








