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

LEDGraphics::LEDSet2D* outside_forward[4];
LEDGraphics::LEDSet2D* outside_backward[4];
boolean outside_is_forward[4];
LEDGraphics::LEDSet2D* inside;

LEDGraphics::Glow* outer_glow[4];
LEDGraphics::Glow* inner_glow;

LEDGraphics::Glow* party_glow;

LEDGraphics::Hill* outer_wave[4];
LEDGraphics::Wave* inner_wave;

LEDGraphics::BlendBrush* partybrushes[4];

#define DEVICE_COUNT 4
const char* dev_intruders = "SR Intruders";
const char* dev_glow = "SR Victory Glow";
const char* dev_prize = "SR Prize";
const char* dev_party ="SR Party";
const char* devs[] = {dev_intruders, dev_glow, dev_prize, dev_party};

enum Mode
{
  intruders,
  victory_glow,
  take_the_prize,
  party
};

Mode mode = intruders;
bool showleds = false;

unsigned long last_mode_change_time=0;
float ramp_time=2000.0f;


// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(74880); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen
  
  inside = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,0,48,false);

  outside_forward[0] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,53,89,false);
  outside_forward[1] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,126,90,true);
  outside_forward[2] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,127,162,false);
  outside_forward[3] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,197,163,true);

  outside_backward[0] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,89,53,true);
  outside_backward[1] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,90,126,false);
  outside_backward[2] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,162,127,true);
  outside_backward[3] = new LEDGraphics::LEDSet2D(leds,NUM_LEDS,163,197,false);

  for(int n=0;n<4;n++)
  {
    outside_is_forward[n]=true;
  }

  inner_glow = new LEDGraphics::Glow(0.1, 0, 0.1, 0.8);
  outer_glow[0] = new LEDGraphics::Glow(0.25, 0.75, 0.0, 0.8);
  outer_glow[1] = new LEDGraphics::Glow(0.25, 0.7, 0.0, 0.8);
  outer_glow[2] = new LEDGraphics::Glow(0.25, 0.25, 0.0, 0.8);
  outer_glow[3] = new LEDGraphics::Glow(0.25, 0, 0.0, 0.8);

  party_glow = new LEDGraphics::Glow(2.4, 0, 0, 1.0);

  inner_wave = new LEDGraphics::Wave(0.25, 5, 1.0);
  outer_wave[0] = new LEDGraphics::Hill(0.25, 5, 0, 50);
  outer_wave[1] = new LEDGraphics::Hill(0.5, 5, 0, 50);
  outer_wave[2] = new LEDGraphics::Hill(0.2, 5, 0, 50);
  outer_wave[3] = new LEDGraphics::Hill(0.7, 5, 0, 50);

  Serial.println("Starting WiFi for " + (String)SSID);
  WiFi.begin(SSID, WFPASS);
  

  //pinMode(DATA_PIN, OUTPUT);
  Serial.println("Adding LEDs");
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BYTE);
  

  Serial.println("Configuring Fauxmo");
  fauxmo.addDevice(dev_intruders);
  fauxmo.addDevice(dev_glow);
  fauxmo.addDevice(dev_prize);
  fauxmo.addDevice(dev_party);

  fauxmo.setPort(80); // required for gen3 devices
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
      String thisdev(device_name);
      showleds=state;
      
      Serial.println("Device " + thisdev + " bool is " + (String)state);

      if(state)
      {
        for(int n=0;n<DEVICE_COUNT;n++)
        {
          if(strcmp(devs[n],device_name)!=0)
          {
            Serial.println("Turning off " + (String)devs[n]);
            fauxmo.setState(devs[n],false,'0');
          }
          else
          {
            Serial.println("Turning on " + (String)devs[n]);
            mode = (Mode)n;
          }
        }

        switch(mode)
        {
          case Mode::intruders:
            ramp_time=2000.0f;
            break;
          case Mode::party:
            ramp_time=7000.0f;
            break;
          case Mode::take_the_prize:
            ramp_time=0.0f;
          case Mode::victory_glow:
            ramp_time=20000.0f;
            break;
          default:
            ramp_time=2000.0f;
            break;
        }
      }
      last_mode_change_time=millis();
  });

  // Wait for connection
  Serial.println("Waiting for WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected. IP is " + WiFi.localIP().toString());
}

LEDGraphics::BlendBrush blue(CRGB::Blue, 1.0f);
LEDGraphics::BlendBrush white(CRGB::White, 1.0f);
LEDGraphics::BlendBrush red(CRGB::Red, 1.0f);
LEDGraphics::BlendBrush gold(CRGB::Gold, 1.0f);

LEDGraphics::BlendBrush randombrush(CRGB::White, 1.0f);

void loop() {
  fauxmo.handle();

  unsigned long current_time = millis();

  float ramp = ((float)(current_time-last_mode_change_time))/ramp_time;
  if(ramp>1.0f){ramp=1.0f;}
  //else(Serial.println("Ramping at " + (String)(MAX_BYTE*ramp)));
  FastLED.setBrightness(MAX_BYTE*ramp);
  //Serial.println("Brightness is " + (String)(MAX_BYTE*ramp));

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
        for(int n=0;n<4;n++)
        {
          outer_glow[n]->UpdateAlong(current_time);
          outer_glow[n]->Paint(outside_forward[n],&gold);
        }
        break;
      case Mode::take_the_prize:
        for(int n=0;n<4;n++)
        {
          outer_glow[n]->UpdateAlong(current_time);
          outer_glow[n]->Paint(outside_forward[n],&blue);
        }
        inner_glow->UpdateAlong(current_time);
        inner_glow->Paint(inside,&white);
        break;
      case Mode::intruders:
        for(int n=0;n<4;n++)
        {
          outer_wave[n]->UpdateAlong(current_time);
          if(outer_wave[n]->Finished())
          {
            outside_is_forward[n]=!outside_is_forward[n];
          }
          if(outside_is_forward[n])
          {
            outer_wave[n]->Paint(outside_forward[n],&red);
          }
          else
          {
            outer_wave[n]->Paint(outside_backward[n],&red);
          }
        }
        break;
      case Mode::party:
        party_glow->UpdateAlong(current_time);
        if(party_glow->Finished())
        {
          randombrush.randomizeColor();
        }
        for(int n=0;n<4;n++)
        {
          party_glow->Paint(outside_forward[n],&randombrush);
        }
        break;
    }


    //ledset.paint_wave(current_time,start_millis,wave_start_mid,wave_speed,wave_width,CRGB::Yellow);
  }

  FastLED.show();
  //delay(100);
}
