#include "Trace.h"
#if MAPPING_TRACE
# include <EEPROM.h>

void Trace::addPoint(int x, int y)
{
  if(count == BYTES/2)
    return;
  points_x[count] = x - last_x;
  points_y[count] = y - last_y;
}
void Trace::dump()
{
  if(!loaded)
    load();
    
  Serial.write('T');
  Serial.write(count);
  for(int i = 0; i < count; ++i)
  {
    Serial.write(points_x[i]);
    Serial.write(points_y[i]);
  }
}

void Trace::save()
{
  int address = 0;
  for(int i = 0; i < BYTES/2; ++i)
  {
    EEPROM.update(address++, points_x[i]);
    EEPROM.update(address++, points_y[i]);
  }
}
void Trace::load()
{
  int address = 0;
  for(int i = 0; i < BYTES/2; ++i)
  {
    points_x[i] = EEPROM.read(address++);
    points_y[i] = EEPROM.read(address++);
  }
}

bool Trace::timer()
{
  if(millis()-t_mark > TRACE_TIME)
  {
    t_mark = millis();
    return true;
  }
  return false;
}
void Trace::debug()
{
  Serial.write('T');
  for(int i = 0; i < BYTES; ++i)
  {
    Serial.write(EEPROM.read(i));
  }
}
#endif
