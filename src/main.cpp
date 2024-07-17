#include <Arduino.h>
#include <RF24.h>
#include "customImpl.h"

#include "generic.cpp"
#include "madbod.cpp"
#include "skurvogn.cpp"

CustomImpl *impl = new Generic();


#define DEBUG 1
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif


#define CE_PIN 0
#define CSN_PIN 10
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = { "1Node", "2Node" };


#define id impl->getId()
#define num_channels 180 // fix this
static const int num_leds_in_strip = impl->getNumLeds();
CRGB leds[300];

#define LED_PIN  7 
#define built_in_led 8
bool split = false;
uint8_t dmx[num_channels];
uint8_t data[32];
uint8_t fragment[6][32];


// put function declarations here:
int recvData(); // Declare the recvData function
void setOneColour(const CRGB &colour);
void normalMode();
void setLEDs();
void setDMX();
void localEffect(uint8_t led_index, uint8_t effect_value);
void loopFromToColour(int from, int to, CRGB colour);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(built_in_led, OUTPUT);
  digitalWrite(built_in_led, HIGH); //Led is reversed. Low is on and high is off.
  delay(1000);

  if (!radio.begin()) {
    debug("radio hardware is not responding!!", 0);
    while (1) {
      digitalWrite(built_in_led, HIGH);
      delay(1000);
      digitalWrite(built_in_led, LOW);
      delay(1000);
      debug("radio hardware is not responding!!", 0);
    }  // hold in infinite loop
  }
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, num_leds_in_strip);
  //ws2812B 4pin sorte full cover
  //ws2811 til hvid strip hvid tape.  

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to each other.
  //radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  radio.setPayloadSize(sizeof(data));  //Here we are setting the size of data to be received as the size of data array.

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[0]);  // using pipe 1

  //radio.setAutoAck(false);
  //radio.setDataRate(RF24_250KBPS); //estimated max is only 64kbps
  radio.startListening();     
}

int recvData()
{
  //debug("start of recvData\n",0);
  uint8_t pipe;
  if ( radio.available(&pipe) ) {
    digitalWrite(built_in_led, LOW);
    //debug("radio available\n",0);
    uint8_t bytes = radio.getPayloadSize();
    radio.read(&data, bytes);
    digitalWrite(built_in_led, HIGH);
      
    // check fragment number
    debug("Fragment number: %d\n", data[0]);
    uint8_t offset = data[0] * 31;
    //debug("Fragment number: %d\n", data[0]);
    uint8_t limit = (data[0] == 5) ? 27 : 31; // last fragment has different size

    if (offset > 155) {
      return 0; // invalid fragment number
    }

    for (u_int i = 0; i < limit; i++) {
      dmx[i + offset] = data[i + 1];
    }
    
    return 1;
  }
  return 0;
}

void loop() {
  setOneColour(CRGB::Red);
  delay(1000);
  setOneColour(CRGB::Green);
  delay(1000);
  if(recvData() )
  {
    debug("Data received:\n",0);
    for (int i = 0; i < 32; i++)
    {
      debug("%d ", data[i]);
    }
    debug("\n",0);

    // print first 30 channels
    for (int i = 0; i < 30; i++)
    {
      debug("%d ", dmx[i]);
    }
    debug("\n",0);
    
    setDMX();
  }
}

uint8_t prevStep = 0;
long prevStepMillis = 0;
float bpm = 0;

// If called regularly, the getBPM updates and returns a BPM value based on the time difference between each step-increase
float getBPM(uint8_t step) {
  if (step == 0) return 0;
  if(step == prevStep) return bpm;
  long curtime = millis();
  long diff = curtime - prevStepMillis;
  prevStepMillis = curtime;
  bpm = 60000 / diff;
  return bpm;
}

struct EffectPayload {
  uint8_t effect_id;
  uint8_t step;
  uint8_t bpm;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t master_dimmer;
};

void setDMX(){
  //dmx[0] master dimmer
  //dmx[1] master R
  //dmx[2] master G
  //dmx[3] master B
  //dmx[4] Step
  //dmx[5] NOT USED
  //dmx[6] NOT USED
  //dmx[7] NOT USED
  //dmx[8] fixture A effect
  //dmx[9] fixture B effect
  //dmx[10] fixture C effect
  FastLED.setBrightness(dmx[0]);
  
  debug("dmx id: %d\n", id);
  debug("dmx effect_id: %d\n", dmx[id]);

  EffectPayload effect;
  effect.master_dimmer = dmx[0];
  effect.r = dmx[1];
  effect.g = dmx[2];
  effect.b = dmx[3];
  effect.step = dmx[4];
  effect.bpm = getBPM(dmx[4]);
  effect.effect_id = dmx[id];

  updateEffect(effect);
  FastLED.show();
}


uint8_t COLORS[8][3] = {
  {0, 0, 0},
  {255, 255, 255},
  {255, 0, 0},
  {0, 255, 0},
  {0, 0, 255},
  {255, 255, 0},
  {0, 255, 255},
  {255, 0, 255},
};

String COLOR_NAMES[8] = {
  "Black",
  "White",
  "Red",
  "Green",
  "Blue",
  "Yellow",
  "Cyan",
  "Magneta"
};


