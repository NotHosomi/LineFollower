#include "defines.h"
#if MAPPING_GRID
#include "Grid.h"
# include <EEPROM.h>

#define OFFSET_X G_WIDTH/2
#define OFFSET_Y G_HEIGHT/10

Grid::Grid()
{
  // these should work exactly the same. First should be a tiny bit faster
  //for(unsigned char* p = tiles; p != &tiles[SIZE]; ++p
  //{
  //  *p = 0;
  //}
  for(int i = 0; i < BYTES; ++i)
  {
    tiles[i] = 0;
  }
}

void Grid::setTile(int x, int y)
{
  // move this sec to whereever this actually gets recorded perhaps?
  x += OFFSET_X;
  y += OFFSET_Y;
  if(x < 0 || y < 0 || x >= G_WIDTH || y >= G_HEIGHT)
    return;
  
  int index = y * G_WIDTH + x;
  index /= 8;
  int bindex = x % 8;
  // 128U is b00000000'00000000'00000000'00000000'10000000
  tiles[index] |= 128U >> bindex;
}

void Grid::setTile(double x, double y)
{
  setTile((int)(x / G_SCALE), (int)(y / G_SCALE));
}

void Grid::dump()
{
  Serial.write('G');
  Serial.write(char(G_HEIGHT));
  Serial.write(char(G_WIDTH));
  for(size_t i = 0; i < BYTES; ++i)
    Serial.write(tiles[i]);
}

void Grid::debug()
{
  Serial.write('H');
  Serial.write(char(G_HEIGHT));
  Serial.write(char(G_WIDTH));
  for(size_t i = 0; i < BYTES; ++i)
  {
    for(int b = 0; b < 8; ++b)
    {
      if((bool)(tiles[i] & 128u >> b))
        Serial.print('1');
      else
        Serial.print('0');
    }
    if(i % G_WIDTH == 0)
      Serial.print('\n');
  }      
}

#define USHRT_MAX 65535
void Grid::save()
{
  int address = 0;
  unsigned short contig = 0;
  bool current = 0;
  for(int i = 0; i < BYTES; ++i)
  {
    for(int b = 0; b < 8; ++b;
    {
      auto tile = (bool)(tiles[i] & 128u >> b);
      if(tile != current)
      {
        EEPROM.update(address++, *reinterpretive_cast<char*>(&contig));
        EEPROM.update(address++, *reinterpretive_cast<char*>((&contig) + 1));
        current = !current;
        contig = 0;
      }
      else
      {
        ++contig;
        if(contig == USHRT_MAX)
        {
          EEPROM.update(address++, *reinterpretive_cast<char*>(&contig));
          EEPROM.update(address++, *reinterpretive_cast<char*>((&contig) + 1));
          contig = 0;
          EEPROM.update(address++, *reinterpretive_cast<char*>(&contig));
          EEPROM.update(address++, *reinterpretive_cast<char*>((&contig) + 1));
        }
      }
      if(address == 1024)
        break;
      // todo check this all is correct
    }
  }
  if(address < 1020)
  {
    for(int i = 0; i<4; ++i)
      EEPROM.update(address++, char(0));
  }
  
}

void Grid::load()
{
  
}

#endif
