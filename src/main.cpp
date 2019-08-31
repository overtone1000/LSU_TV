#include <ESP8266WiFi.h>
#include <fauxmoESP.h>
#include <WiFiCredentials.h>

fauxmoESP fauxmo;

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "LEDGraphics/LEDGraphics.h"

// How many leds are in the strip?
#define NUM_LEDS 30

// Data pin that led data will be written out over
#define DATA_PIN 14

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
LEDGraphics::LEDSet2D ledset_1(leds,NUM_LEDS,0,15,false);
LEDGraphics::LEDSet2D ledset_2(leds,NUM_LEDS,30,16,true);

String devname = "Go Tigers";

bool showleds = false;

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(74880); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  WiFi.begin(SSID, WFPASS);

  //pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(120);

  fauxmo.addDevice(devname.c_str());

  fauxmo.setPort(80); // required for gen3 devices
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
      String thisdev(device_name);
      if(thisdev.equals(devname))
      {
        Serial.println("Device " + devname + " is " + (String)state);
        showleds=state;
      }
  });
}


unsigned long start_millis=5000;
float wave_speed = 15;
float wave_width=NUM_LEDS/2;
float wave_start_mid = ((float)NUM_LEDS)/2.0F;
int test=0;

void loop() {
  fauxmo.handle();

  unsigned long current_time = millis();
  float passed = (current_time-start_millis)*wave_speed/1000.0F;
  float total = NUM_LEDS+wave_width;
  if(passed>total)
  {
    //Serial.println((String)passed + "/" + (String)total);
    //Serial.println("Restarting wave.");
    start_millis=current_time;
  }

  LEDGraphics::AddBrush redbrush(CRGB::Red);
  LEDGraphics::AddBrush bluebrush(CRGB::Blue);

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds[n] = CRGB::Black;
    redbrush.paint(leds+n);
    bluebrush.paint(leds+n);
  }

  if(showleds)
  {
    //ledset_1.paint_wave(current_time,start_millis,0,wave_speed,wave_width,CRGB::Purple);
    //ledset_2.paint_wave(current_time,start_millis,0,wave_speed,wave_width,CRGB::Purple);
    //ledset.paint_wave(current_time,start_millis,wave_start_mid,wave_speed,wave_width,CRGB::Yellow);
  }

  FastLED.show();
  delay(100);
}
