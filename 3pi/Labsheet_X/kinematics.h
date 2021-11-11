#pragma once
#include "encoders.h"
#include <Arduino.h>

#define WHEEL_RAD 16 //mm
#define AXEL_LEN 86  //mm
#define WHEEL_CIRC (2 * WHEEL_RAD * PI) // mm

// 3pi+ info
// gear ratio: 29.86:1
// encoder counts per shaft rotation: 12
#define CPR 358.32
#define COUNT2MM (WHEEL_CIRC/CPR);
#define RAD2DEG (180.0/PI)
#define THETA_FIX 2 * PI / 0.56 // 1 full rotation gives 0.56 as the theta angle, so this should correct that, hopefully

// sources of error
// encoder count per rot
// wheel radius measure 
// axel length measure


// Class to track robot position.
class Kinematics_c {
public:

  // Constructor, must exist.
  Kinematics_c() 
  {
    setupEncoder0();
    setupEncoder1();
  } 

  // Use this function to update
  // your kinematics
  double netL = 0;
  double netR = 0;
  void update()
  {
    #if DEBUG_ENCODE
    char buffer[50];
    sprintf(buffer, "%d %d L R", count_eL, count_eR);
    Serial.println(buffer);
    return;
    #endif
    double L = (double)count_eL;
    double R = (double)count_eR;
    netL += L * COUNT2MM;
    netR += R * COUNT2MM;
    count_eL = 0;
    count_eR = 0;

    L *= COUNT2MM;
    R *= COUNT2MM;
    
    double dist = (L / 2) + (R / 2);
    x += dist * cos(rot);
    y += dist * sin(rot);
  
    rot += ((L / AXEL_LEN) - (R / AXEL_LEN));

    #if DEBUG_ODO
    //char buffer[50];
    //sprintf(buffer, "X: %8.4f\tY: %8.4f", x, y);
    //Serial.println(buffer);
    Serial.print(x);
    Serial.print(" X ");
    Serial.print(y);
    Serial.print(" Y ");
    Serial.print(rot);
    Serial.print(" Th ");
    Serial.print(netL);
    Serial.print(" L ");
    Serial.print(netR);
    Serial.println(" R ");
    #endif
  }

  //static int getCountL()
  //{
  //  return count_eL;
  //}

  
  float x = 0;
  float y = 0;
  float rot = 0;    
};
