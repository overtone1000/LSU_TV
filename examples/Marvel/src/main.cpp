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

unsigned long current_time=0;
unsigned long last_time=0;


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
  //mp3.showDebug(1);
  mp3.begin(9600);
  delay(500);
  mp3.sendCommand(CMD_SEL_DEV,0,2);
  delay(20);
  mp3.setVol(BASELINE_VOLUME);

  current_time = millis();
  last_time=current_time;
  Serial.println("fin");
}

LEDGraphics::Wave slow_half(0.2,NUM_LEDS*2,0.1);
LEDGraphics::Wave med_half(0.49,NUM_LEDS,0.1);
LEDGraphics::Wave fast_third(1.115137998,NUM_LEDS/2,0.1);

//8 beats, ends at 23285, starts at 16111. Thats 8/(23.285-16.111) Hz = 1.115137998

LEDGraphics::Hill sparklewave(10000,0.5,SPARKLE_STEP,NUM_LEDS);

LEDGraphics::AddBrush blue(CRGB::Blue,1.0f);
LEDGraphics::BlendBrush orange(CRGB::OrangeRed,1.0f);
LEDGraphics::BlendBrush firebrush(CRGB::OrangeRed,1.0f);

LEDGraphics::AddBrush specialwhite(CRGB::DarkOrange,1.0f);
LEDGraphics::BlendBrush specialblue(CRGB::Blue,1.0f);

//CRGB background(0,0,5);
CRGB background(0,0,0);

bool prior_left_state = true;
bool prior_right_state = true;

bool current_left_state = true;
bool current_right_state = true;

unsigned long left_charge_start=0;
unsigned long right_charge_start=0;

unsigned long left_blast_start=0;
unsigned long right_blast_start=0;

enum Mode
{
  Resting,
  Blasting,
  Charging,
  DoubleCharging,
  Special
};
String mode_debug[] = {
  "Resting",
  "Blasting",
  "Charging",
  "DoubleCharging",
  "Special"
};
String ModeString(Mode mode){return (String)(mode_debug[mode]);}

Mode previous_mode=Resting;
Mode current_mode=Resting;

unsigned long time_current_mode_started=0;
const unsigned long theme_length=33019;
const unsigned long blast_length=1000; //4885;

unsigned long special_start=0;
const unsigned long special_time=5000;

bool watch_for_next_mode=false;

unsigned long charge_wave_time=0;

bool set_mode(Mode new_mode)
{
  //Don't change current mode if one of the timed modes is still running
  switch(current_mode)
  {
    case Blasting:
    {
      if((time_current_mode_started+blast_length)>current_time)
      {
        Serial.println("Mode not changed. Still blasting." + (String)time_current_mode_started + "|" + (String)blast_length + "|" + (String)current_time);
        return false;
      }
    }
    break;
    case Special:
    {
      if((time_current_mode_started+theme_length)>current_time)
      {
        Serial.println("Mode not changed. Still specialing." + (String)time_current_mode_started + "|" + (String)theme_length + "|" + (String)current_time);
        return false;
      }
    }
    default:
    break;
  }

  Serial.println();
  Serial.print("Mode now ");
  Serial.print(ModeString(new_mode));
  Serial.print(" at ");
  Serial.print(current_time);
  Serial.println();

  if(new_mode!=current_mode)
  {
    if(new_mode==DoubleCharging)
    {
      Serial.println("Vol 1.5x");
      mp3.setVol(BASELINE_VOLUME*1.5);
      delay(20);

      special_start=current_time;
      Serial.println("Starting special at " + (String)special_start);
    }
    if(new_mode==Blasting)
    {
      watch_for_next_mode=true;
    }
    if(new_mode==Special)
    {
      watch_for_next_mode=true;
      Serial.println("Vol 2x");
      mp3.setVol(BASELINE_VOLUME*2);
      delay(20);
    }
    else if(current_mode==DoubleCharging || current_mode==Special)
    {
      Serial.println("Vol 1x");
      mp3.setVol(BASELINE_VOLUME);
      delay(20);
    }
  }
  current_mode=new_mode;
  time_current_mode_started=current_time;

  return true;
}

bool button_change()
{
  bool retval=false;

  if(current_right_state && current_right_state!=prior_right_state)
  {
    retval = set_mode(Blasting);
  }
  else if(current_left_state && current_left_state!=prior_left_state)
  {
    retval = set_mode(Blasting);
  }
  else if(current_right_state != current_left_state)
  {
    retval = set_mode(Charging);
  }
  else if(!current_right_state && !current_left_state)
  {
    retval = set_mode(DoubleCharging);
  }
  else
  {
    retval = set_mode(Resting);
  }

  if(current_left_state!=prior_left_state)
  {
    prior_left_state=current_left_state;
    if(!current_left_state)
    {
      left_charge_start = current_time;  
    }    
    else
    {
      left_blast_start = current_time;  
    }
  }
  if(current_right_state!=prior_right_state)
  {
    prior_right_state=current_right_state;
    if(!current_right_state)
    {
      right_charge_start = current_time;  
    }    
    else
    {
      right_charge_start = current_time;  
    }
  }

  return retval;
}

void check_next_mode()
{
  if(watch_for_next_mode){watch_for_next_mode=!button_change();}
}

