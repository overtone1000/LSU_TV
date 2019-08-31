#ifndef LEDGraphics_h
#define LEDGraphics_h

#include <FastLED.h>
#include <cmath>

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
    void paint_wave(unsigned long current_millis, unsigned long start_millis, float wave_start, float wave_speed, float wave_width, MagnitudeBrush* brush);
  private:
    std::vector<CRGB*> leds;
  };
}

#endif
