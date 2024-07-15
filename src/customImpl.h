#ifndef CUSTOMIMPL_H
#define CUSTOMIMPL_H
#include <Arduino.h>
#include <FastLED.h>

class CustomImpl {
    public:
        int id;
        int num_leds_in_strip;
        //const int split;
        virtual void customEffect(CRGB *leds) = 0;
        virtual void setBrightness(uint8_t brightness) = 0;
    
    
        void localEffect(CRGB *leds, uint8_t led_index, uint8_t effect_value){
            //full color 0-7
            //
            switch (effect_value) {
                //full color 0-7
                case 0:
                    // turn off all leds
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::Black;
                    }
                    break;
                case 1:
                    // All Red
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::Red;
                    }
                    break;
                case 2:
                    // All Green
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::Green;
                    }
                    break;
                case 3:
                    // All Blue
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::Blue;
                    }
                    break;
                case 4:
                    // All White
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::White;
                    }
                    break;
                case 5:
                    // All Yellow
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::Yellow;
                    }
                    break;
                case 6:
                    // All Cyan
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::Cyan;
                    }
                    break;
                case 7:
                    // All Magenta
                    for (int i = 0; i < num_leds_in_strip; i++) {
                        leds[i] = CRGB::Magenta;
                    }
                    break;
                
                case 255:
                    customEffect(leds);
                    break;
            }  
        }
    
        int getID() {
            return id;
        }
        int getNumLeds() {
            return num_leds_in_strip;
        }
};


#endif
