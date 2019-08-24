#include "LEDGraphics.h"

LEDSet2D::LEDSet2D(CRGB* start_led, CRGB* stop_led)
{
  short delta=1;
  unsigned short count=abs(stop_led-start_led);
  this->leds.resize(count);
  if(start_led>stop_led)
  {
      delta=-1;
  }
  for(int n=0;n<count;n++)
  {
    this->leds[n]=start_led+n;
  }
}

void LEDSet2D::paint_wave(unsigned long current_millis, unsigned long start_millis, float wave_start, float wave_speed, float wave_width, CRGB color)
{
  //wave_speed is LEDs per second
  //wave_width is LEDs
  float last_x = wave_speed*((float)(current_millis-start_millis))/1000.0F;
  float first_x = last_x-wave_width;

  int last_LED = floor(last_x);
  int first_LED = ceil(first_x);

  Serial.println((String)first_x + "|" + (String)last_x);
  Serial.println((String)first_LED + "|" + (String)last_LED);

  if(last_LED>=leds.size()){last_LED=leds.size()-1;}
  if(first_LED<0){first_LED=0;}

  for(int n=first_LED;n<=last_LED;n++)
  {
    float x = ((float)n-first_x);
    float x_percent = x/wave_width;
    uint8_t xbyte = round(x_percent*MAX_BYTE);
    uint8_t dim_factor = cos8(xbyte);

    *(leds[n]) = color;
    leds[n]->fadeLightBy(dim_factor);
    //leds[n] = color; //This works perfectly, so the problem is with dimming.

    Serial.print((String)dim_factor + ", ");
  }
  Serial.println();
}
