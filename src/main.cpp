#include <Arduino.h>
#include "customImpl.h"

#include "generic.cpp"
#include "katapult_ekstra.cpp"

CustomImpl *impl = new Katapult();


#define DEBUG 0
#if DEBUG
#define debug(x,t) printf(x,t)
#else
#define debug(x,t)
#endif


#define CE_PIN 0
#define CSN_PIN 10

uint8_t address[][6] = { "1Node", "2Node" };


#define id impl->getId()
#define num_channels 180 // fix this
static const int timeout_millis = 60000;
static const int num_leds_in_strip = impl->getNumLeds();
static const LEDSections sections = impl->getLEDSections();
CRGB leds[300];

#define LED_PIN  7 
#define built_in_led 8
/**
 * DECLARATION OF FUNCTIONS
 */

long tick = 0; // Used to keep track of local animations

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(built_in_led, OUTPUT);
  digitalWrite(built_in_led, HIGH); //Led is reversed. Low is on and high is off.
  delay(1000);

  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, num_leds_in_strip);

  // BRG NEJ
  // BGR NEJ
  // RGB
  // RBG
  // GRB
  // GBR

  
  // WS2812B 4pin sorte full cover - BRG
  // WS2811 til hvid strip hvid tape - BRG
}


long lastAnimationMillis = 0;

void loop() {
  long currentMillis = millis();
  delay(1);
  if (currentMillis - lastAnimationMillis < 20) return;
  lastAnimationMillis = currentMillis;
  tick++;
  
  CRGB* newLeds = impl->customEffect(tick);
  for (int i = 0; i < num_leds_in_strip; i++) {
    leds[i] = newLeds[i];
  }
  FastLED.show();  
}