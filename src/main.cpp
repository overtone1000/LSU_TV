// Use if you want to force the software SPI subsystem to be used for some reason (generally, you don't)
// #define FASTLED_FORCE_SOFTWARE_SPI
// Use if you want to force non-accelerated pin access (hint: you really don't, it breaks lots of things)
// #define FASTLED_FORCE_SOFTWARE_SPI
// #define FASTLED_FORCE_SOFTWARE_PINS

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include <cmath>
///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds.  This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
//

// How many leds are in the strip?
#define NUM_LEDS 30
#define MAX_BYTE 255
// Data pin that led data will be written out over
#define DATA_PIN 14

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(74880); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  //pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
}

void paint_wave(unsigned long current_millis, unsigned long start_millis, float wave_speed, float wave_width, CRGB color)
{
  //wave_speed is LEDs per second
  //wave_width is LEDs
  float last_x = wave_speed*((float)(current_millis-start_millis))/1000.0F;
  float first_x = last_x-wave_width;

  int last_LED = floor(last_x);
  int first_LED = ceil(first_x);

  Serial.println((String)first_x + "|" + (String)last_x);
  Serial.println((String)first_LED + "|" + (String)last_LED);

  if(last_LED>NUM_LEDS){last_LED=NUM_LEDS;}
  if(first_LED<0){first_LED=0;}

  for(int n=first_LED;n<=last_LED;n++)
  {
    float x = ((float)n-first_x);
    float x_percent = x/wave_width;
    uint8_t xbyte = round(x_percent*MAX_BYTE);
    uint8_t dim_factor = cos8(xbyte);

    leds[n] = color;
    leds[n].fadeLightBy(dim_factor);
    //leds[n] = color; //This works perfectly, so the problem is with dimming.

    Serial.print((String)dim_factor + ", ");
  }
  Serial.println();
}

unsigned long start_millis=5000;
float wave_speed = 5;
float wave_width=NUM_LEDS;
int test=0;
void loop() {
  unsigned long current_time = millis();
  float passed = (current_time-start_millis)*wave_speed/1000.0F;
  float total = NUM_LEDS+wave_width;
  if(passed>total)
  {
    Serial.println((String)passed + "/" + (String)total);
    Serial.println("Restarting wave.");
    start_millis=current_time;
  }

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds[n] = CRGB::Black;
  }
  paint_wave(current_time,start_millis,wave_speed,wave_width,CRGB::Purple);

  FastLED.show();
  delay(100);
}
