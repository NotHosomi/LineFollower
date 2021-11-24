#if MAPPING_GRID
#include "Grid.h"
#include "defines.h"

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
  instance = this;
}

Grid::~Grid()
{
  for(int i = 0; i < BYTES; ++i)
  {
    tiles[i] = 0;
  }
  if(instance == this)
    instance = nullptr;
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

void Grid::setTile(float x, float y)
{
  setTile((int)(x / G_SCALE), (int)(y / G_SCALE));
}

void dump()
{
  Serial.write('G');
  Serial.write(char(G_HEIGHT));
  Serial.write(char(G_WIDTH));
  Serial.write(tiles);
}

#endif
