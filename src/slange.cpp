#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Slange : public CustomImpl {
    
    int id = 19; // Declare the 'id' variable as an integer with the value of 8
    static const int num_leds = 50; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150

    long lastAnimationMillis = 0;
    long tick = 0;

    public:
       
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect(long tick) override {
            //return customEffectA();
            return customEffectB();
        }

        CRGB* customEffectA() {
            long currentMillis = millis();
            const long speed = 100;

            const int snakeLength = 1; // Number of turned on LEDs 

            if (currentMillis - lastAnimationMillis > speed){
                
                for (int i = 0; i < num_leds; i++) {
                    int white = 200;
                    leds[i].setRGB(white,white,white);
                    //leds[i] = CRGB::Blue;
                }

                for (int i = tick; i < tick + snakeLength; i++) {
                    if (id == 20) leds[i] = CRGB::Red;
                    if (id == 21) leds[i] = CRGB::Red;
                    if (id == 22) leds[i] = CRGB::Red;
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
            const long speed = 200;

            const int snakeLength = num_leds/3; // Number of turned on LEDs 

            CRGB currentColour = CRGB::Black;

            if (currentMillis - lastAnimationMillis > speed){
                
                if (id == 20) currentColour = CRGB::Red;
                if (id == 21) currentColour = CRGB::Green;
                if (id == 19) currentColour = CRGB::Orange;

                for (int i = tick; i < tick + snakeLength; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = currentColour;
                    } else {
                        leds[i] = currentColour;
                    }
                }

                if (id == 20) currentColour = CRGB::Blue;
                if (id == 21) currentColour = CRGB::MediumPurple;
                if (id == 19) currentColour = CRGB::Blue;

                for (int i = tick + snakeLength; i < tick + snakeLength*2; i++) {
                    if(i > num_leds){
                        leds[i-num_leds] = currentColour;
                    } else {
                        leds[i] = currentColour;
                    }
                }

                if (id == 20) currentColour = CRGB::Magenta;
                if (id == 21) currentColour = CRGB::Orange;
                if (id == 19) currentColour = CRGB::White;

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
            LEDSections sections = {{0, 50, 100}, 3};
            return sections;
        }

};








