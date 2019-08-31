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
//LEDGraphics::LEDSet2D ledset_1(leds,NUM_LEDS,0,15,false);
//LEDGraphics::LEDSet2D ledset_2(leds,NUM_LEDS,30,16,true);
LEDGraphics::LEDSet2D ledset_1(leds,NUM_LEDS,0,30,false);
LEDGraphics::LEDSet2D ledset_2(leds,NUM_LEDS,30,0,true);

String devname = "Go Tigers";

bool showleds = true;

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(74880); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  WiFi.begin(SSID, WFPASS);

  //pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(60);

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

struct Wave
{
  unsigned long start_millis;
  float speed;
  float width;
  float LED_count;

  Wave(unsigned long start_millis, float speed, float width, float LED_count)
  {
    this->start_millis=start_millis;
    this->speed=speed;
    this->width=width;
    this->LED_count = LED_count;
  }

  void CheckReset(unsigned long current_time)
  {
    float passed = (current_time-start_millis)*speed/1000.0F;
    float total = LED_count+width;
    if(passed>total)
    {
      //Serial.println((String)passed + "/" + (String)total);
      //Serial.println("Restarting wave.");
      start_millis=current_time;
    }
  }
};

Wave w1(5000,1.5,NUM_LEDS/2,((float)NUM_LEDS));
Wave w2(5000,15,NUM_LEDS/4,((float)NUM_LEDS));

LEDGraphics::BlendBrush purple(CRGB::Purple, 1.0f);
LEDGraphics::BlendBrush yellow(CRGB::Yellow, 1.0f);

void loop() {
  fauxmo.handle();

  unsigned long current_time = millis();

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds[n] = CRGB::Black;
  }

  if(showleds)
  {
    w1.CheckReset(current_time);
    w2.CheckReset(current_time);
    ledset_2.paint_wave(current_time,w1.start_millis,0,w1.speed,w1.width,&purple);
    ledset_1.paint_wave(current_time,w2.start_millis,0,w2.speed,w2.width,&yellow);

    //ledset.paint_wave(current_time,start_millis,wave_start_mid,wave_speed,wave_width,CRGB::Yellow);
  }

  FastLED.show();
  delay(100);
}
