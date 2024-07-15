#include <Arduino.h>
#include "customImpl.h"

#define dmxID 4
#define num_leds 150

class Skurvogn : public CustomImpl {
    public:
        int id = dmxID;
        int num_leds_in_strip = num_leds;

};








