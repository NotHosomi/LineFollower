#pragma once
#include "defines.h"



// Class to operate the linesensor(s).
class LineSensor_c
{
public:
  LineSensor_c()
  {
    pinMode(LS_PIN_LL, INPUT);
    pinMode(LS_PIN_L,  INPUT);
    pinMode(LS_PIN_C,  INPUT);
    pinMode(LS_PIN_R,  INPUT);
    pinMode(LS_PIN_RR, INPUT);
    pinMode(LS_PIN_EMIT, OUTPUT);
    
    digitalWrite(LS_PIN_EMIT, HIGH) // it is supposed to be HIGH, right?
  } 
  void refresh(int* gsv)
  {
    gsv[0] 
  }
};
