#ifndef LEDGraphics_h
#define LEDGraphics_h

#include <FastLED.h>
//#include <cmath>
#include <math.h>

#define MAX_BYTE 255

namespace LEDGraphics
{
  class Brush
  {
  public:
    Brush(CRGB color){this->color=color;}
    virtual void paint(CRGB* pixel)=0;
  protected:
    CRGB color=CRGB::Black;
  };

  class MagnitudeBrush:public Brush
  {
  public:
    MagnitudeBrush(CRGB color, float magnitude):Brush(color){this->magnitude=magnitude;}
    void SetMagnitude(float magnitude){this->magnitude=magnitude;}
    virtual void paint(CRGB* pixel)=0;
  protected:
    float magnitude;
  };

  class BlendBrush:public MagnitudeBrush
  {
  public:
    BlendBrush(CRGB color, float alpha):MagnitudeBrush(color,alpha){}
    virtual void paint(CRGB* pixel);
  };

  class AddBrush:public MagnitudeBrush
  {
  public:
    AddBrush(CRGB color, float opacity):MagnitudeBrush(color,opacity){}
    virtual void paint(CRGB* pixel);
  };

  class LEDSet2D
  {
  public:
    LEDSet2D(CRGB* led_array, unsigned int total_LEDS, unsigned int first_LED, unsigned int last_LED, bool reversed);
    LEDSet2D(CRGB** led_array, unsigned int total_LEDS);
    ~LEDSet2D();
    //void paint_wave(unsigned long current_millis, unsigned long start_millis, float wave_start, float wave_speed, float wave_width, MagnitudeBrush* brush);
    CRGB** ledArray(){return leds;}
    const int ledCount(){return led_count;}
  private:
    CRGB** leds=nullptr;
    int led_count=0;
  };

  class Wave
  {
    private:
    unsigned long start_millis;
    float speed;
    float width;
    float LED_count;
    void CheckReset(unsigned long current_time);

    public:
    Wave(unsigned long start_millis, float speed, float width, float LED_count);
    void Paint(unsigned long current_millis, LEDSet2D* led_set, MagnitudeBrush* brush);
  };
}

#endif
