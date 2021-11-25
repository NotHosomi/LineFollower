#include "Trace.h"
#if MAPPING_TRACE

void Trace::addPoint(int x, int y)
{
  if(count == TRACE_SIZE)
    return;
  points_x[count] = x;
  points_y[count] = y;
}
void Trace::dump()
{
  Serial.write('T');
  Serial.write(count);
  for(int i = 0; i < count; ++i)
  {
    Serial.write(points_x[i]);
    Serial.write(points_y[i]);
  }
}
#endif
