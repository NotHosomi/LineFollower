#pragma once
#include <vector>

namespace Decoder
{
	void decode(char* buffer, int size);

	void decodeGrid(char* buffer, int size);
	void decodeTrace(char* buffer, int size);
	void decodeVectors(char* buffer, int size);
	enum opcodes : char
	{
		OP_GRID = 'G',
		OP_TRACE = 'T',
		OP_EVENTS = 'E'
	};
	struct point
	{
		point() = default;
		point(short _x, short _y)
		{
			x = _x;
			y = _y;
		}
		short x = 0;
		short y = 0;
	};
};

