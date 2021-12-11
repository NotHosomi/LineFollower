#include "Trace.h"
#if MAPPING_TRACE
# include <EEPROM.h>

#if MAPPING_EVENT
void Trace::addPoint(int x, int y, double theta)
#else
void Trace::addPoint(int x, int y)
#endif
{
  if(count >= BYTES/sizeof(vec_t))
    return;

  int diff_x = x - last_x;
  int diff_y = y - last_y;
  if(diff_x * diff_x + diff_y * diff_y < MIN_DIST_SQR)
    return;
#if MAPPING_EVENT
  digitalWrite(PIN_BUZZ, LOW);
  if(abs(theta - last_theta) < THETA_THRESHOLD)
    return;
  digitalWrite(PIN_BUZZ, HIGH);
#endif
  
  points_x[count] = static_cast<vec_t>(diff_x);
  points_y[count] = static_cast<vec_t>(diff_y);
  ++count;
  last_x = x;
  last_y = y;
#if MAPPING_EVENT
  last_theta = theta;
#endif

}
void Trace::dump()
{
  load();
    
  // FOR SOME REASON, serial is inconsistent between Arduino and CPP when handling binary. 
//  Serial.write('T');
//  for(int i = 0; i < count; ++i)
//  {
//    Serial.write(points_x[i]);
//    Serial.write(points_y[i]);
//  }

#if MAPPING_EVENT
  Serial.write('E');
#else
  Serial.write('S');
#endif
  for(int i = 0; i < count; ++i)
  {
    Serial.print(" ");
    Serial.print((int)points_x[i]);
    Serial.print("_");
    Serial.print((int)points_y[i]);
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
  for(int i = 0; i < BYTES/sizeof(vec_t); ++i)
  {
    if(i == count && i < BYTES/sizeof(vec_t))
    {
      for(int j = 0; j < 2*sizeof(vec_t); ++j)
        EEPROM.update(address++, (char)0); // place the stop code
      break;
    }
    #if MAPPING_EVENT
    EEPROM.write(address++, points_x[i] >> 8);
    EEPROM.write(address++, points_x[i] & 0xFF);
    EEPROM.write(address++, points_y[i] >> 8);
    EEPROM.write(address++, points_y[i] & 0xFF);
    #else
    EEPROM.update(address++, points_x[i]);
    EEPROM.update(address++, points_y[i]);
    #endif
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
  count = BYTES/sizeof(vec_t);
  for(int i = 0; i < BYTES/sizeof(vec_t); ++i)
  {
  #if MAPPING_EVENT
    short byte1 = EEPROM.read(address++);
    short byte2 = EEPROM.read(address++);
    points_x[i] = (byte1 << 8) + byte2;
    
    byte1 = EEPROM.read(address++);
    byte2 = EEPROM.read(address++);
    points_y[i] = (byte1 << 8) + byte2;
  #else
    points_x[i] = EEPROM.read(address++);
    points_y[i] = EEPROM.read(address++);
  #endif
  
    if(points_x[i] == 0 && points_y[i] == 0)
    {
      count = i;
      break;
    }
  }
}

#if !MAPPING_EVENT
bool Trace::timer()
{
  if(millis()-t_mark > TRACE_TIME)
  {
    t_mark = millis();
    return true;
  }
  return false;
}
#endif

// write every point value in EEPROM
void Trace::debug()
{
  Serial.print("Values: ");
  for(int i = 0; i < BYTES; ++i)
  {
    Serial.print((int)(char)EEPROM.read(i));
    Serial.print(" ");
  }
  Serial.println();

  load();
  Serial.write('T');
  for(int i = 0; i < count; ++i)
  {
    Serial.print((int)points_x[i]);
    Serial.print("_");
    Serial.print((int)points_y[i]);
    Serial.print("  ");
  }
  Serial.println();
  Serial.write('T');
  for(int i = 0; i < count; ++i)
  {
    Serial.print(points_x[i]);
    Serial.print("_");
    Serial.print(points_y[i]);
    Serial.print("  ");
  }
  Serial.println();
}
#endif
