#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Portalen : public CustomImpl {
    
    int id = 32; // Declare the 'id' variable as an integer with the value of 8
    static const int num_leds =  50; // Declare the 'num_leds' variable as an integer with the value of 150

    long lastAnimationMillis = 0;
    long tick = 0;

    
    const long speed = 150;

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150
    public:
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect(long tick) override {
            // put your main code here, to run repeatedly:
            debug("Generic custom effect\n",0);
            long currentMillis = millis();
            const int snakeLength = 15; // Number of turned on LEDs 
            const int snakeLength2 = 15; // Number of turned on LEDs

            if (currentMillis - lastAnimationMillis > speed){
                
                for (int i = 0; i < num_leds; i++) {
                    leds[i] = CRGB::Magenta;
                }

                for (int i = tick; i < tick + snakeLength; i++) {
                    leds[i] = CRGB::Red;
                    if (i >= num_leds) {
                        leds[i % num_leds] = CRGB::Red;
                    }
                }
                
                for (int i = tick+snakeLength+ 10; i < tick+snakeLength+ 10 + snakeLength2; i++) {
                    leds[i] = CRGB::Blue;
                    if (i >= num_leds) {
                        leds[i % num_leds] = CRGB::Blue;
                    }
                }
                lastAnimationMillis = currentMillis;
                tick++;

                if (tick >= num_leds) tick = 0;
                debug(" - Move - %d\n", NULL);
            }
            
            debug("Tick: %d\n", tick);
            
            return leds;
        }

        

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

        LEDSections getLEDSections() {
            LEDSections sections = {{0, 50, 100}, 3};
            return sections;
        }
};








