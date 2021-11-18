#pragma once
//#if MAPPING_GRID


// example grid mem footprint (256 x 256) = 256 x (8 x 32)
// without bitset implementation:
//  256 x 256 = 67840 bytes // WAYYY too large
// WITH bitset:
//  256 x 32  =  8480 bytes // Still too large, but considerably better


// Max var mem is 2560
// 256 taken up by dynamic memory at time of writing
// that leaves 2295 bytes of locals
// 128 x 10 = 1280. Half of memory
// thats a gridspace of 128 x 80
#define G_HEIGHT 128
#define G_WIDTH 80 // should be divisible by 8
#define BYTES G_HEIGHT * G_WIDTH/8


class Grid
{
public:
  Grid();
  ~Grid();

  void setTile(int x, int y);
  unsigned char* getGrid() { return tiles; };
  
  
private:
  unsigned char tiles[BYTES];
};
//#endif
