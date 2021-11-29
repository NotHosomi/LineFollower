#include "Decoder.h"
#include <stdexcept>
#include "bitmap_format.h"


void Decoder::decode(char* buffer, int size)
{
	char opcode = buffer[0];
	// trim the opcode from the buffer
	buffer[0] = 0;
	buffer = &buffer[1];
	--size;

	switch (opcode)
	{
	case OP_GRID:
		std::cout << "decode: Grid" << std::endl;
		decodeGrid(buffer, size);
		break;
	case OP_TRACE:
	case OP_VEC:
		throw std::invalid_argument("Opcode not yet implemented");
		break;
	default:
		//throw std::invalid_argument("Invalid opcode value");
		std::cout << "invalid opcode"  << opcode << std::endl;
		return;
		break;
	}
}

void Decoder::decodeGrid(char* buffer, int size)
{
	//TODO allow for int H&W
	//unsigned int width = *(std::reinterpret_cast<unsigned int*>(buffer));
	//unsigned int height = *(std::reinterpret_cast<unsigned int*>(buffer + sizeof(int)));
	unsigned char width = buffer[0];
	unsigned char height = buffer[1];
	buffer[0] = 0;
	buffer[1] = 0;
	buffer = &buffer[2];
	size -= 2;

	// extract compressed data
	std::vector<bool> pixelbuffer;
	pixelbuffer.reserve(height * width);
	int count = 0;
	for (int i = 0; i < height * width / 8; ++i)
	{
		for (int bit = 0; bit < 8; ++bit)
		{
			pixelbuffer.push_back(buffer[i] & 128u >> bit);
			if(buffer[i] & 128u >> bit)
				count++;
		}
	}
	std::cout << "WxH: " << (int)width << ", " << (int)height << std::endl;
	std::cout << "pixel count: " << count << std::endl;

	// transfer data into bitmap
	BMP::BMP img(width, height, false);

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if(pixelbuffer[width*y + x])
				img.set_pixel(x, y, 0, 0, 0, 0);
			else
				img.set_pixel(x, y, 255, 255, 255, 0);
		}
	img.write("output.bmp");
	std::cout << "Image written" << std::endl;
}
