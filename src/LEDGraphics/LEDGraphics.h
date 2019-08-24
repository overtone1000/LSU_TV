#ifndef LEDGraphics_h
#define LEDGraphics_h

#include <FastLED.h>
#include <cmath>

#define MAX_BYTE 255

class LEDSet2D
{

public:
  LEDSet2D(CRGB* led_array, unsigned int total_LEDS, unsigned int first_LED, unsigned int last_LED, bool reversed);
  void paint_wave(unsigned long current_millis, unsigned long start_millis, float wave_start, float wave_speed, float wave_width, CRGB color);

private:
  std::vector<CRGB*> leds;
};

#endif
