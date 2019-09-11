#include <LEDGraphics.h>

namespace LEDGraphics
{
  void BlendBrush::paint(CRGB* pixel)
  {
    for(int n=0;n<3;n++)
    {
      (*pixel)[n]=color[n]*magnitude + (*pixel)[n]*(1.0f-magnitude);
    }
  }

  void AddBrush::paint(CRGB* pixel)
  {
    for(int n=0;n<3;n++)
    {
      (*pixel)[n]+=color[n]*magnitude;
    }
  }

  LEDSet2D::~LEDSet2D()
  {
    delete[] this->leds;
  }

  LEDSet2D::LEDSet2D(CRGB* led_array, unsigned int total_LEDS, unsigned int first_LED, unsigned int last_LED, bool reversed)
  {
    CRGB* start_led = led_array+first_LED;
    CRGB* stop_led = led_array+last_LED;
    CRGB* led_last = led_array+total_LEDS;
    CRGB* next=start_led;
    CRGB** temp_leds = new CRGB*[total_LEDS];

    this->led_count=0;
    while(next!=stop_led)
    {
      temp_leds[this->led_count]=next;
      this->led_count++;

      if(reversed)
      {
          next--;
          if(next<led_array)
          {
            next=led_last;
          }
      }
      else
      {
          next++;
          if(next>led_last)
          {
            next=led_array;
          }
      }
    }
    temp_leds[this->led_count]=(stop_led);
    this->led_count++;

    this->leds = new CRGB*[this->led_count];
    memcpy(this->leds,temp_leds,this->led_count*sizeof(CRGB*));
    delete[] temp_leds;
  }

  void LEDSet2D::paint_wave(unsigned long current_millis, unsigned long start_millis, float wave_start, float wave_speed, float wave_width, MagnitudeBrush* brush)
  {
    //wave_speed is LEDs per second
    //wave_width is LEDs

    float last_x = wave_speed*((float)(current_millis-start_millis))/1000.0F;
    float first_x = last_x-wave_width;

    int last_LED = floor(last_x);
    int first_LED = ceil(first_x);

    if(last_LED>=this->led_count){last_LED=this->led_count-1;}
    if(first_LED<0){first_LED=0;}

    for(int n=first_LED;n<=last_LED;n++)
    {
      float x = ((float)n-first_x);
      float x_percent = x/wave_width;
      uint8_t xbyte = round(x_percent*MAX_BYTE);
      uint8_t dim_factor = cos8(xbyte);

      brush->SetMagnitude((float)(MAX_BYTE-dim_factor)/(float)(MAX_BYTE));
      brush->paint(leds[n]);
      //*(leds[n]) = color;
      //leds[n]->fadeLightBy(dim_factor);
      //leds[n] = color; //This works perfectly, so the problem is with dimming.

      Serial.print((String)dim_factor + ", ");
    }
    Serial.println();
  }
}
