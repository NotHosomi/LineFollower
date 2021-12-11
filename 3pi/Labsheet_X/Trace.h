#pragma once
#include "defines.h"
#if MAPPING_TRACE
#include <Arduino.h>

// Another memory limitation. Need to know the 
#define BYTES 1024 // 1 KB of NVM
// Time based:
#define TRACE_TIME 500 // 0.5 seconds
// Event based:
#define THETA_THRESHOLD 40 * PI/180
#define MIN_DIST_SQR 100 // Only take a sample if the bot has travelled at least a centimeter

#if MAPPING_EVENT
  #define vec_t short
#else
  #define vec_t char
#endif  

/*
 * If the trace were recoreded in chars rather than shorts, then the length of the trace could be doubled,
 * but it would need to be rounded to the nearest centimeter instead of the nearest millimeter
 * i.e. increase in sample frequency or line length at the expense of resolution
 */
class Trace
{
public:
  Trace() = default;

#if MAPPING_EVENT
  void addPoint(int x, int y, double theta);
#else
  void addPoint(int x, int y);
#endif
  void dump();
  void save();
  void load();
  void debug();
  unsigned short count = 0;
#if !MAPPING_EVENT
  bool timer();
#endif
private:
  vec_t points_x[BYTES/(2*sizeof(vec_t)) + 2];
  vec_t points_y[BYTES/(2*sizeof(vec_t)) + 2];
  int last_x = 0;
  int last_y = 0;
  bool loaded = false;

#if MAPPING_EVENT
  double last_theta;
#else
  unsigned long t_mark = 0;
#endif
};

#endif
