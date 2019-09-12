#include <LEDGraphics.h>

// How many leds are in the strip?
#define NUM_LEDS 20

// Data pin that led data will be written out over
#define DATA_PIN 14
#define ONBOARD_NEOPIXEL 8

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
LEDGraphics::LEDSet2D all_forward(leds,NUM_LEDS,0,NUM_LEDS,false);
LEDGraphics::LEDSet2D all_backward(leds,NUM_LEDS,NUM_LEDS,0,true);

CRGB onboard_led[1];
LEDGraphics::LEDSet2D onboard(onboard_led,NUM_LEDS,NUM_LEDS,0,true);

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(9600); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  //pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, ONBOARD_NEOPIXEL>(onboard_led, 1);
  FastLED.setBrightness(60);
}

LEDGraphics::Wave slow_full(5000,4,NUM_LEDS,((float)NUM_LEDS));

LEDGraphics::BlendBrush blue(CRGB::Blue,1.0f);

void loop() {
  unsigned long current_time = millis();
  Serial.println("Starting loop at " + (String)current_time);

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds[n] = CRGB::Black;
  }

  onboard_led[0] = CRGB::Blue; //This works
  //onboard_led[0] = CRGB::Black; //This works

  slow_full.Paint(current_time, &onboard, &blue);

  FastLED.show();
  delay(100);
}
