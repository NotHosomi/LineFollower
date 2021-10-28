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
    
    digitalWrite(LS_PIN_EMIT, HIGH); // it is supposed to be HIGH, right?
  }
  static void refresh(int* gsv);
//private:
  static const int ls_pins[];
};
