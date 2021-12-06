#pragma once
#include "encoders.h"
#include <Arduino.h>
#include "defines.h"
#if MAPPING_GRID
  #include "Grid.h"
#elif MAPPING_TRACE
  #include "Trace.h"
#endif

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
  //double netL = 0;
  //double netR = 0;
  #if MAPPING_GRID
  void update(Grid* grid, double* gsv)
  #elif MAPPING_TRACE
  void update(Trace* trace, double* gsv)
  #else
  void update()
  #endif
  {
    #if DEBUG_ENCODE
    char buffer[50];
    sprintf(buffer, "%d %d L R", count_eL, count_eR);
    Serial.println(buffer);
    return;
    #endif
    double L = (double)count_eL;
    double R = (double)count_eR;
    //netL += L * COUNT2MM;
    //netR += R * COUNT2MM;
    count_eL = 0;
    count_eR = 0;

    L *= COUNT2MM;
    R *= COUNT2MM;
    
    double ICR_offset = 0;
    double dist = 0;
    double phi = (L - R) / AXEL_LEN;
    if(L!=R)
    {
      ICR_offset = AXEL_LEN/2 * (L+R)/(L-R);
      dist = ICR_offset * phi; // could improve accuracy further by taking the chord of this arc
    }
    else
    {
      dist = (L / 2) + (R / 2);
    }
    x += dist * cos(rot);
    y += dist * sin(rot);
    rot += phi;

    #if DEBUG_ODO
      Serial.print(x);
      Serial.print(" X ");
      Serial.print(y);
      Serial.print(" Y ");
      Serial.print(rot);
      Serial.print(" Th ");
      //Serial.print(netL);
      //Serial.print(" L ");
      //Serial.print(netR);
      //Serial.println(" R ");
    #endif

    // GRID MAPPING
#if MAPPING_GRID
//    if(gsv[GSLL] < -0.5)
//      grid->setTile(x + LSO_OUTER_X*cos(rot), y - LSO_OUTER_Y*sin(rot));
//    if(gsv[GSL] < -0.5)
//      grid->setTile(x + LSO_INNER_X*cos(rot), y - LSO_INNER_Y*sin(rot));
//    if(gsv[GSC] < -0.5)
//      grid->setTile(x + LSO_CENTER_X*cos(rot), y);
//    if(gsv[GSR] < -0.5)
//      grid->setTile(x + LSO_INNER_X*cos(rot), y + LSO_INNER_Y*sin(rot));
//    if(gsv[GSRR] < -0.5)
//      grid->setTile(x + LSO_OUTER_X*cos(rot), y + LSO_OUTER_Y*sin(rot));
    
    grid->setTile(x, y); // track bot position, rather than line position

#elif MAPPING_TRACE
//    float avg_x = 0;
//    float avg_y = 0;
//  
//    int hits = 0;
//    if(gsv[GSLL] < -0.5)
//    {
//      avg_x += LSO_OUTER_X*cos(rot);
//      avg_y += LSO_OUTER_Y*sin(rot);
//      ++hits;
//    }
//    if(gsv[GSL] < -0.5)
//    {
//      avg_x +=  LSO_INNER_X*cos(rot);
//      avg_y += LSO_INNER_Y*sin(rot);
//      ++hits;
//    }
//    if(gsv[GSC] < -0.5)
//    {
//      avg_x += LSO_OUTER_X*cos(rot);
//      ++hits;
//    }
//    if(gsv[GSR] < -0.5)
//    {
//      avg_x += LSO_INNER_X;
//      avg_y += LSO_INNER_Y*sin(rot);
//      ++hits;
//    }
//    if(gsv[GSRR] < -0.5)
//    {
//      avg_x += LSO_OUTER_X*cos(rot);
//      avg_y += LSO_OUTER_Y;
//      ++hits;
//    } 
//    avg_x /= hits;
//    avg_y /= hits;
//    avg_x += x;
//    avg_y += y;
    if(trace->timer())
    {
      trace->addPoint(round(x), round(y));
    }
#endif
  }
  
  
  float x = 0;
  float y = 0;
  float rot = 0;    
};
