#include <Arduino.h>
#include <RF24.h>
#include "customImpl.h"

#include "generic.cpp"
#include "slange.cpp"
#include "istapper.cpp"
#include "portalen.cpp"
#include "drinks.cpp"
#include "stolpe.cpp"
#include "lysekrone.cpp"

CustomImpl *impl = new Lysekrone();

#define TEST 0

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
#define timeout_millis 120000
static const int num_leds_in_strip = impl->getNumLeds();
static const LEDSections sections = impl->getLEDSections();
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

enum Animation_Type {NONE, STROBE, WAVE, RAINBOW};

/**
 * DECLARATION OF FUNCTIONS
 */
int recvData(); // Declare the recvData function
void setOneColour(const CRGB &colour);
void normalMode();
void setLEDs();
void pushDMXtoLED();
void loopFromToColour(int from, int to, CRGB colour);
void setAnimation(Animation_Type new_animation);
void setSoloMode();

// Effects
void alternateColorPicker(uint8_t step, uint8_t colors[][3], uint8_t num_colors);
void alternateColor(DMXPayload payload);
void updateEffect(DMXPayload payload);
void setToFullColor(DMXPayload payload);
void customEffect();
void fillEffect(DMXPayload payload);
void setSection(uint8_t i, CRGB color);
void sections2(DMXPayload payload);
void sections3(DMXPayload payload);
void sections4(DMXPayload payload);
void sectionsFull(DMXPayload payload);
void sectionsRandom1(DMXPayload payload);
void sectionsRandom2(DMXPayload payload);
void setSectionEffect(DMXPayload payload);

// Animations
void animate();
void strobe();
void wave(uint8_t length, uint8_t multiplier);
void rainbow(uint8_t length);

Animation_Type animation = NONE;
long tick = 0; // Used to keep track of local animations
DMXPayload payload;

void setAnimation(Animation_Type new_animation) {
  animation = new_animation;
  tick = 0;
}

