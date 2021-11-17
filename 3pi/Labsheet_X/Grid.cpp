#include "Grid.h"

Grid::~Grid()
{
  for(int i = 0; i < G_HEIGHT * G_WIDTH/8; ++i;)
  {
    tiles = 0;
  }
}

Grid::setTile(int x, int y)
{
  int index = y * G_WIDTH + x;
  index /= 8;
  int bindex = x % 8;
  // 128u is b10000000
  tiles[index] |= uchar(128) >> bindex;
}
