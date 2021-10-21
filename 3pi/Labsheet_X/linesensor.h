#pragma once
#include "defines.h"
#include <Arduino.h>

// Class to operate the linesensor(s).
class LineSensors
{
public:
  static void init()
  {
    pinMode(LS_PIN_LL, INPUT);
    pinMode(LS_PIN_L,  INPUT);
    pinMode(LS_PIN_C,  INPUT);
    pinMode(LS_PIN_R,  INPUT);
    pinMode(LS_PIN_RR, INPUT);
    pinMode(LS_PIN_EMIT, OUTPUT);
    
    digitalWrite(LS_PIN_EMIT, HIGH); // it is supposed to be HIGH, right?
  }
  static void refresh(int* gsv)
  {
    gsv[0] = analogRead(LS_PIN_LL);
    gsv[1] = analogRead(LS_PIN_L);
    gsv[2] = analogRead(LS_PIN_C);
    gsv[3] = analogRead(LS_PIN_R);
    gsv[4] = analogRead(LS_PIN_RR);
#if SERIAL
    char buffer[50] = "";
    sprintf(buffer, " LL: %d  L: %d  C: %d  R: %d  R: %d\n", gsv[0], gsv[1], gsv[2], gsv[3], gsv[4]);
    Serial.write(buffer);
#endif
  }
};
