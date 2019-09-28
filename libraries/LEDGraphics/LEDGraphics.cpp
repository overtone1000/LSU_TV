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
      int res = int((*pixel)[n]) + (int)color[n]*magnitude;
      if(res>255)
      {
        (*pixel)[n]=255;
      }
      else
      {
        (*pixel)[n]=res;
      }
      
    }
  }

  LEDSet2D::LEDSet2D(CRGB* led_array, unsigned int total_LEDS, unsigned int first_LED, unsigned int last_LED, bool reversed)
  {
    this->~LEDSet2D();

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

    debug_name=nullptr;
    SetDebugName("Unnamed LEDSet2D 1");
  }

  LEDSet2D::LEDSet2D(CRGB** led_array, unsigned int total_LEDS)
  {
    this->~LEDSet2D();
    this->led_count = total_LEDS;
    this->leds = new CRGB*[this->led_count];
    memcpy(this->leds,led_array,this->led_count*sizeof(CRGB*));

    debug_name=nullptr;
    SetDebugName("Unnamed LEDSet2D 2");
  }

  LEDSet2D::~LEDSet2D()
  {
    if(this->leds)
    {
      delete[] leds;
    }
    if(this->debug_name)
    {
      delete[] debug_name;
    }
  }

  void LEDSet2D::SetDebugName(String name)
  {
    //Serial.println("Changing LEDSet2D name to " + name);
    if(this->debug_name!=nullptr)
    {
      //Serial.println("Deleting current name.");
      delete[] this->debug_name;
      //Serial.println("Deleted.");
    }
    //Serial.println("Create new array.");
    this->debug_name = new char[name.length()+1];
    //Serial.println("Copying array.");
    strcpy(this->debug_name,name.c_str());
  }
  String LEDSet2D::GetDebugName(){return (String)this->debug_name;}
  CRGB** LEDSet2D::ledArray(){return this->leds;}
  const int LEDSet2D::ledCount(){return this->led_count;}

  //void LEDSet2D::paint_wave(unsigned long current_millis, unsigned long start_millis, float wave_start, float wave_speed, float wave_width, MagnitudeBrush* brush)
  void Hill::Paint(unsigned long current_millis, LEDSet2D* led_set, MagnitudeBrush* brush)
  {
    //wave_speed is LEDs per second
    //wave_width is LEDs
    CheckReset(current_millis);

    float last_x = speed*((float)(current_millis-start_millis))/1000.0F;
    float first_x = last_x-width;

    int last_LED = floor(last_x);
    int first_LED = ceil(first_x);

    if(last_LED>=led_set->ledCount()){last_LED=led_set->ledCount()-1;}
    if(first_LED<0){first_LED=0;}

    for(int n=first_LED;n<=last_LED;n++)
    {
      float x = ((float)n-first_x);
      float x_percent = x/width;
      uint8_t xbyte = round(x_percent*MAX_BYTE);
      uint8_t dim_factor = cos8(xbyte);

      brush->SetMagnitude((float)(MAX_BYTE-dim_factor)/(float)(MAX_BYTE));
      brush->paint(led_set->ledArray()[n]);
      //*(leds[n]) = color;
      //leds[n]->fadeLightBy(dim_factor);
      //leds[n] = color; //This works perfectly, so the problem is with dimming.

      //Serial.print((String)dim_factor + ", ");
    }
    //Serial.println();
  }

  Hill::Hill(unsigned long start_millis, float speed, float width, float LED_count)
  {
    this->start_millis=start_millis;
    this->speed=speed;
    this->width=width;
    this->LED_count = LED_count;
  }

  void Hill::CheckReset(unsigned long current_time)
  {
    float passed = ((float)(current_time-start_millis))*speed/1000.0F;
    float total = LED_count+width;
    if(passed>total)
    {
      //Serial.println((String)passed + "/" + (String)total);
      //Serial.println("Restarting wave.");
      start_millis=current_time;
    }
  }

  Wave::Wave(float frequency, float wavelength, float magnitude)
  {
    this->magnitude=magnitude;
    this->wavelength=wavelength;
    this->wave_millis=round(1000.0f/frequency);
  }
  void Wave::Paint(unsigned long current_millis, LEDSet2D* led_set, MagnitudeBrush* brush)
  {
    uint8_t init_angle = (float)(current_millis%wave_millis)/(float)wave_millis*MAX_BYTE;
    
    //Serial.println("Starting paint with LEDSet " + led_set->GetDebugName() + ". Set contains " + led_set->ledCount() + " LEDs.");
    //Serial.println("Initial angle = " + (String)init_angle);

    const unsigned int count = led_set->ledCount()-1;
    unsigned int n;
    for(n=0;n<count;n++)
    {
      float along_wave = (float)n/wavelength;
      //Serial.println("Along wave is " + (String)along_wave);
      uint8_t add_angle = MAX_BYTE*along_wave;
      uint8_t total_angle = init_angle+add_angle;
      uint8_t brightness = cos8(total_angle);
      float final_magnitude = ((float)brightness/(float)MAX_BYTE)*this->magnitude;
      //Serial.println("Final magnitude is " + (String)final_magnitude);
      brush->SetMagnitude(final_magnitude);
      brush->paint(led_set->ledArray()[n]);
    }
  }
}
