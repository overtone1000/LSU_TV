#include <LEDGraphics.h>

#define START_DELAY 2000
#define LOOP_DELAY 50
#define SPARKLE_STEP 3
// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds_left[NUM_LEDS];
CRGB leds_right[NUM_LEDS];
CRGB onboard_led[1];

LEDGraphics::LEDSet2D left_forward(leds_left,NUM_LEDS,0,NUM_LEDS,false);
LEDGraphics::LEDSet2D left_backward(leds_left,NUM_LEDS,NUM_LEDS,0,true);

LEDGraphics::LEDSet2D right_forward(leds_right,NUM_LEDS,0,NUM_LEDS,false);
LEDGraphics::LEDSet2D right_backward(leds_right,NUM_LEDS,NUM_LEDS,0,true);

LEDGraphics::LEDSet2D onboard(onboard_led,1,0,0,true);

LEDGraphics::LEDSet2D* left_sparkles;
LEDGraphics::LEDSet2D* right_sparkles;

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(START_DELAY);
  Serial.begin(9600); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  Serial.println("Starting setup.");

  //pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812, DATA_PIN_LEFT, GRB>(leds_left, NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_PIN_RIGHT, GRB>(leds_right, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, ONBOARD_NEOPIXEL>(onboard_led, 1);
  FastLED.setBrightness(100);

  CRGB* left_sparkle_arr[NUM_LEDS];
  CRGB* right_sparkle_arr[NUM_LEDS];
  int i = 0;
  int step = NUM_LEDS/SPARKLE_STEP;
  int wrap_count = 0;

  Serial.println("Populating sparkle arrays.");
  for(int n=0;n<NUM_LEDS;n++)
  {
    left_sparkle_arr[n] = leds_left+i;
    right_sparkle_arr[n] = leds_right+i;

    i+=step;
    if(i>=NUM_LEDS)
    {
      wrap_count++;
      i=wrap_count;
    }
  }

  Serial.println("Creating sparkle LEDSet2Ds.");
  left_sparkles=new LEDGraphics::LEDSet2D(left_sparkle_arr,NUM_LEDS);
  right_sparkles=new LEDGraphics::LEDSet2D(right_sparkle_arr,NUM_LEDS);

  Serial.println("Finished setup.");
}

LEDGraphics::Wave slow_half(START_DELAY,NUM_LEDS/20,NUM_LEDS/2,NUM_LEDS);
LEDGraphics::Wave med_half(START_DELAY,NUM_LEDS/10,NUM_LEDS/2,NUM_LEDS);
LEDGraphics::Wave med_third(START_DELAY,NUM_LEDS/10,NUM_LEDS/3,NUM_LEDS);

LEDGraphics::Wave sparklewave(START_DELAY,NUM_LEDS/20,SPARKLE_STEP,NUM_LEDS);

LEDGraphics::AddBrush blue(CRGB::Blue,0.1f);
LEDGraphics::BlendBrush orange(CRGB::Orange,1.0f);
CRGB background(0,0,5);

void loop() {
  unsigned long current_time = millis();
  Serial.println("Starting loop at " + (String)current_time);

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds_left[n] = background;
    leds_right[n] = background;
  }

  //onboard_led[0] = CRGB::Blue; //This works
  onboard_led[0] = background; //This works

  slow_half.Paint(current_time, &left_forward, &blue);
  slow_half.Paint(current_time, &right_forward, &blue);

  med_half.Paint(current_time, &left_backward, &blue);
  med_half.Paint(current_time, &right_backward, &blue);

  med_third.Paint(current_time, &left_forward, &blue);
  med_third.Paint(current_time, &right_forward, &blue);

  sparklewave.Paint(current_time, left_sparkles, &orange);
  sparklewave.Paint(current_time, right_sparkles, &orange);

  FastLED.show();
  delay(LOOP_DELAY);
}