// --------------- local effect implementation (new protocol) ------------------
void updateEffect(EffectPayload effect) {
  if (effect.effect_id < 8) {
    setOneColour(CRGB(COLORS[effect.effect_id][0],COLORS[effect.effect_id][1],COLORS[effect.effect_id][2]));
  }


  switch (effect.effect_id) {
      case 0:
        debug("All leds off\n",0);
        setOneColour(CRGB::Black);
        break;
      case 1:
        debug("All LED to White\n",0);
        setOneColour(CRGB::White);
        break;
      case 2:
        debug("All LED to RED\n",0);
        setOneColour(CRGB::Red);
        break;
      case 3:
        debug("All LED to GREEN\n",0);
        setOneColour(CRGB::Green);
        break;
      case 4:
        debug("All LED to BLUE\n",0);
        setOneColour(CRGB::Blue);
        break;
      case 5:
        debug("All LED to Magneta\n",0);
        setOneColour(CRGB::Magenta);
        break;
      case 6:
        debug("All LED to Cyan\n",0);
        setOneColour(CRGB::Cyan);
        break;
      case 7:
        debug("All LED to Yellow\n",0);
        setOneColour(CRGB::Yellow);
        break;



      
      case 1:
        debug("All LED to R,G,B\n",0);
        setOneColour(CRGB(effect.r, effect.g, effect.b));
        break;
  }
}



// -------------------------local effect implementation-------------------------
  void localEffect(uint8_t led_index, uint8_t effect_value){
    //full color 0-7
    //
    debug("Effect value: %d\n", effect_value);
      switch (effect_value) {
      //full color 0-7
        case 0:
            // turn off all leds
            setOneColour(CRGB::Black);
            debug("All leds off\n",0);
            break;
        case 1:
            // All Red
            setOneColour(CRGB::Red);
            debug("All Red\n",0);
            debug("from impl \n",0);
            for (int i = 0; i < 3; i++)
            {
                printf("R: %d, G: %d, B: %d \n", leds[i].r, leds[i].g, leds[i].b);
            }
            break;
        case 2:
            // All Green
            setOneColour(CRGB::Green);
            debug("All Green\n",0);
            break;
        case 3:
            // All Blue
            setOneColour(CRGB::Blue);
            debug("All Blue\n",0);
            break;
        case 4:
            // All White
            setOneColour(CRGB::White);
            debug("All White\n",0);
            break;
        case 5:
            // All Yellow
            setOneColour(CRGB::Yellow);
            debug("All Yellow\n",0);
            break;
        case 6:
            // All Cyan
            setOneColour(CRGB::Cyan);
            debug("All Cyan\n",0);
            break;
        case 7:
            // All Magenta
            setOneColour(CRGB::Magenta);
            debug("All Magenta\n",0);
            break;
        

        //Strobe effect with speed control 
        case 8:
          // strobe Red
          if (led_index % 2 == 0) {
            setOneColour(CRGB::Red);
          } else {
            setOneColour(CRGB::Black);
          }
          break;
        case 9:
          // strobe Green
          if (led_index % 2 == 0) {
            setOneColour(CRGB::Green);
          } else {
            setOneColour(CRGB::Black);
          }
          break;
        case 10:
          // strobe Blue
          if (led_index % 2 == 0) {
            setOneColour(CRGB::Blue);
          } else {
            setOneColour(CRGB::Black);
          }
          break;
        case 11:
          // strobe White
          if (led_index % 2 == 0) {
            setOneColour(CRGB::White);
          } else {
            setOneColour(CRGB::Black);
          }
          break;
        case 12:
          // strobe Yellow
          if (led_index % 2 == 0) {
            setOneColour(CRGB::Yellow);
          } else {
            setOneColour(CRGB::Black);
          }
          break;
        case 13:
          // strobe Cyan
          if (led_index % 2 == 0) {
            setOneColour(CRGB::Cyan);
          } else {
            setOneColour(CRGB::Black);
          }
          break;
        case 14:
          // strobe Magenta
          if (led_index % 2 == 0) {
            setOneColour(CRGB::Magenta);
          } else {
            setOneColour(CRGB::Black);
          }
          break;

        //Animation fill from start to end
        case 15:
          // fill Red
          loopFromToColour(0, led_index, CRGB::Red);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
          break;
        case 16:
          // fill Green
          loopFromToColour(0, led_index, CRGB::Green);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
          break;
        case 17:
          // fill Blue
          loopFromToColour(0, led_index, CRGB::Blue);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
          break;
        case 18:
          // fill White
          loopFromToColour(0, led_index, CRGB::White);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
          break;
        case 19:
          // fill Yellow
          loopFromToColour(0, led_index, CRGB::Yellow);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
          break;
        case 20:
          // fill Cyan
          loopFromToColour(0, led_index, CRGB::Cyan);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
          break;
        case 21:
          // fill Magenta
          loopFromToColour(0, led_index, CRGB::Magenta);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
          break;
        
        //Animation unfill from start to end
        case 22:
          // unfill black
          loopFromToColour(0, led_index, CRGB::Black);
          break;

        case 255:
          CRGB* newLeds = impl->customEffect();
          for (int i = 0; i < num_leds_in_strip; i++) {
            leds[i] = newLeds[i];
          }
          break;
      }
    FastLED.show();  
  }


void setOneColour(const CRGB &colour) {
  for (int i = 0; i < num_leds_in_strip; i++) {
    leds[i] = colour;
  }
}

void loopFromToColour(int from, int to, CRGB colour){
  for (int i = from; i < to; i++) {
    leds[i] = colour;
  }
}



