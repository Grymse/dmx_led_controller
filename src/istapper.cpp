#include <Arduino.h>
#include <cstdlib>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Istapper : public CustomImpl {
    
    int id = 21; // Declare the 'id' variable as an integer with the value of 3
    static const int num_leds =  150/3; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150
    public:
        
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect() override {
            const int numberOfIstapper = 16;

            const int istapRanges[numberOfIstapper][2] = {
                {0, 9},     //1
                {10, 19},   //2
                {20, 29},   //3
                {30, 39},   //4
                {40, 49},   //5
                {50, 59},   //6
                {60, 69},   //7
                {70, 79},   //8
                {80, 89},   //9
                {90, 99},   //10
                {100, 109}, //11
                {110, 119}, //12
                {120, 129}, //13
                {130, 139}, //14
                {130, 139}, //15
                {140, 150}, //16
            };
            
            srand(time(0));
            int randomNum = rand() % numberOfIstapper;

            debug("Istap number: %d\n", randomNum);

            int lower = istapRanges[randomNum][0];
            int upper = istapRanges[randomNum][1];

            for (int i = 0; i < num_leds; i++) {
                    leds[i] = CRGB::Black;
                }

            for (int i = lower; i <= upper; i++) {
                leds[i] = CRGB::Yellow;
            }
            
            return leds;
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

};








