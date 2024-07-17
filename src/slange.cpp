#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Slange : public CustomImpl {
    
    int id = 20; // Declare the 'id' variable as an integer with the value of 8
    static const int num_leds =  300; // Declare the 'num_leds' variable as an integer with the value of 150

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

        CRGB* customEffectA() {
            long currentMillis = millis();
            const long speed = 40;

            const int snakeLength = 25; // Number of turned on LEDs 

            if (currentMillis - lastAnimationMillis > speed){
                
                for (int i = 0; i < num_leds; i++) {
                    int white = 200;
                    leds[i].setRGB(white,white,white);
                    //leds[i] = CRGB::Blue;
                }

                for (int i = tick; i < tick + snakeLength; i++) {
                    if (id == 20) leds[i] = CRGB::Red;
                    if (id == 21) leds[i] = CRGB::Yellow;
                    if (id == 22) leds[i] = CRGB::Blue;
                }

                lastAnimationMillis = currentMillis;
                tick++;

                if (tick >= num_leds) tick = 0;
                debug(" - Move - %d\n", NULL);
            }
            
            debug("Tick: %d\n", tick);
            
            return leds;
        }

        CRGB* customEffectB() {
            long currentMillis = millis();
            const long speed = 40;

            const int snakeLength = num_leds/3; // Number of turned on LEDs 

            if (currentMillis - lastAnimationMillis > speed){

                for (int i = tick; i < tick + snakeLength; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = CRGB::Red;
                    } else {
                        leds[i] = CRGB::Red;
                    }
                }

                for (int i = tick + snakeLength; i < tick + snakeLength*2; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = CRGB::Blue;
                    } else {
                        leds[i] = CRGB::Blue;
                    }
                }

                for (int i = tick + snakeLength*2; i < tick + snakeLength*3; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = CRGB::Pink;
                    } else {
                        leds[i] = CRGB::Pink;
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








