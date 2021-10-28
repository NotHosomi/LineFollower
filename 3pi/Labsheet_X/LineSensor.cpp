#include "LineSensor.h"


// static members
const int LineSensors::ls_pins[] = {LS_PIN_LL, LS_PIN_L, LS_PIN_C, LS_PIN_R, LS_PIN_RR};


static void LineSensors::refresh(int* gsv)
{
  char buffer[50] = "";
  unsigned long t0;
  unsigned long mark[5] = {0, 0, 0, 0, 0}; // mark 1
  
  for(int i = 0; i < LS_COUNT; ++i) // Note: declaring int i in the for loop call uses registry, rather than frame lookup
  {
    pinMode( ls_pins[i], OUTPUT );
    digitalWrite( ls_pins[i], HIGH );
    delayMicroseconds(10);
    pinMode( ls_pins[i], INPUT );
  }
  t0 = micros();

  char count = 0;
  do
  {
    for(int i = 0; i < LS_COUNT; ++i)
    {
      if(digitalRead(ls_pins[i]) == LOW && mark[i] == 0)
      {
        mark[i] = micros();
        ++count;
      }
    }
  } while(count < LS_COUNT);

  for(int i = 0; i < LS_COUNT; ++i)
  {
    gsv[i] = (int)(t0 - mark[i]);
  }
}
