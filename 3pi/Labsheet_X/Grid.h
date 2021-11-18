#pragma once

// mem footprint (256 x 256)
//  256 x (8 x 32)
//  256 x 32  =  8480 bytes
// without bitset implementation:
//  256 x 256 = 67840 bytes
#define G_HEIGHT 256
#define G_WIDTH 256 // should be divisible by 8
#define SIZE G_HEIGHT * G_WIDTH/8


class Grid
{
public:
  Grid();
  ~Grid();

  void setTile(int x, int y);
  unsigned char* getGrid() { return tiles };
  
  
private:
  unsigned char[G_HEIGHT * G_WIDTH/8] tiles;
}
