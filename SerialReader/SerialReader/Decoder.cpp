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

	switch (buffer[0])
	{
	case OP_GRID:
		decodeGrid(buffer, size);
		break;
	case OP_TRACE:
	case OP_EVENTS:
		throw std::invalid_argument("Opcode not yet implemented");
		break;
	default:
		throw std::invalid_argument("Invalid opcode value");
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
	for (int i = 0; i < height * width / 8; ++i)
	{
		for (int bit = 0; bit < 8; ++bit)
		{
			pixelbuffer.push_back(buffer[i] & 128u >> bit);
		}
	}

	// transfer data into bitmap
	BMP::BMP img(width / 8, height, false);
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			img.set_pixel(x, y, 255, 255, 255, 0);
	img.write("output.bmp");
}

void Decoder::decodeTrace(char* buffer, int size)
{
	int count = *reinterpret_cast<int*>(&buffer[0]);
	buffer = &buffer[sizeof(int)];
	std::vector<point> points;
	point mins, maxs;
	for (int i = 0; i < count; i += 2*sizeof(short))
	{
		points.emplace_back(*reinterpret_cast<int*>(&buffer[i]),
			*reinterpret_cast<int*>(&buffer[i + sizeof(short)]));

		if (points.back().x < mins.x)
			mins.x = points.back().x;
		else if (points.back().x > maxs.x)
			maxs.x = points.back().x;
		if (points.back().y > maxs.y)
			maxs.y = points.back().y;
		else if (points.back().y < mins.y)
			mins.y = points.back().y;
	}

	int width = maxs.x - mins.x;
	int height = maxs.y - mins.y;
	BMP::BMP img(width, height, false);
	img.set_pixel(points[0].x, points[0].y, 255, 0, 0, 0);
	img.set_pixel(points.back().x, points.back().y, 0, 255, 255, 0);
	for (point p : points)
	{
		img.set_pixel(p.x + 1, p.y, 255, 255, 255, 0);
		img.set_pixel(p.x - 1, p.y, 255, 255, 255, 0);
		img.set_pixel(p.x, p.y + 1, 255, 255, 255, 0);
		img.set_pixel(p.x, p.y - 1, 255, 255, 255, 0);
	}
	img.write("output.bmp");
}
