#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "LEDGraphics.h"

// How many leds are in the strip?
#define NUM_LEDS 200

// Data pin that led data will be written out over
#define DATA_PIN 14

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
//LEDGraphics::LEDSet2D ledset_1(leds,NUM_LEDS,0,15,false);
//LEDGraphics::LEDSet2D ledset_2(leds,NUM_LEDS,30,16,true);
LEDGraphics::LEDSet2D ledset(leds,NUM_LEDS,0,NUM_LEDS-1,false);
LEDGraphics::LEDSet2D ledset_2(leds,NUM_LEDS,NUM_LEDS-1,0,true);

// This function sets up the ledsand tells the controller about them

int current_led=0;
const int interval=500;
int last_time=0;

void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(74880); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  Serial.println("Adding LEDs");
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(60);

  last_time=millis();
}


void loop() {

  unsigned long current_time = millis();

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds[n] = CRGB::Black;
  }

  if(current_time>last_time+interval)
  {
    last_time=current_time;
    les[current_led++]=CRGB::White;
  }

  FastLED.show();
  delay(100);
}
