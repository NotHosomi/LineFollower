#pragma once
#include "defines.h"
#include <Arduino.h>

// Class to operate the linesensors.
class LineSensors
{
public:
  static void init()
  {    
    for(int i = 0; i < LS_COUNT; ++i)
    pinMode(ls_pins[i], INPUT);
    pinMode(LS_PIN_EMIT, OUTPUT);
    
    digitalWrite(LS_PIN_EMIT, HIGH);
  }
  static void refresh(double* gsv);
//private:
  static const int ls_pins[];
  static const double gs_scale[];
  static const double gs_min[];
};
