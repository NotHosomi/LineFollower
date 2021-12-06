#pragma once
#include <vector>

namespace Decoder
{
	void decode(char* buffer, int size);

	void decodeGrid(char* buffer, int size);
	void decodeGridAlt(char* buffer, int size);
	void decodeTrace(char* buffer, int size);

	enum opcodes : char
	{
		OP_GRID = 'G',
		OP_GRID_ALT = 'H',
		OP_TRACE = 'T',
		OP_EVENTS = 'E'
	};
	struct Point
	{
		Point() = default;
		Point(short _x, short _y)
		{
			x = _x;
			y = _y;
		}
		int x = 0;
		int y = 0;
		Point operator+(const Point& lhs)
		{
			Point p;
			p.x = x + lhs.x;
			p.y = y + lhs.y;
			return p;
		}
	};
};

