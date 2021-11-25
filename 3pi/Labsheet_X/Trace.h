#pragma once
#include "defines.h"
#if MAPPING_TRACE

// Another memory limitation. Need to know the 
#define TRACE_SIZE 250 // limit x 4 = bytes required

/*
 * If the trace were recoreded in chars rather than shorts, then the length of the trace could be doubled,
 * but it would need to be rounded to the nearest centimeter instead of the nearest millimeter
 * i.e. increase in sample frequency or line length at the expense of resolution
 */
class Trace
{
public:
  Trace() = default;

  void addPoint(int x, int y);
  void dump();
private:
  unsigned int count = 0;
  short points_x[TRACE_SIZE];
  short points_y[TRACE_SIZE];
};

#endif
