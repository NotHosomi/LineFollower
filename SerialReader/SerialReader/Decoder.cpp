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
	case OP_GRID_ALT:
		std::cout << "decode: Grid2" << std::endl;
		decodeGridAlt(buffer, size);
		break;
	case OP_TRACE:
		std::cout << "decode: Trace" << std::endl;
		decodeTrace(buffer, size);
	case OP_EVENTS:
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
	int pixels = width * height;
	std::cout << 3 + pixels / 8 << " bytes expected" << std::endl;
	std::cout << "WxH: " << (int)width << ", " << (int)height << std::endl;

	buffer[0] = 0;
	buffer[1] = 0;
	buffer = &buffer[2];
	size -= 2;

	// extract compressed data
	std::vector<bool> pixelbuffer;
	pixelbuffer.reserve(pixels);
	int count = 0;
	for (int i = 0; i < pixels / 8; ++i)
	{
		for (int bit = 0; bit < 8; ++bit)
		{
			bool p = buffer[i] & 128u >> bit;
			pixelbuffer.push_back(p);

			if (p)
			{
				count++;
				std::cout << '1';
			}
			else
			{
				std::cout << '0';
			}
		}
	}
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

void Decoder::decodeGridAlt (char* buffer, int size)
{
	unsigned char width = buffer[0];
	unsigned char height = buffer[1];
	int bytes = width * height;
	std::cout << bytes + 3 << " bytes expected" << std::endl;
	std::cout << "WxH: " << (int)width << ", " << (int)height << std::endl;

	std::vector<bool> pixelbuffer;
	pixelbuffer.reserve(width * height);
	int count = 0;
	for (int i = 2; i < height * width + 2; ++i)
	{
		bool p = buffer[i] == '1';
		pixelbuffer.push_back(p);
		if (p)
		{
			++count;
			std::cout << '1';
		}
		else
			std::cout << '0';
		if ((i - 2) % width == 0)
			std::cout << std::endl;
	}
	std::cout << "pixel count: " << count << std::endl;

	// transfer data into bitmap
	BMP::BMP img(width, height, false);

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (pixelbuffer[width * y + x])
				img.set_pixel(x, y, 0, 0, 0, 0);
			else
				img.set_pixel(x, y, 255, 255, 255, 0);
		}
	img.write("output.bmp");
	std::cout << "Image written" << std::endl;
}

void Decoder::decodeTrace(char* buffer, int size)
{
	int count = *reinterpret_cast<int*>(&buffer[0]);
	buffer = &buffer[sizeof(int)];
	std::vector<Point> points;
	Point mins, maxs;
	for (int i = 0; i < count; i += 2*sizeof(short))
	{
		//points.emplace_back(*reinterpret_cast<int*>(&buffer[i]),
		//	*reinterpret_cast<int*>(&buffer[i + sizeof(short)]));
		points.emplace_back((int)buffer[i], (int)buffer[i + 1]);
		points.back() = points.back() + points[points.size - 2]; // add this vec to the previous point to get the new point

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
	BMP::BMP img(width+2, height+2, false);
	img.set_pixel(points[0].x, points[0].y, 255, 0, 0, 0);
	for (Point p : points)
	{
		p.x += 1 - mins.x;
		p.y += 1 - mins.y;
		img.set_pixel(p.x + 1, p.y, 255, 255, 255, 0);
		img.set_pixel(p.x - 1, p.y, 255, 255, 255, 0);
		img.set_pixel(p.x, p.y + 1, 255, 255, 255, 0);
		img.set_pixel(p.x, p.y - 1, 255, 255, 255, 0);
	}
	img.set_pixel(points.back().x, points.back().y, 0, 255, 255, 0);
	img.write("output.bmp");
}
