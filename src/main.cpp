#include <Arduino.h>
#include <RF24.h>
#include <FastLED.h>

#define CE_PIN 0
#define CSN_PIN 10
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = { "1Node", "2Node" };

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(built_in_led, OUTPUT);
  digitalWrite(built_in_led, HIGH); //Led is reversed. Low is on and high is off.
  delay(1000);

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
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
  //printf("start of recvData\n");
  uint8_t pipe;
  if ( radio.available(&pipe) ) {
    digitalWrite(built_in_led, LOW);
    //printf("radio available\n");
    uint8_t bytes = radio.getPayloadSize();
    radio.read(&data, bytes);
    digitalWrite(built_in_led, HIGH);
      
    // check fragment number
    printf("Fragment number: %d\n", data[0]);
    uint8_t offset = data[0] * 31;
    //printf("Fragment number: %d\n", data[0]);
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

void setLEDs()
{
  if (dmx[1]== 0) // normal mode, effect is off
  {
    normalMode();
  }
  else if (dmx[1] == 1)// Red-Blue effect 1
  { 
    CRGB colour = CRGB::Red;
    if (split) CRGB::Blue;
    setOneColour(colour);
  }
  else if (dmx[1] == 2) //Red-Blue effect 2
  {
    CRGB colour = CRGB::Blue;
    if (split) CRGB::Red;
    setOneColour(colour);
  }
  else if (dmx[1]== 3) //side to side, off - on
  {
    brightnessFlag = true;
    if (split) FastLED.setBrightness(0);
    else FastLED.setBrightness(dmx[0]);
    normalMode();
  }
  else if (dmx[1]== 4) //side to side, off - on
  {
    brightnessFlag = true;
    if (!split) FastLED.setBrightness(0);
    else FastLED.setBrightness(dmx[0]);
    normalMode();
  }

  if (!brightnessFlag) FastLED.setBrightness(dmx[0]);
  brightnessFlag = false;
  FastLED.show();
}

void setOneColour(const CRGB &colour)
{
  for (int i = 0; i < num_leds; i++)
  {
    leds[i] = colour;
  }
}

void normalMode()
{
  if (full_leds == num_leds) {
    for (int i = 0; i < num_leds; i++)
    {
      leds[i].setRGB(dmx[i * 3 + 2], dmx[i * 3 + 3], dmx[i * 3 + 4]);
    }
  }
  else {
    // expand dmx data to fill all leds 
    int dividor = full_leds / num_leds * 10;
    int j = 0;
    int k = 0;

    for (int i = 0; i < num_leds; i++)
    {
      while (k < dividor) {
        leds[j].setRGB(dmx[i * 3 + 2], dmx[i * 3 + 3], dmx[i * 3 + 4]);
        j++;
        k+= 10;
      }
      if (k > dividor) {
        // set the next led to the same colour as the previous one
        leds[j].setRGB(dmx[i * 3 + 2], dmx[i * 3 + 3], dmx[i * 3 + 4]);
        j++;
      }
      k -= dividor;      
    }
  }
}

void loop() {
  //printf("start of loop\n");
  
  if(recvData() )
  {
    
    
    printf("Data received:\n");
    for (int i = 0; i < 32; i++)
    {
      printf("%d ", data[i]);
    }
    printf("\n");
    
    setLEDs();
  }
}



