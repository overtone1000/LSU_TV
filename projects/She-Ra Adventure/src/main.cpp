#include <ESP8266WiFi.h>
#include <fauxmoESP.h>
#include <WiFiCredentials.h>

fauxmoESP fauxmo;

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

LEDGraphics::LEDSet2D* inside;

LEDGraphics::LEDSet2D* outside[4];

String dev_intruders = "She-Ra Intruders";
String dev_glow = "She-Ra Victory Glow";
String dev_prize = "She-Ra Prize";
String dev_party ="She-Ra Party";

enum Mode
{
  victory_glow=1,
  take_the_prize=2,
  intruders=3,
  party=4
};

Mode mode = intruders;
bool showleds = true;

unsigned long last_mode_change_time=0;
const float ramp_time=2000.0f;


// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(74880); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen
  
  inside = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,0,48,false);

  outside[0] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,53,89,false);
  outside[1] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,126,90,true);
  outside[2] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,127,162,false);
  outside[3] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,197,163,true);

  Serial.println("Starting WiFi for " + (String)SSID);
  WiFi.begin(SSID, WFPASS);

  //pinMode(DATA_PIN, OUTPUT);
  Serial.println("Adding LEDs");
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BYTE);
  

  Serial.println("Configuring Fauxmo");
  fauxmo.addDevice(dev_glow.c_str());
  fauxmo.addDevice(dev_prize.c_str());
  fauxmo.addDevice(dev_intruders.c_str());
  fauxmo.addDevice(dev_party.c_str());

  fauxmo.setPort(80); // required for gen3 devices
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
      String thisdev(device_name);
      showleds=state;
      String modestr = (String)device_name;
      if(modestr.equals(dev_glow))
      {
        mode=Mode::victory_glow;
      }
      else if(modestr.equals(dev_prize))
      {
        mode=Mode::take_the_prize;
      }
      else if(modestr.equals(dev_intruders))
      {
        mode=Mode::intruders;
      }
      else if(modestr.equals(dev_party))
      {
        mode=Mode::party;
      }
      Serial.println("Device " + modestr + " is " + (String)state);
      last_mode_change_time=millis();
  });
}

LEDGraphics::Glow outer_glow0(0.25, 0.0, 0.0, 1.0);
LEDGraphics::Glow outer_glow1(0.25, 0.25, 0.0, 1.0);
LEDGraphics::Glow outer_glow2(0.25, 0.5, 0.0, 1.0);
LEDGraphics::Glow outer_glow3(0.25, 0.75, 0.0, 1.0);

LEDGraphics::Glow inner_glow(0.1, 0, 0.2, 1.0);

LEDGraphics::BlendBrush blue(CRGB::Blue, 1.0f);
LEDGraphics::BlendBrush white(CRGB::White, 1.0f);
LEDGraphics::BlendBrush red(CRGB::Red, 1.0f);


void loop() {
  fauxmo.handle();

  unsigned long current_time = millis();

  float ramp = ((float)(current_time-last_mode_change_time))/ramp_time;
  if(ramp>1.0f){ramp=1.0f;}
  FastLED.setBrightness(MAX_BYTE*ramp);
  Serial.println("Brightness is " + (String)(MAX_BYTE*ramp));

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds[n] = CRGB::Black;
  }

  //byte magn = (current_time%1000)/1000.0/2.0*MAX_BYTE;
  //Serial.println(magn);
  //for(int n=0;n<NUM_LEDS;n++)
  //{
  //  leds[n].r = magn;
  //}
  
  if(showleds)
  {
    switch(mode)
    {
      case Mode::victory_glow:
        outer_glow0.Paint(current_time,outside[0],&blue);
        outer_glow1.Paint(current_time,outside[1],&blue);
        outer_glow2.Paint(current_time,outside[2],&blue);
        outer_glow3.Paint(current_time,outside[3],&blue);
        break;
      case Mode::take_the_prize:
        outer_glow0.Paint(current_time,outside[0],&blue);
        outer_glow1.Paint(current_time,outside[1],&blue);
        outer_glow2.Paint(current_time,outside[2],&blue);
        outer_glow3.Paint(current_time,outside[3],&blue);
        inner_glow.Paint(current_time,inside,&white);
        break;
      case Mode::intruders:
        outer_glow0.Paint(current_time,outside[0],&red);
        outer_glow1.Paint(current_time,outside[1],&red);
        outer_glow2.Paint(current_time,outside[2],&red);
        outer_glow3.Paint(current_time,outside[3],&red);
        break;
      case Mode::party:
        break;
    }


    //ledset.paint_wave(current_time,start_millis,wave_start_mid,wave_speed,wave_width,CRGB::Yellow);
  }

  FastLED.show();
  //delay(100);
}
