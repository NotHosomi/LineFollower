#include "LineSensor.h"


// static members
const int LineSensors::ls_pins[] = {LS_PIN_LL, LS_PIN_L, LS_PIN_C, LS_PIN_R, LS_PIN_RR};
const double LineSensors::gs_scale[] = {SCALE_LL, SCALE_L, SCALE_C, SCALE_R, SCALE_RR};
const double LineSensors::gs_min[] = {MIN_LL, MIN_L, MIN_C, MIN_R, MIN_RR};

static void LineSensors::refresh(double* gsv)
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
    gsv[i] = (int)(t0 - mark[i]); // casting straight from unsigned long to double causes overflow, so had to cast to int first
    gsv[i] = min(gsv[i], gs_min[i]);
    gsv[i] -= gs_min[i];
    gsv[i] /= gs_scale[i];
  }
  #if DEBUG_GROUND
  for(int i = 0; i < LS_COUNT; ++i)
  {
    Serial.print(gsv[i]);
    Serial.print(" ");
    //Serial.print(-gs_scale[i]);
    //Serial.print(" ");
  }
  Serial.println("\nLL L C R RR");
  //Serial.println("\nLL LL L L C C R R RR RR");
  #endif
}
