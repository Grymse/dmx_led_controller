#include <Arduino.h>
#include "customImpl.h"


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif 

class Katapult
 : public CustomImpl {
    
    int id = 17; // KATAPULT: 8,9,10,11,12, FEST-TELT: 17,18
    static const int num_leds =  150; // Declare the 'num_leds' variable as an integer with the value of 150

    CRGB leds[num_leds]; // Declare the 'leds' variable as an array of CRGB with the size of 150
    public:
        void setBrightness(uint8_t brightness) override {
            // put your main code here, to run repeatedly:
            debug("Set brightness\n",brightness);
        }

        CRGB* customEffect(long tick) override {
            // put your main code here, to run repeatedly:
            debug("Generic custom effect\n",0);
            // alternate blue and red
            int step = tick / 20;
            uint8_t colors = 6;
            uint8_t currentMethod = (step / 30) % 6;

            CRGB color = CRGB(COLORS[step / 8 % colors][0], COLORS[step / 8 % colors][1], COLORS[step / 8 % colors][2]);

            switch(currentMethod) {
                case 0:
                    sectionsFull(tick / 5, color, false);
                    break;
                case 1:
                    sectionsFull(step, color, false);
                    break;
                case 2:
                    sectionsFull(step, color, true);
                    break;
                case 3:
                    blink(step, color);
                    break;
                case 4:
                    sections2(step, color);
                    break;
                case 5:
                    sectionsFull(tick / 5, color, true);
                    break;
            }

            return leds;
        }

        int getId() override {
            return id;
        }
        
        int getNumLeds() {
            return num_leds;
        }

        LEDSections getLEDSections() {
            LEDSections sections = {{0, 30, 60, 90, 120}, 5};
            return sections;
        }


        uint8_t COLORS[6][3] = {
            {255, 0, 0},
            {0, 255, 0},
            {0, 0, 255},
            {255, 255, 0},
            {0, 255, 255},
            {255, 0, 255},
        };


        void blink(long step, CRGB color) {
          for (int i = 0; i < num_leds; i++) {
            leds[i] = step % 2 == 0 ? color : CRGB::Black;
          }
        }


        void sections2(long step, CRGB color) {
          LEDSections sections = getLEDSections();
          
          for (long i = 0; i < sections.noOfSections; i++) {
            setSection(i, (i + step) % 2 == 0 ? color : CRGB::Black);
          }
        }
        void sectionsFull(long step, CRGB color, bool reverse) {
          LEDSections sections = getLEDSections();

          step = reverse ? 1000000-step : step;
          for (long i = 0; i < sections.noOfSections; i++) {
            setSection(i, (i + step) % sections.noOfSections == 0 ? color : CRGB::Black);
          }
        }


        void setSection(uint8_t i, CRGB color) {
            LEDSections sections = getLEDSections();
            uint16_t start = sections.sectionsStartIndex[i];
            uint16_t end = sections.noOfSections == i + 1 ? num_leds : sections.sectionsStartIndex[i + 1];

            loopFromToColour(start, end, color);
        }


        void loopFromToColour(int from, int to, CRGB colour){
            for (int i = from; i < to; i++) {
                leds[i] = colour;
            }
        }
};










