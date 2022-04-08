#include <ESP8266WiFi.h>
#include "WiFiCredentials.h"

#include <ESP8266WebServer.h>
#define HTTP_REST_PORT 80
ESP8266WebServer httpRestServer(HTTP_REST_PORT);

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
LEDGraphics::LEDSet2D ledset_1(leds,NUM_LEDS,0,NUM_LEDS-1,false);
LEDGraphics::LEDSet2D ledset_2(leds,NUM_LEDS,NUM_LEDS-1,0,true);

LEDGraphics::LEDSet2D ledset_m1(leds,NUM_LEDS,NUM_LEDS-1,NUM_LEDS*2/3,true);
LEDGraphics::LEDSet2D ledset_m2(leds,NUM_LEDS,NUM_LEDS*2/3-1,NUM_LEDS/3,true);
LEDGraphics::LEDSet2D ledset_m3(leds,NUM_LEDS,NUM_LEDS/3-1,0,true);

const char* dev_gotigers = "Go Tigers";
const char* dev_merica = "Merica";
const char* dev_sheramessage = "SR Message";

enum Mode
{
  tigers=1,
  merica=2,
  sr_message=3,
  goblet_simmer=4
};

Mode mode = goblet_simmer;
bool showleds = false;

unsigned long goblet_blaze_start=0;

void restServerRouting() {
  httpRestServer.on("/goblet_simmer", HTTP_GET, []() {
    showleds=true;
    mode=goblet_simmer;
    httpRestServer.send(200);
  });
  httpRestServer.on("/goblet_blaze", HTTP_GET, []() {
    Serial.println("Goblet blaze");
    goblet_blaze_start=millis();
    httpRestServer.send(200);
  });
  httpRestServer.on("/tigers", HTTP_GET, []() {
    showleds=true;
    mode=tigers;
    httpRestServer.send(200);
  });
  httpRestServer.on("/america", HTTP_GET, []() {
    showleds=true;
    mode=merica;
    httpRestServer.send(200);
  });
  httpRestServer.on("/shera", HTTP_GET, []() {
    showleds=true;
    mode=sr_message;
    httpRestServer.send(200);
  });
  httpRestServer.on("/off", HTTP_GET, []() {
    showleds=false;
    httpRestServer.send(200);
  });
}

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

  Serial.println("Configuring restful endpoint");
  
  restServerRouting();
  httpRestServer.begin();

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


const CRGB fire1=CRGB::DarkOrange; //CRGB(209,76,50); //light yellow
const CRGB fire2=CRGB::DarkRed; //burnt orange
const CRGB fire3=CRGB::OrangeRed; //(211,101,16); //dark orange

const CRGB blaze1=CRGB(114,254,46); //purple
const CRGB blaze2=CRGB(16,26,211); //blue
const CRGB blaze3=CRGB(16,208,211); //cyan


const unsigned long goblet_blaze_fade_in=1000;
const unsigned long goblet_blaze_stay=1000;
const unsigned long goblet_blaze_fade_out=3000;

#define FIREFLICKERTIMEARRAYSIZE 3
const unsigned long fire_flicker_t[FIREFLICKERTIMEARRAYSIZE] = {25,100,250};

void loop() {
  httpRestServer.handleClient();

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
        w1.UpdateAlong(current_time);
        w2.UpdateAlong(current_time);
        w1.Paint(&ledset_1,&yellow); 
        w2.Paint(&ledset_1,&purple); 
        //w1.CheckReset(current_time);
        //w2.CheckReset(current_time);
        //ledset_2.paint_wave(current_time,w1.start_millis,0,w1.speed,w1.width,&purple);
        //ledset_1.paint_wave(current_time,w2.start_millis,0,w2.speed,w2.width,&yellow);
        break;
      case Mode::merica:
        m1.UpdateAlong(current_time);
        m2.UpdateAlong(current_time);
        m3.UpdateAlong(current_time);
        m1.Paint(&ledset_m1,&red); 
        m2.Paint(&ledset_m2,&white); 
        m3.Paint(&ledset_m3,&blue); 
        break;
      case Mode::sr_message:
        m1.UpdateAlong(current_time);
        m2.UpdateAlong(current_time);
        m3.UpdateAlong(current_time);
        m1.Paint(&ledset_m1,&blue); 
        m2.Paint(&ledset_m2,&blue); 
        m3.Paint(&ledset_m3,&blue);
        break;
      case Mode::goblet_simmer:
        
        LEDGraphics::BlendBrush final_fire1(fire1,1.0);
        LEDGraphics::BlendBrush final_fire2(fire2,1.0);
        LEDGraphics::BlendBrush final_fire3(fire3,1.0);
        float blaze_alpha = 0;
        unsigned long blaze_time=current_time-goblet_blaze_start;
        if(blaze_time<=goblet_blaze_fade_in)
        {
          blaze_alpha=(float)blaze_time/(float)goblet_blaze_fade_in;
        }
        else if(blaze_time<=goblet_blaze_fade_in+goblet_blaze_stay)
        {
          blaze_alpha=1;
        }
        else if(blaze_time<=goblet_blaze_fade_in+goblet_blaze_stay+goblet_blaze_fade_out)
        {
          blaze_alpha=1.0f-(float)(blaze_time-goblet_blaze_fade_in-goblet_blaze_stay)/(float)goblet_blaze_fade_out;
        }
        final_fire1.blendOver(blaze1,blaze_alpha);
        final_fire2.blendOver(blaze2,blaze_alpha);
        final_fire3.blendOver(blaze3,blaze_alpha);

        //Flickering fire effect
        float fire_flicker_magnitude=1.0f-(blaze_alpha/2.0f); //Don't flicker as much during blaze
        for(int n=0;n<FIREFLICKERTIMEARRAYSIZE;n++)
        {
          fire_flicker_magnitude*=sin(current_time/fire_flicker_t[n])/2.0f+1.0f;
        }
        fire_flicker_magnitude=0.6f*(1.0-fire_flicker_magnitude);
        final_fire1.fadeLightBy(fire_flicker_magnitude);
        final_fire2.fadeLightBy(fire_flicker_magnitude);
        final_fire3.fadeLightBy(fire_flicker_magnitude);

        /*
        Serial.print("Blaze alpha=" + (String)blaze_alpha);
        Serial.print(" ");
        Serial.print("Blaze start="+ (String)goblet_blaze_start);
        Serial.print(" ");
        Serial.print("Blaze time="+ (String)blaze_time);
        Serial.println();
        */

        m1.UpdateAlong(current_time);
        m2.UpdateAlong(current_time);
        m3.UpdateAlong(current_time);
        m1.Paint(&ledset_m1,&final_fire1); 
        m2.Paint(&ledset_m2,&final_fire2); 
        m3.Paint(&ledset_m3,&final_fire3);
        break;
    }
    //ledset.paint_wave(current_time,start_millis,wave_start_mid,wave_speed,wave_width,CRGB::Yellow);
  }

  FastLED.show();
  //delay(100); //This delay was screwing up timing!

  //Serial.println("Looped. Millis:" + (String)current_time);
}
