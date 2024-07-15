#include <Arduino.h>
#include <RF24.h>
#include <FastLED.h>

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


#define id impl->getID()
#define full_leds 150
#define num_leds 60
#define num_channels num_leds*3 + 2 // 1 master dimmer + 1 local effect


#define LED_PIN  9 
#define built_in_led 8
CRGB leds[full_leds];

bool split = false;
// if brightnessFlag is true, the brightness is set at somepoint in the current loop.
// if false, the brightness is set with the dmx packet.
// often used to hardcode brightness to a value in a special program.
bool brightnessFlag = false;
uint8_t dmx[num_channels];
uint8_t data[32];
uint8_t fragment[6][32];


// put function declarations here:
int recvData(); // Declare the recvData function
void setOneColour(const CRGB &colour);
void normalMode();
void setLEDs();
void setDMX();

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
    }  // hold in infinite loop
  }
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, full_leds);

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
    
    setDMX();
  }
}

void setDMX(){
  //dmx[0] master dimmer
  //dmx[1] master effect
  //dmx[2] LED index
  //dmx[3] fixture A effect
  //dmx[4] fixture B effect
  //dmx[5] fixture C effect
  //...


  FastLED.setBrightness(dmx[0]);
  
  //Master effect implementation here, Might remove if not needed.

  if (dmx[id] == 1) digitalWrite(built_in_led, LOW);
  else digitalWrite(built_in_led, HIGH);
  if (dmx[3]== 2) 
  // All Red
    for (int i = 0; i < 150; i++) {
        leds[i] = CRGB::Red;
    }
    debug("All Red\n",0);
  impl->localEffect(leds, dmx[2], dmx[id]);
  FastLED.show();
  

}



