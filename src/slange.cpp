#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class slange : public CustomImpl {
    
    int id = 20; // Declare the 'id' variable as an integer with the value of 8
    static const int num_leds =  150; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150

    long lastAnimationMillis = 0;
    long tick = 0;

    public:
       
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect() override {
            // put your main code here, to run repeatedly:
            debug("Generic custom effect\n",0);
            // alternate blue and red

            long currentMillis = millis();

            const int snakeLength = 10; // Number of turned on LEDs 

            if (currentMillis - lastAnimationMillis > 500){
                
                for (int i = tick; i < tick + snakeLength; i++) {
                    leds[i] = CRGB::Blue;
                }

                lastAnimationMillis = currentMillis;
                tick++;
            }
            
            debug("Tick: %d\n", tick);

            FastLED.show();  
            
            return leds;
            
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

};








