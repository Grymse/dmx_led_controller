#include <Arduino.h>
#include <cstdlib>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class istapper : public CustomImpl {
    
    int id = 3; // Declare the 'id' variable as an integer with the value of 3
    static const int num_leds =  150; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150
    public:
        
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect() override {
            const int numberOfIstapper = 10;

            const int istapRanges[numberOfIstapper][2] = {
                {0, 5},
                {7, 14},
                {16, 21},
                // ... fill in the remaining values
            };
            
            srand(time(0));
            int randomNum = rand() % numberOfIstapper;

            int lower = istapRanges[randomNum][0];
            int upper = istapRanges[randomNum][1];

            for (int i = lower; i <= upper; i++) {
                leds[i] = CRGB::FloralWhite;
            }
    
/*             for (int i = 0; i < numberOfIstapper; i++) {
                for (int j = 0; j < 2; j++) {
                    int v = istapRanges[i][j];
                    if (v != 0){
                    }
                }
            } */
            
            return leds;
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

        LEDSections getLEDSections() {
            uint8_t sectionsStartIndex[] = {0, 10, 30, 60, 80, 90, 100};
            LEDSections sections = {sectionsStartIndex, 7};
            return sections;
        }
};