void handle_mode()
{
  check_next_mode();
  switch(current_mode)
  {
    case Blasting:
    {
      if(current_mode!=previous_mode)
      {
        Serial.println("Playing blast.");
        mp3.play(BLAST);
        delay(20);
      }
    }
    break;
    case DoubleCharging:
    {
      if(special_start+special_time<current_time)
      {
        set_mode(Special);
        handle_mode();
        return;
      }
    }
    //break; //Cascade to charging
    case Charging:
    {
      if(previous_mode!=Charging && previous_mode!=DoubleCharging)
      {
        Serial.println("Playing charging.");
        mp3.play(CHARGING);
        charge_wave_time=current_time;
        delay(20);
      }
    }
    break;
    case Special:
    {
      if(current_mode!=previous_mode)
      {
        Serial.println("Playing Theme.");
        mp3.play(AVENGERS_THEME);
        delay(20);
      }
    }
    break;
    default:break;
  }

  previous_mode=current_mode;
}


void process_input()
{
  current_left_state = digitalRead(BUTTON_LEFT);
  current_right_state = digitalRead(BUTTON_RIGHT);

  if(current_left_state!=prior_left_state || current_right_state!=prior_right_state)
  {
    button_change();
  }  
}

const float charge_freq = 0.5; //Hz
const unsigned long ms_per_wave = 1000/charge_freq;

void chargepaint(CRGB* leds, unsigned long ramp_time)
{
  //Left is pushed
  float final_magnitude=(float)(current_time-ramp_time)/(float)FIRE_FADE_IN_MILLIS;
  if(final_magnitude>1.0f){final_magnitude=1.0f;}
  
  float wave_multiplier = (((float)cos8((current_time-charge_wave_time)%(ms_per_wave)*MAX_BYTE/ms_per_wave))/((float)MAX_BYTE)+1.0f)/2.0f;
  final_magnitude*=wave_multiplier;

  Serial.println((String)current_time + ", " + (String)charge_wave_time + " - " + (String)wave_multiplier + "|" + (String)final_magnitude);

  firebrush.SetMagnitude(final_magnitude);
  for(int n=0;n<NUM_LEDS;n++)
  {
    firebrush.paint(leds+n);
  }
}

void pattern1paint(CRGB* leds, LEDGraphics::MagnitudeBrush* wave, LEDGraphics::MagnitudeBrush* sparkle)
{
  slow_half.Paint(current_time, left_forward, wave);
  slow_half.Paint(current_time, right_forward, wave);

  med_half.Paint(current_time, left_backward, wave);
  med_half.Paint(current_time, right_backward, wave);

  sparklewave.Paint(current_time, left_sparkles, sparkle);
  sparklewave.Paint(current_time, right_sparkles, sparkle);
}

void pattern2paint(CRGB* leds, LEDGraphics::MagnitudeBrush* wave, LEDGraphics::MagnitudeBrush* counterwave)
{
  slow_half.Paint(current_time, left_forward, wave);
  slow_half.Paint(current_time, right_forward, wave);

  med_half.Paint(current_time, left_backward, counterwave);
  med_half.Paint(current_time, right_backward, counterwave);

  fast_third.Paint(current_time, left_forward, wave);
  fast_third.Paint(current_time, left_forward, wave);
}

void conditional_paint()
{
  pattern1paint(leds_left,&blue,&orange);
  pattern1paint(leds_right,&blue,&orange);

  switch(current_mode)
  {
    case Blasting:
      if(current_time<left_blast_start+blast_length)
      {
        for(int n=0;n<NUM_LEDS;n++)
        {
          leds_left[n] = CRGB::Red;
        }
      }
      if(current_time<right_blast_start+blast_length)
      {
        for(int n=0;n<NUM_LEDS;n++)
        {
          leds_right[n] = CRGB::Red;
        }
      }
    case DoubleCharging:
    case Charging:
    {
      if(!current_left_state)
      {
        chargepaint(leds_left,left_charge_start);
      }
      if(!current_right_state)
      {
        chargepaint(leds_right,right_charge_start);
      }
    }
    break;
    case Special:
    {
      for(int n=0;n<NUM_LEDS;n++)
      {
        leds_left[n] = background;
        leds_right[n] = background;
      }
      pattern2paint(leds_left,&specialwhite,&specialblue);
      pattern2paint(leds_right,&specialwhite,&specialblue);
    }
    break;
    default:break;
  }
}

void loop() {
  //mp3.play(AVENGERS_THEME); //Play once
  //mp3.play(BLAST); //Play once

  //Serial.println("Starting loop at " + (String)current_time);

  if(current_time<FADE_IN_MILLIS)
  {
    uint8_t brightness = (float)FINAL_BRIGHTNESS*(float)current_time/(float)FADE_IN_MILLIS;
    Serial.println("Final brightness is " + (String)brightness);
    FastLED.setBrightness(brightness);
  }
  else
  {
    if(current_mode==Special)
    {
      FastLED.setBrightness(FINAL_BRIGHTNESS*2);
    }
    else
    {
      FastLED.setBrightness(FINAL_BRIGHTNESS);
    }
  }

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds_left[n] = background;
    leds_right[n] = background;
  }

  

  process_input();
  handle_mode();
  conditional_paint();
  
  FastLED.show();

  last_time=current_time;
  while(current_time<last_time+LOOP_DELAY)
  {
    current_time = millis();
    yield();
  }
}