#include <ESP8266WiFi.h>
#include <fauxmoESP.h>
#include <WiFiCredentials.h>

fauxmoESP fauxmo;

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "LEDGraphics.h"

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
LEDGraphics::LEDSet2D ledset_1(leds,NUM_LEDS,0,29,false);
LEDGraphics::LEDSet2D ledset_2(leds,NUM_LEDS,29,0,true);

LEDGraphics::LEDSet2D ledset_m1(leds,NUM_LEDS,29,20,true);
LEDGraphics::LEDSet2D ledset_m2(leds,NUM_LEDS,19,10,true);
LEDGraphics::LEDSet2D ledset_m3(leds,NUM_LEDS,9,0,true);

const char* dev_gotigers = "Go Tigers";
const char* dev_merica = "Merica";
const char* dev_sheramessage = "SR Message";

enum Mode
{
  tigers=1,
  merica=2,
  sr_message=3
};

Mode mode = tigers;
bool showleds = false;

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(74880); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  Serial.println("Test LED build.");
  LEDGraphics::LEDSet2D testset(leds,NUM_LEDS,0,15,false);
  LEDGraphics::LEDSet2D testset2(leds,NUM_LEDS,15,0,true);

  Serial.println("Starting WiFi for " + (String)SSID);
  WiFi.begin(SSID, WFPASS);

  //pinMode(DATA_PIN, OUTPUT);
  Serial.println("Adding LEDs");
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(60);

  Serial.println("Configuring Fauxmo");
  //fauxmo.addDevice(dev_gotigers);
  //fauxmo.addDevice(dev_merica);
  fauxmo.addDevice(dev_sheramessage);

  fauxmo.setPort(80); // required for gen3 devices
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
      String thisdev(device_name);
      showleds=state;
      if(strcmp(device_name,dev_gotigers)==0)
      {
        mode=Mode::tigers;
      }
      else if(strcmp(device_name,dev_merica)==0)
      {
        mode=Mode::merica;
      }
      else if(strcmp(device_name,dev_sheramessage)==0)
      {
        mode=Mode::sr_message;
      }
      Serial.println("Device " + (String)device_name + " is " + (String)state);
  });

    // Wait for connection
  Serial.println("Waiting for WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected. IP is " + WiFi.localIP().toString());
}

LEDGraphics::Wave w1(0.25,NUM_LEDS,1.0);
LEDGraphics::Wave w2(0.25,NUM_LEDS/5,1.0);

LEDGraphics::Wave m1(0.5,NUM_LEDS/3,1.0);
LEDGraphics::Wave m2(0.5,NUM_LEDS/3,1.0);
LEDGraphics::Wave m3(0.5,NUM_LEDS/3,1.0);

LEDGraphics::BlendBrush purple(CRGB::Purple, 0.5f);
LEDGraphics::BlendBrush yellow(CRGB::Yellow, 0.5f);

LEDGraphics::BlendBrush red(CRGB::Red,1.0f);
LEDGraphics::BlendBrush white(CRGB::White,1.0f);
LEDGraphics::BlendBrush blue(CRGB::Blue,1.0f);

void loop() {
  fauxmo.handle();

  unsigned long current_time = millis();

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds[n] = CRGB::Black;
  }

  if(showleds)
  {
    switch(mode)
    {
      case Mode::tigers:
        Serial.println("Showing tigers...");
<<<<<<< HEAD
        w1.UpdateAlong(current_time);
        w2.UpdateAlong(current_time);
        w1.Paint(&ledset_1,&yellow); 
        w2.Paint(&ledset_1,&purple); 
=======
        //w1.Paint(current_time,&ledset_1,&yellow); 
        //w2.Paint(current_time,&ledset_1,&purple); 
>>>>>>> 79adcc572616458eeb1eb266a8e748770fa916c4
        //w1.CheckReset(current_time);
        //w2.CheckReset(current_time);
        //ledset_2.paint_wave(current_time,w1.start_millis,0,w1.speed,w1.width,&purple);
        //ledset_1.paint_wave(current_time,w2.start_millis,0,w2.speed,w2.width,&yellow);
        break;
      case Mode::merica:
<<<<<<< HEAD
        m1.UpdateAlong(current_time);
        m2.UpdateAlong(current_time);
        m3.UpdateAlong(current_time);
        m1.Paint(&ledset_m1,&red); 
        m2.Paint(&ledset_m2,&white); 
        m3.Paint(&ledset_m3,&blue); 
=======
        //m1.Paint(current_time,&ledset_m1,&red); 
        //m2.Paint(current_time,&ledset_m2,&white); 
        //m3.Paint(current_time,&ledset_m3,&blue); 
        //m1.CheckReset(current_time);
        //m2.CheckReset(current_time);
        //m3.CheckReset(current_time);
        //ledset_m1.paint_wave(current_time,m1.start_millis,0,m1.speed,m1.width,&red);
        //ledset_m2.paint_wave(current_time,m2.start_millis,0,m2.speed,m2.width,&white);
        //ledset_m3.paint_wave(current_time,m3.start_millis,0,m3.speed,m3.width,&blue);
>>>>>>> 79adcc572616458eeb1eb266a8e748770fa916c4
        break;
      case Mode::sr_message:
        w1.UpdateAlong(current_time);
        w2.UpdateAlong(current_time);
        w1.Paint(&ledset_1,&blue); 
        w2.Paint(&ledset_1,&blue);
    }


    //ledset.paint_wave(current_time,start_millis,wave_start_mid,wave_speed,wave_width,CRGB::Yellow);
  }

  FastLED.show();
  delay(100);
}
