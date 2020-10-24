#include <LEDGraphics.h>

namespace LEDGraphics
{
  Brush::Brush(CRGB color){this->changeColor(color);}
  void Brush::changeColor(CRGB color){this->color=color;}
  void Brush::randomizeColor()
  {
    this->color.r=rand()%256;
    this->color.g=rand()%256;
    this->color.b=rand()%256;

    //Serial.println("Color is " + (String)this->color.r
    //+","+(String)this->color.g
    //+","+(String)this->color.b);
  }

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
    bool cont=true;
    while(cont)
    {
      cont=(next!=stop_led);
      
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
  void Hill::Paint(LEDSet2D* led_set, MagnitudeBrush* brush)
  {
    float along = this->CurrentAlong();
    float last_x = this->start_led*(1.0-along) + this->stop_led*along;
    //float last_x = speed*((float)(current_millis-start_millis))/1000.0F;
    float first_x = last_x-width;

    int last_LED = floor(last_x);
    int first_LED = ceil(first_x);

    //Serial.println("LEDs are " + (String) first_LED + "," + (String) last_LED);
    

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
    }
  }

  Hill::Hill(float frequency, float width, float start_led, float stop_led):PeriodicEffect(frequency,0)
  {
    this->width=width;
    this->start_led=start_led;
    this->stop_led=stop_led;
  }

  PeriodicEffect::PeriodicEffect(float frequency, float along_init)
  {
    this->period_millis=abs(round(1000.0f/frequency));
    this->along_init=along_init*MAX_BYTE;
    this->finished=false;
    this->last_along=along_init*MAX_BYTE;
  }

  boolean PeriodicEffect::Finished()
  {
    return this->finished;
  }

  void PeriodicEffect::UpdateAlong(unsigned long current_millis)
  {
    this->last_along=this->this_along;
    this->this_along=((float)(current_millis%this->period_millis)/(float)this->period_millis);
    this->finished=this->this_along < this->last_along;
  }

  float PeriodicEffect::CurrentAlong()
  {
    float along = this->this_along+this->along_init;
    float retval = along-floor(along);
    //Serial.println("Current along is " + (String)(retval));
    return retval;
  }

  uint8_t PeriodicEffect::CurrentAngle()
  {
    uint8_t retval = round(this->CurrentAlong()*((float)MAX_BYTE));
    //Serial.println("Current angle is " + retval);
    return retval;

  }

  Wave::Wave(float frequency, float wavelength, float magnitude):PeriodicEffect(frequency,0.0)
  {
    this->magnitude=magnitude;
    this->wavelength=wavelength; 
  }

  void Wave::Paint(LEDSet2D* led_set, MagnitudeBrush* brush)
  {
    //Serial.println("Starting paint with LEDSet " + led_set->GetDebugName() + ". Set contains " + led_set->ledCount() + " LEDs.");
    //Serial.println("Initial angle = " + (String)init_angle);

    const unsigned int count = led_set->ledCount()-1;
    unsigned int n;
    for(n=0;n<count;n++)
    {
      float along_wave = (float)n/wavelength;
      //Serial.println("Along wave is " + (String)along_wave);
      uint8_t add_angle = MAX_BYTE*along_wave;
      uint8_t total_angle = this->CurrentAngle()+add_angle;
      uint8_t brightness = cos8(total_angle);
      float final_magnitude = ((float)brightness/(float)MAX_BYTE)*this->magnitude;
      //Serial.println("Final magnitude is " + (String)final_magnitude);
      brush->SetMagnitude(final_magnitude);
      brush->paint(led_set->ledArray()[n]);
    }
  }

  Glow::Glow(float frequency, float angle_init, float highmag, float lowmag):PeriodicEffect(frequency,angle_init)
  {
    this->highmag=highmag;
    this->lowmag=lowmag;
  }

  void Glow::Paint(LEDSet2D* led_set, MagnitudeBrush* brush)
  {
    uint8_t brightness = cos8(this->CurrentAngle());
    float fraction = ((float)brightness/(float)MAX_BYTE);
    float final_magnitude = this->lowmag*(1.0-fraction)+this->highmag*(fraction);
    //Serial.println("Final magnitude is " + (String)final_magnitude);
    brush->SetMagnitude(final_magnitude);

    const unsigned int count = led_set->ledCount()-1;
    unsigned int n;
    for(n=0;n<count;n++)
    {
      brush->paint(led_set->ledArray()[n]);
    }
  }
}
