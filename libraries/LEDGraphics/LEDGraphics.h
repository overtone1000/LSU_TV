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
    Brush(CRGB color);
    void changeColor(CRGB color);
    void fadeLightBy(float factor);
    void randomizeColor();
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
    void blendOver(CRGB color, float alpha){
      this->color.r=this->color.r*(1.0-alpha)+color.r*alpha;
      this->color.g=this->color.g*(1.0-alpha)+color.g*alpha;
      this->color.b=this->color.b*(1.0-alpha)+color.b*alpha;
    }
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
    //void paint_Hill(unsigned long current_millis, unsigned long start_millis, float Hill_start, float Hill_speed, float Hill_width, MagnitudeBrush* brush);
    CRGB** ledArray();
    const int ledCount();
    void SetDebugName(String name);
    String GetDebugName();
  private:
    CRGB** leds=nullptr;
    unsigned int led_count=0;
    char* debug_name=nullptr;
  };

  class PeriodicEffect
  {
    private:
    unsigned long period_millis;
    float along_init;
    float last_along;
    float this_along;

    boolean finished;

    public:
    PeriodicEffect(float frequency, float along_init);
    boolean Finished();
    void UpdateAlong(unsigned long current_millis);
    float CurrentAlong();
    uint8_t CurrentAngle();
  };

  class Hill:public PeriodicEffect
  {
    private:
    unsigned long start_millis;
    float width;
    float start_led;
    float stop_led;

    public:
    Hill(float frequency, float width, float start_led, float stop_led);
    void Paint(LEDSet2D* led_set, MagnitudeBrush* brush);
  };  

  class Wave:public PeriodicEffect
  {
    private:
    unsigned long wave_millis;
    float magnitude;
    float wavelength;
    public:
    Wave(float frequency, float wavelength, float magnitude);
    void Paint(LEDSet2D* led_set, MagnitudeBrush* brush);
  };

  class Glow:public PeriodicEffect
  {
    private:
    float highmag;
    float lowmag;
    public:
    Glow(float frequency, float angle_init, float highmag, float lowmag);
    void Paint(LEDSet2D* led_set, MagnitudeBrush* brush);
  };
}

#endif
