#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Stolpe : public CustomImpl {
    
    int id = 13; // Declare the 'id' variable as an integer with the value of 8
    static const int num_leds = 50; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150

    long lastAnimationMillis = 0;
    long tick = 0;

    public:
       
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect() override {
            //return customEffectA();
            return customEffectB();
        }

        CRGB* customEffectB() {
            long currentMillis = millis();
            const long speed = 200;

            const int snakeLength = num_leds/3; // Number of turned on LEDs 

            CRGB currentColour = CRGB::Black;

            if (currentMillis - lastAnimationMillis > speed){
                
                currentColour = CRGB::White;

                for (int i = tick; i < tick + snakeLength; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = currentColour;
                    } else {
                        leds[i] = currentColour;
                    }
                }

                currentColour = CRGB::Orange;

                for (int i = tick + snakeLength; i < tick + snakeLength*2; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = currentColour;
                    } else {
                        leds[i] = currentColour;
                    }
                }

                currentColour = CRGB::Black;

                for (int i = tick + snakeLength*2; i < tick + snakeLength*3; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = currentColour;
                    } else {
                        leds[i] = currentColour;
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
            uint8_t sectionsStartIndex[] = {0, 50, 100};
            LEDSections sections = {sectionsStartIndex, sizeof(sectionsStartIndex)};
            return sections;
        }

};








