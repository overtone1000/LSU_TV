#include <LEDGraphics.h>

#define START_DELAY 2000
#define LOOP_DELAY 50
#define SPARKLE_STEP 3

#define FADE_IN_MILLIS 10000
#define FINAL_BRIGHTNESS 100

#define FIRE_FADE_IN_MILLIS 2000

#include <SerialMP3Player.h>
#define CMD_SEL_DEV 0X09
#define MUSIC_FOLDER 0
#define AVENGERS_THEME 1
#define CHARGING 2
#define BLAST 3

SerialMP3Player mp3(RX_PIN,TX_PIN);

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds_left[NUM_LEDS];
CRGB leds_right[NUM_LEDS];
CRGB onboard_led[1];

LEDGraphics::LEDSet2D* left_forward;
LEDGraphics::LEDSet2D* left_backward;

LEDGraphics::LEDSet2D* right_forward;
LEDGraphics::LEDSet2D* right_backward;

LEDGraphics::LEDSet2D* onboard;

LEDGraphics::LEDSet2D* left_sparkles;
LEDGraphics::LEDSet2D* right_sparkles;

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(START_DELAY);
  Serial.begin(9600); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  Serial.println("1");
  
  //pinMode(DATA_PIN, OUTPUT);
  Serial.println("2");
  FastLED.addLeds<WS2812, DATA_PIN_LEFT, GRB>(leds_left, NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_PIN_RIGHT, GRB>(leds_right, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, ONBOARD_NEOPIXEL>(onboard_led, 1);
  FastLED.setBrightness(0);
  FastLED.show();

  Serial.println("3");
  CRGB* left_sparkle_arr[NUM_LEDS];
  CRGB* right_sparkle_arr[NUM_LEDS];
  int i = 0;
  int step = NUM_LEDS/SPARKLE_STEP;
  int wrap_count = 0;
  
  Serial.println("4");
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

  Serial.println("5");
  onboard_led[0] = CRGB(2,2,2);
  for(int n=0;n<NUM_LEDS;n++)
  {
    leds_left[n] = CRGB(0,0,0);
    leds_right[n] = CRGB(0,0,0);
  }
  FastLED.show();

  Serial.println("6");
  left_forward=new LEDGraphics::LEDSet2D(leds_left,NUM_LEDS,0,NUM_LEDS,false);
  left_backward=new LEDGraphics::LEDSet2D(leds_left,NUM_LEDS,NUM_LEDS,0,true);
  right_forward=new LEDGraphics::LEDSet2D(leds_right,NUM_LEDS,0,NUM_LEDS,false);
  right_backward=new LEDGraphics::LEDSet2D(leds_right,NUM_LEDS,NUM_LEDS,0,true);
  onboard=new LEDGraphics::LEDSet2D(onboard_led,1,0,0,true);
  
  Serial.println("7");
  left_sparkles=new LEDGraphics::LEDSet2D(left_sparkle_arr,NUM_LEDS);
  right_sparkles=new LEDGraphics::LEDSet2D(right_sparkle_arr,NUM_LEDS);

  left_forward->SetDebugName("LF");
  left_backward->SetDebugName("LB");
  right_forward->SetDebugName("RF");
  right_backward->SetDebugName("RB");
  onboard->SetDebugName("OB");
  left_sparkles->SetDebugName("LS");
  right_sparkles->SetDebugName("RS");

  Serial.println("8");
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  Serial.println("9");
  mp3.showDebug(1);
  mp3.begin(9600);
  delay(500);
  mp3.sendCommand(CMD_SEL_DEV,0,2);
  delay(20);
  mp3.setVol(BASELINE_VOLUME);

  Serial.println("fin");
}

LEDGraphics::Wave slow_half(0.2,NUM_LEDS*2,0.1);
LEDGraphics::Wave med_half(0.49,NUM_LEDS,0.1);
LEDGraphics::Wave fast_third(0.99,NUM_LEDS/2,0.1);

LEDGraphics::Hill sparklewave(10000,0.5,SPARKLE_STEP,NUM_LEDS);

LEDGraphics::AddBrush blue(CRGB::Blue,1.0f);
LEDGraphics::BlendBrush orange(CRGB::DarkOrange,1.0f);
LEDGraphics::BlendBrush firebrush(CRGB::DarkOrange,1.0f);

//CRGB background(0,0,5);
CRGB background(0,0,0);

bool prior_left_state = true;
bool prior_right_state = true;

bool current_left_state = true;
bool current_right_state = true;

unsigned long left_time=0;
unsigned long right_time=0;

unsigned long current_time=0;

enum Mode
{
  Resting,
  Blasting,
  Charging,
  Special
};
String mode_debug[] = {
  "Resting",
  "Blasting",
  "Charging",
  "Special"
};
String ModeString(Mode mode){return (String)(mode_debug[mode]);}

Mode previous_mode=Resting;
Mode current_mode=Resting;

bool previous_double_charging=false;
bool current_double_charging=false;

unsigned long time_current_mode_started=0;
const unsigned long theme_length=33019;
const unsigned long blast_length=4885;

unsigned long special_start=0;
const unsigned long special_time=5000;

void set_mode(Mode new_mode)
{
  //Don't change current mode if one of the timed modes is still running
  switch(current_mode)
  {
    case Blasting:
    {
      if(time_current_mode_started+blast_length<current_time)
      {
        return;
      }
    }
    break;
    case Special:
    {
      if(time_current_mode_started+theme_length<current_time)
      {
        return;
      }
    }
    default:
    break;
  }
  current_mode=new_mode;
  time_current_mode_started=millis();
  Serial.println();
  Serial.print("Mode set to ");
  Serial.print(ModeString(current_mode));
  Serial.print(" at ");
  Serial.print(time_current_mode_started);
  Serial.println();
}

void handle_mode(unsigned long current_time)
{
  switch(current_mode)
  {
    case Blasting:
    {
      if(current_mode!=previous_mode)
      {
        Serial.println("Playing blast.");
        mp3.play(BLAST);
      }
    }
    break;
    case Charging:
    {
      if(current_mode!=previous_mode)
      {
        Serial.println("Playing charging.");
        mp3.play(CHARGING);
      }
      if(special_start+special_time<current_time)
      {
        set_mode(Special);
        return;
      }
    }
    break;
    case Special:
    {
      if(current_mode!=previous_mode)
      {
        Serial.println("Playing Avengers theme.");
        mp3.play(AVENGERS_THEME);
      }
    }
    break;
  }

  previous_mode=current_mode;
}

void button_change(unsigned long current_time)
{
  current_double_charging=false;
  if(current_right_state && current_right_state!=prior_right_state)
  {
    set_mode(Blasting);
  }
  else if(current_left_state && current_left_state!=prior_left_state)
  {
    set_mode(Blasting);
  }
  else if(!current_right_state || !current_left_state)
  {
    set_mode(Charging);
    if(!current_right_state && !current_left_state)
    {
      current_double_charging=true;
    }
  }
  else
  {
    set_mode(Resting);
  }
  

  if(previous_double_charging!=current_double_charging)
  {
    if(current_double_charging)
    {
      Serial.println("Increasing volume.");
      mp3.setVol(BASELINE_VOLUME*1.5);

      Serial.println("Starting special countdown.");
      special_start=current_time;
    }
    else
    {
      Serial.println("Returning to normal volume.");
      mp3.setVol(BASELINE_VOLUME);
    }
    previous_double_charging=current_double_charging;
  }
}

void process_input(unsigned long current_time)
{
  current_left_state = digitalRead(BUTTON_LEFT);
  current_right_state = digitalRead(BUTTON_RIGHT);

  if(current_left_state!=prior_left_state || current_right_state!=prior_right_state)
  {
    button_change(current_time);
    if(current_left_state!=prior_left_state)
    {
      prior_left_state=current_left_state;
      left_time = current_time;  
    }
    if(current_right_state!=prior_right_state)
    {
      prior_right_state=current_right_state;
      right_time = current_time;
    }
  }  
}

void conditional_paint(unsigned long current_time)
{
  switch(current_mode)
  {
    case Blasting:
    {

    }
    //break; //don't break, fall through to Charging in case the other button is still pushed
    case Charging:
    {
      if(!current_left_state)
      {
        //Left is pushed
        float final_magnitude=(float)(current_time-left_time)/(float)FIRE_FADE_IN_MILLIS;
        if(final_magnitude>1.0f){final_magnitude=1.0f;}

        firebrush.SetMagnitude(final_magnitude);
        for(int n=0;n<NUM_LEDS;n++)
        {
          firebrush.paint(leds_left+n);
        }
      }
      
      if(!current_right_state)
      {
        //Right is pushed
        float final_magnitude=(float)(current_time-right_time)/(float)FIRE_FADE_IN_MILLIS;
        if(final_magnitude>1.0f){final_magnitude=1.0f;}
        
        firebrush.SetMagnitude(final_magnitude);
        for(int n=0;n<NUM_LEDS;n++)
        {
          firebrush.paint(leds_right+n);
        }
      }
    }
    break;
    case Special:
    {

    }
    break;
  }
}

void loop() {
  //mp3.play(AVENGERS_THEME); //Play once
  //mp3.play(BLAST); //Play once

  unsigned long current_time = millis();
  //Serial.println("Starting loop at " + (String)current_time);

  if(current_time<FADE_IN_MILLIS)
  {
    uint8_t brightness = (float)FINAL_BRIGHTNESS*(float)current_time/(float)FADE_IN_MILLIS;
    Serial.println("Final brightness is " + (String)brightness);
    FastLED.setBrightness(brightness);
  }

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds_left[n] = background;
    leds_right[n] = background;
  }

  slow_half.Paint(current_time, left_forward, &blue);
  slow_half.Paint(current_time, right_forward, &blue);

  med_half.Paint(current_time, left_backward, &blue);
  med_half.Paint(current_time, right_backward, &blue);

  sparklewave.Paint(current_time, left_sparkles, &orange);
  sparklewave.Paint(current_time, right_sparkles, &orange);

  process_input(current_time);
  handle_mode(current_time);
  conditional_paint(current_time);
  
  FastLED.show();
  delay(LOOP_DELAY);
}