void setSoloMode() {
  payload.master_dimmer = 255;
  payload.effect_id = 255;
  pushDMXtoLED();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Set initial values
  setSoloMode();

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

  FastLED.addLeds<WS2812B, LED_PIN, BRG>(leds, num_leds_in_strip);
  // ws2812B 4pin sorte full cover
  // ws2811 til hvid strip hvid tape - BRG

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

long lastReceivedData = 0;


void loop() {
  if (millis() - lastReceivedData > timeout_millis) {
    setSoloMode();
    debug("setSoloMode\n", "");
    debug("%d ", millis());
    debug("%d ", lastReceivedData);
  }

  if(recvData() )
  {
    lastReceivedData = millis();
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
    
  } else if(payload.effect_id == 255){
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
  bpm = (bpm * 7) / 8;
  bpm += (60000 / diff) / 8;

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
  //payload.effect_id = 255; // Skal udkommenteres

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

  if (40 <= payload.effect_id && payload.effect_id <= 43) fillEffect(payload);

  if (57 <= payload.effect_id && payload.effect_id <= 64) setAnimation(WAVE);

  if (65 <= payload.effect_id && payload.effect_id <= 68) setAnimation(RAINBOW);

  if (128 <= payload.effect_id && payload.effect_id <= 133) setSectionEffect(payload);

  if (payload.effect_id == 255) customEffect();
}

void setSectionEffect(DMXPayload payload) {
  switch(payload.effect_id) {
    case 128:
      sections2(payload);
      break;
    case 129:
      sections3(payload);
      break;
    case 130:
      sections4(payload);
      break;
    case 131:
      sectionsFull(payload);
      break;
    case 132:
      sectionsRandom1(payload);
      break;
    case 133:
      sectionsRandom2(payload);
      break;
  }
}

void fillEffect(DMXPayload payload) {
  uint16_t multiplier = 150 < num_leds_in_strip ? 2 : 1;
  int middle = num_leds_in_strip / 2;

  switch(payload.effect_id) {
    case 40: // FILL FROM START
      loopFromToColour(0, multiplier * (uint16_t) payload.step, CRGB(payload.r, payload.g,payload.b));
      loopFromToColour(multiplier * (uint16_t) payload.step, num_leds_in_strip, CRGB::Black);
      break;

    case 41: // FILL FROM END
      loopFromToColour(0, multiplier * (uint16_t) payload.step, CRGB::Black);
      loopFromToColour(multiplier * (uint16_t) payload.step, num_leds_in_strip, CRGB(payload.r, payload.g,payload.b));
      break;

    case 42: // FILL FROM BOTH
      loopFromToColour(payload.step, num_leds_in_strip - payload.step, CRGB::Black);
      loopFromToColour(0, payload.step, CRGB(payload.r, payload.g,payload.b));
      loopFromToColour(num_leds_in_strip - payload.step, num_leds_in_strip, CRGB(payload.r, payload.g,payload.b));
      break;

    case 43: // FILL FROM MIDDLE
      loopFromToColour(0, middle - payload.step, CRGB::Black);
      loopFromToColour(middle - payload.step, middle + payload.step, CRGB(payload.r, payload.g,payload.b));
      loopFromToColour(middle + payload.step, num_leds_in_strip, CRGB::Black);
      break;
    
    case 44: // OVERWRITE FILL FROM START
      loopFromToColour(0, multiplier * (uint16_t) payload.step, CRGB(payload.r, payload.g,payload.b));
      break;

    case 45: // OVERWRITE FILL FROM END
      loopFromToColour(multiplier * (uint16_t) payload.step, num_leds_in_strip, CRGB(payload.r, payload.g,payload.b));
      break;

    case 46: // OVERWRITE FILL FROM BOTH
      loopFromToColour(0, payload.step, CRGB(payload.r, payload.g,payload.b));
      loopFromToColour(num_leds_in_strip - payload.step, num_leds_in_strip, CRGB(payload.r, payload.g,payload.b));
      break;

    case 47: // OVERWRITE FILL FROM MIDDLE
      loopFromToColour(middle - payload.step, middle + payload.step, CRGB(payload.r, payload.g,payload.b));
      break;
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

void customEffect() {
  CRGB* newLeds = impl->customEffect();
    for (int i = 0; i < num_leds_in_strip; i++) {
      leds[i] = newLeds[i];
    }
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
    case WAVE:
      wave((payload.effect_id - 57) % 4 + 1, payload.effect_id < 61 ? 1 : -1);
      break;
    case RAINBOW:
      rainbow((payload.effect_id - 65) % 4 + 1);
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
    setOneColour(CRGB(payload.r, payload.g, payload.b));
  } else {
    setOneColour(CRGB::Black);
  }
}

/**
 * Updates wave in accordance to the wave and tick. Continuous updates creates a wave effect.
 * @param length starts from 1. The higher number, the shorter the wave.
 * @param multiplier Add positive number to increase and reverse speed of wave.
 */
void wave(uint8_t length, uint8_t multiplier) {
  uint8_t reducedBPM = payload.bpm / 30; // divided by 30, to make effect more stable!
  float speed_multiplier = (((float) reducedBPM) / 2) * multiplier;
  uint16_t wavelength = 512 / length;
  uint16_t half_wavelength = wavelength / 2;

  for (uint16_t i = 0; i < num_leds_in_strip; i++) {
    uint16_t count = ((uint16_t) (i + tick * speed_multiplier)) % wavelength;
    if(half_wavelength < count) {
      count = half_wavelength - count;
    }

    float wave = (float) count / half_wavelength;
    leds[i] = CRGB(payload.r * wave, payload.g * wave, payload.b * wave);
  }
}

void rainbow(uint8_t length) {
  uint8_t speed_multiplier = payload.bpm / 30; // divided by 30, to make effect more stable!
  
  for (int i = 0; i < num_leds_in_strip; i++) {
    leds[i] = CHSV(i - ((tick * speed_multiplier) % 255 * length), 255, 255); /* The higher the value 4 the less fade there is and vice versa */ 
  }
}

void sections2(DMXPayload payload) {
  for (long i = 0; i < sections.noOfSections; i++) {
    setSection(i, (i + payload.step) % 2 == 0 ? CRGB(payload.r, payload.g, payload.b) : CRGB::Black);
  }
}

void sections3(DMXPayload payload) {
  for (long i = 0; i < sections.noOfSections; i++) {
    setSection(i, (i + payload.step) % 3 == 0 ? CRGB(payload.r, payload.g, payload.b) : CRGB::Black);
  }
}

void sections4(DMXPayload payload) {
  for (long i = 0; i < sections.noOfSections; i++) {
    setSection(i, (i + payload.step) % 4 == 0 ? CRGB(payload.r, payload.g, payload.b) : CRGB::Black);
  }
}

void sectionsFull(DMXPayload payload) {
  for (long i = 0; i < sections.noOfSections; i++) {
    setSection(i, (i + payload.step) % sections.noOfSections == 0 ? CRGB(payload.r, payload.g, payload.b) : CRGB::Black);
  }
}

// Random numbers between 0-63
byte psudoRandom[] = {38, 33, 31, 45, 2, 56, 15, 44, 33, 13, 32, 37, 39, 52, 8, 6, 25, 15, 16, 17, 14, 31, 33, 63, 62, 56, 61, 44, 60, 45, 33, 17, 22, 55, 63, 51, 0, 11, 54, 12, 37, 54, 55, 37, 33, 39, 58, 20, 54, 63, 5, 12, 51, 57, 51, 58, 29, 58, 17, 7, 56, 36, 22, 51};

void sectionsRandom1(DMXPayload payload) {
  for (int i = 0; i < sections.noOfSections; i++) {
    setOneColour(CRGB::Black);

    uint8_t section_id = psudoRandom[(payload.step) % 64];
    setSection(section_id, CRGB(payload.r, payload.g, payload.b));
  }
}

void sectionsRandom2(DMXPayload payload) {
  for (int i = 0; i < sections.noOfSections; i++) {
    setOneColour(CRGB::Black);

    uint8_t section_id1 = psudoRandom[(payload.step) % 64];
    uint8_t section_id2 = psudoRandom[(payload.step + 32) % 64];
    setSection(section_id1, CRGB(payload.r, payload.g, payload.b));
    setSection(section_id2, CRGB(payload.r, payload.g, payload.b));
  }
}

void setSection(uint8_t i, CRGB color) {
  uint16_t start = sections.sectionsStartIndex[i];
  uint16_t end = sections.noOfSections == i + 1 ? num_leds_in_strip : sections.sectionsStartIndex[i + 1];
  loopFromToColour(start, end, color);
}
