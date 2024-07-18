#include <Arduino.h>
#include <cstdlib>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Drinks : public CustomImpl {
    
    int id = 24; // Declare the 'id' variable as an integer with the value of 3
    static const int num_leds =  50; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150

    long lastAnimationMillis = 0;

    public:
        
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect(long tick) override {

            srand(time(0));
            int randomNum1 = rand() % 7;
            int randomNum2 = rand() % 7;
            int randomNum3 = rand() % 7;

            debug("randomNum1: %d\n", randomNum1);
            debug("randomNum2: %d\n", randomNum2);
            debug("randomNum3: %d\n", randomNum3);
            
            long currentMillis = millis();
            const long speed = 500;

            if (currentMillis - lastAnimationMillis > speed){
                
                for (int i = 0; i < num_leds/3; i++) {
                    leds[i].setRGB(
                        COLORS[randomNum1][0], 
                        COLORS[randomNum1][1], 
                        COLORS[randomNum1][2]
                    );
                }

                for (int i = num_leds/3; i < num_leds/3*2; i++) {
                    leds[i].setRGB(
                        COLORS[randomNum2][0], 
                        COLORS[randomNum2][1], 
                        COLORS[randomNum2][2]
                    );
                }

                for (int i = num_leds/3*2; i < num_leds; i++) {
                    leds[i].setRGB(
                        COLORS[randomNum3][0], 
                        COLORS[randomNum3][1], 
                        COLORS[randomNum3][2]
                    );
                }

                lastAnimationMillis = currentMillis;
            }
            
            return leds;
        }

        uint8_t COLORS[7][3] = {
        {255, 255, 255},
        {255, 0, 0},
        {0, 255, 0},
        {0, 0, 255},
        {255, 255, 0},
        {0, 255, 255},
        {255, 0, 255},
        };

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








