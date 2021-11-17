#pragma once
#define G_HEIGHT 64
#define G_WIDTH 32 // should be divisible by 8
#SIZE G_HEIGHT * G_WIDTH/8

class Grid
{
public:
  Grid() = default;
  ~Grid();

  setTile(int x, int y);
  char* getGrid() { return tiles };
  
  
private:
  char[G_HEIGHT * G_WIDTH/8] tiles;
}
