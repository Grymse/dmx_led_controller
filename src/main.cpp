#include <Arduino.h>
#include <RF24.h>
#include "customImpl.h"

#include "generic.cpp"

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

/**
 * DECLARATION OF STRUCTS
 */

struct DMXPayload {
  uint8_t effect_id;
  uint8_t step;
  uint16_t bpm;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t master_dimmer;
};
enum Animation_Type {NONE, STROBE};

/**
 * DECLARATION OF FUNCTIONS
 */
int recvData(); // Declare the recvData function
void setOneColour(const CRGB &colour);
void normalMode();
void setLEDs();
void pushDMXtoLED();
void localEffect(uint8_t led_index, uint8_t effect_value);
void loopFromToColour(int from, int to, CRGB colour);
void setAnimation(Animation_Type new_animation);
void alternateColorPicker(uint8_t step, uint8_t colors[][3], uint8_t num_colors);
void alternateColor(DMXPayload payload);
void updateEffect(DMXPayload payload);
void setToFullColor(DMXPayload payload);

// Animations
void animate();
void strobe();

Animation_Type animation = NONE;
long tick = 0; // Used to keep track of local animations
DMXPayload payload;

void setAnimation(Animation_Type new_animation) {
  animation = new_animation;
  tick = 0;
}

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
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, num_leds_in_strip);
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
    
    pushDMXtoLED();
  }

  if(animation != NONE) animate();
}

uint8_t prevStep = 0;
long prevStepMillis = 0;
float bpm = 0;

// If called regularly, the getBPM updates and returns a BPM value based on the time difference between each step-increase
uint16_t getBPM(uint8_t step) {
  if (step == 0) return 0;
  if (step == prevStep) return bpm;
  long curtime = millis();
  long diff = curtime - prevStepMillis;
  prevStepMillis = curtime;
  bpm = (bpm * 3) / 4;
  bpm += (60000 / diff) / 4;

  return (uint16_t) bpm;
}

void pushDMXtoLED(){
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
  
  payload.master_dimmer = dmx[0];
  payload.r = dmx[1];
  payload.g = dmx[2];
  payload.b = dmx[3];
  payload.step = dmx[4];
  payload.bpm = getBPM(dmx[4]);
  payload.effect_id = dmx[id];

  debug("dmx bpm: %d\n", payload.bpm);
  debug("dmx id: %d\n", id);
  debug("dmx effect_id: %d\n", dmx[id]);

  updateEffect(payload);
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

/**
 * @brief Set the To Full Color object
 * Expects the ID to be between 0-8
 * 0: All leds to R,G,B
 * 1-8: Picks a color from COLORS array
 * @param payload DMX payload
 */
void setToFullColor(DMXPayload payload) {
    if (payload.effect_id == 0) {

      String color = "All leds to color: " + String(payload.r) + "," + String(payload.g) + "," + String(payload.b) + "\n";
      debug(color.c_str(), 0);
      
      setOneColour(CRGB(payload.r, payload.g, payload.b));
      return;
    }

    uint8_t color_id = payload.effect_id - 1;
    debug("All leds to color: %s\n", COLOR_NAMES[color_id].c_str());
    setOneColour(CRGB(
      COLORS[color_id][0], // Red color from color_id array
      COLORS[color_id][1], // Green color from color_id array
      COLORS[color_id][2] // Blue color from color_id array
    ));
}

uint8_t RG[2][3] = {{255, 0, 0}, {0, 255, 0}};
uint8_t GB[2][3] = {{0, 255, 0}, {0, 0, 255}};
uint8_t BR[2][3] = {{0, 0, 255}, {255, 0, 0}};
uint8_t ALL[7][3] = {{255, 0, 0},
{0, 255, 0},
{0, 0, 255},
{255, 255, 0},
{0, 255, 255},
{255, 0, 255},
{255, 255, 255}};

/**
 * @brief Set the To Alternate colors usings step-count
 * Expects the ID to be between 16-21
 * 16:	Red, Green
 * 17:	Green, Blue
 * 18:	Blue, Red
 * 19:	Red, Green, Blue
 * 20:	COLOR_ARRAY (no white)
 * 21:	COLOR_ARRAY (with white)
 * @param payload DMX payload
 */
void alternateColor(DMXPayload payload) {
  switch (payload.effect_id) {
    case 16:
      alternateColorPicker(payload.step, RG, 2);
      break;
    case 17:
      alternateColorPicker(payload.step, GB, 2);
      break;
    case 18:
      alternateColorPicker(payload.step, BR, 2);
      break;
    case 19:
      alternateColorPicker(payload.step, ALL, 3);
      break;
    case 20:
      alternateColorPicker(payload.step, ALL, 6);
      break;
    case 21:
      alternateColorPicker(payload.step, ALL, 7);
      break;
  }
}

void alternateColorPicker(uint8_t step, uint8_t colors[][3], uint8_t num_colors) {
  uint8_t* color = colors[step % num_colors];
  setOneColour(CRGB(
    color[0], // Red color from colors_array
    color[1], // Green color from colors_array
    color[2] // Blue color from colors_array
  ));
}

// --------------- local effect implementation (new protocol) ------------------
void updateEffect(DMXPayload payload) {
  animation = NONE; // RESET ANIMATION TYPE. OVERWRITE TO CORRECT ANIMATION
                    // DO NOT REMOVE THIS LINE.
                    // Do not set animation directly in the following code. Must use setAnimation!!

  if (payload.effect_id <= 8) setToFullColor(payload);

  if (16 <= payload.effect_id && payload.effect_id <= 21)
    alternateColor(payload);

  if (32 <= payload.effect_id && payload.effect_id <= 36) setAnimation(STROBE);
}

// -------------------------OLD effect implementation-------------------------
  void localEffect(uint8_t led_index, uint8_t effect_value){
    //full color 0-7
    //
    debug("Effect value: %d\n", effect_value);
      switch (effect_value) {
        //Animation fill from start to end
        case 15:
          // fill Red
          loopFromToColour(0, led_index, CRGB::Red);
          loopFromToColour(led_index, num_leds_in_strip, CRGB::Black);
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


long lastAnimationMillis = 0;

void animate() {
  long currentMillis = millis();
  if (currentMillis - lastAnimationMillis < 20) return;
  lastAnimationMillis = currentMillis;
  tick++;
  debug("Tick: %d\n", tick);
  debug("animation: %d\n", animation);

  switch (animation) {
    case STROBE:
      strobe();
      break;
  }

  FastLED.show();  
}


// 32	Ultra-Slow
// 33	Slow
// 34	Medium
// 35	Fast
// 36	Ultra-fast
uint8_t strobeLength[5] = {50, 20, 10, 7, 5};
uint8_t strobeOnlength[5] = {10, 5, 3, 2, 1};

void strobe() {
  uint8_t length = strobeLength[payload.effect_id - 32];
  uint8_t onLength = strobeOnlength[payload.effect_id - 32];

  if(tick % length < onLength) {
    setOneColour(CRGB::White);
  } else {
    setOneColour(CRGB::Black);
  }
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


