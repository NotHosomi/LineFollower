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

//tape line width is 18mm
// nyquist-theorum, if the tiles are half of size of the line, they should be accurate enough, perhaps?
#define G_SCALE 9 //mm

// note, ground sensors are 44mm from center
// side 12 deg
// far side 46 deg

class Grid
{
public:
  static Grid* instance;
  Grid();
  ~Grid();

  void setTile(int x, int y); // coords in tiles
  void setTile(float x, float y); // coords in mm
  unsigned char* getGrid() { return tiles; };
  
  
private:
  unsigned char tiles[BYTES];
};
//#endif
