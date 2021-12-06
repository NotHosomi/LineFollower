#include "Trace.h"
#if MAPPING_TRACE
# include <EEPROM.h>

void Trace::addPoint(int x, int y)
{
  if(count >= BYTES/2)
    return;
    
  int diff_x = x - last_x;
  int diff_y = y - last_y;
  if(diff_x == 0 && diff_y == 0)
    return;
    
  points_x[count] = diff_x;
  points_y[count] = diff_y;
  ++count;
  last_x = x;
  last_y = y;

}
void Trace::dump()
{
  load();
    
  Serial.write('T');
  for(int i = 0; i < count; ++i)
  {
    Serial.write(points_x[i]);
    Serial.write(points_y[i]);
  }
}

void Trace::save()
{
  int address = 0;

  // write count into NVM
//  char buff[sizeof(count)] = "";
//  for(int i = 0; i < sizeof(count); ++i)
//  {
//    buff[i] = *reinterpret_cast<char*>(count);
//  }
//  for(int i = 0; i < sizeof(count); ++i)
//  {
//    EEPROM.update(address++, buff[i]);
//  }

  // store points to NVM
  for(int i = 0; i < BYTES/2; ++i)
  {
    if(i == count && i < BYTES/2)
    {
      EEPROM.update(address++, (char)0); // place the stop code
      EEPROM.update(address++, (char)0);
      break;
    }
    EEPROM.update(address++, points_x[i]);
    EEPROM.update(address++, points_y[i]);
  }
}
void Trace::load()
{
  int address = 0;
  
  // place the count into the NVM // Not using this, using stopping byte code instead, cuz displayed abnormal behaviour
//  char buff[sizeof(count)] = "";
//  for(int i = 0; i < sizeof(count); ++i)
//  {
//    buff[i] = EEPROM.read(address++);
//  }
//  count = *reinterpret_cast<unsigned short*>(buff);

  // extract points from NVM
  count = BYTES/2;
  for(int i = 0; i < BYTES/2; ++i)
  {
    points_x[i] = EEPROM.read(address++);
    points_y[i] = EEPROM.read(address++);
    if(points_x[i] == 0 && points_y[i] == 0)
    {
      count = i;
      break;
    }
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

// write every point value in EEPROM
void Trace::debug()
{
  Serial.print("Values: ");
  for(int i = sizeof(count); i < BYTES; ++i)
  {
    Serial.print((int)(char)EEPROM.read(i));
    Serial.print(" ");
  }
  Serial.println();
}
#endif
