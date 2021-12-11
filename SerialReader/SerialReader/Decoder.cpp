#include "Decoder.h"
#include <stdexcept>
#include "bitmap_format.h"
#include <string>
#include <chrono>


void Decoder::decode(char* buffer, int size)
{
	op = buffer[0];
	// trim the opcode from the buffer
	buffer[0] = 0;
	buffer = &buffer[1];
	--size;

	filename = getFilename(); // generate the filename to save data to
	switch (op)
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
		break;
	case OP_TRACE_ALT:
	case OP_EVENTS:
		std::cout << "decode: Trace2" << std::endl;
		decodeTraceAlt(buffer, size);
		break;
	default:
		std::cout << "invalid opcode, printing:\n";
		for (unsigned int i = 0; i < size; ++i)
		{
			std::cout << buffer[i];
		}
		std::cout << std::endl;
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
	img.write("output_grid.bmp");
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
	img.write("output_grid.bmp");
	std::cout << "Image written" << std::endl;
}

void Decoder::decodeTrace(char* buffer, int size)
{
	for (int i = 0; i < size; i += 2)
	{
		std::cout << (int)buffer[i] << "_" << (int)buffer[i + 1] << std::endl;
	}
	std::cout << std::endl;

	std::vector<Point> points;
	points.emplace_back(0, 0);
	Point mins, maxs;
	for (int i = 0; i < size; i += 2)
	{
		points.emplace_back((int)buffer[i], (int)buffer[i + 1]);
		if (points.back() == Point(0, 0))
		{
			std::cout << "Hit exit bytecode (" << points.back().x << " " << points.back().y << ") at address: " << i << std::endl; // Warning
			continue;
		}
		points.back() = points.back() + points[points.size() - 2]; // add this vec to the previous point to get the new point
		std::cout << "\t" << points.back().x << "\t" << points.back().y << std::endl;

		if (points.back().x < mins.x)
			mins.x = points.back().x;
		else if (points.back().x > maxs.x)
			maxs.x = points.back().x;
		if (points.back().y > maxs.y)
			maxs.y = points.back().y;
		else if (points.back().y < mins.y)
			mins.y = points.back().y;
	}

	const int margin = 10;
	int width = maxs.x - mins.x;
	int height = maxs.y - mins.y;
	BMP::BMP img(width+2*margin, height+2*margin, false);
	img.fill_region(0, 0, width + 2 * margin, height + 2 * margin, 255, 255, 255, 0);	// set background to white
	for (Point& p : points)
	{
		p.x += margin - mins.x;
		p.y += margin - mins.y;
	}
	for (Point& p : points)
	{
		drawCirc(img, p, 3);
	}
	img.fill_region(points[0].x - 3,   points[0].y - 3,   7, 7, 255, 0, 0, 0);	// colour first point
	img.fill_region(points.back().x-3, points.back().y-3, 7, 7, 0, 255, 0, 0);// colour final point
	img.write("output_trace.bmp");
}

void Decoder::decodeTraceAlt(char* buffer, int size)
{
	buffer[0] = 0;
	buffer = &buffer[1]; // trim initial delimiter from the buffer

	std::vector<Point> points;
	points.emplace_back(0, 0);
	Point mins, maxs;
	// convert buffer to a string
	std::string s = buffer;
	// Delimit the string into vectors
	size_t pos = 0;
	std::string pair;
	while ((pos = s.find(' ')) != std::string::npos) {
		pair = s.substr(0, pos);
		s.erase(0, pos + 1);
		// extract values from the pair
		pos = pair.find('_');
		if (pos == std::string::npos)
		{
			std::cout << "invalid pair substring (" << pair << ")" << std::endl;
			continue;
		}
		points.emplace_back(
			std::stoi(pair.substr(0, pos)),
			-std::stoi(pair.substr(pos + 1)) );
		
		
		// same as implementation for Binary decode from here on
		if (points.back() == Point(0, 0))
		{
			std::cout << "Hit exit bytecode (" << points.back().x << " " << points.back().y << ") at address: " << points.size() << std::endl; // Warning
			continue;
		}
		points.back() = points.back() + points[points.size() - 2]; // add this vec to the previous point to get the new point
		std::cout << "\t" << points.back().x << "\t" << points.back().y << std::endl;

		if (points.back().x < mins.x)
			mins.x = points.back().x;
		else if (points.back().x > maxs.x)
			maxs.x = points.back().x;
		if (points.back().y > maxs.y)
			maxs.y = points.back().y;
		else if (points.back().y < mins.y)
			mins.y = points.back().y;
	}
	for (auto w : waypoints)
	{
		if (w.x < mins.x)
			mins.x = w.x;
		else if (w.x > maxs.x)
			maxs.x = w.x;
		if (w.y > maxs.y)
			maxs.y = w.y;
		else if (w.y < mins.y)
			mins.y = w.y;
	}
	// save positions to a log file, just in case
	std::ofstream logfile((filename + ".log").c_str());
	for (auto p : points)
	{
		logfile << p.x << " " << p.y << "\n";
	}
	logfile.close();

	std::cout << "Drawing " << points.size() << " points" << std::endl;
	const int margin = 10;
	int width = maxs.x - mins.x;
	int height = maxs.y - mins.y;
	BMP::BMP img(width+2*margin, height+2*margin, false);
	img.fill_region(0, 0, width + 2 * margin, height + 2 * margin, 255, 255, 255, 0);	// set background to white

	// draw waypoints
	for (Point& w : waypoints)
	{
		w.x += margin - mins.x;
		w.y += margin - mins.y;
		drawCirc(img, w, 3);
		img.fill_region(w.x - 3, w.y - 3, 7, 7, 255, 0, 0, 0);
	}

	// draw path
	for (Point& p : points)
	{
		p.x += margin - mins.x;
		p.y += margin - mins.y;
	}
	for (Point& p : points)
	{
		drawCirc(img, p, 3);
	}
	img.fill_region(points[0].x - 3,   points[0].y - 3,   7, 7, 255, 0, 0, 0);	// colour first point
	img.fill_region(points.back().x-3, points.back().y-3, 7, 7, 0, 255, 0, 0);// colour final point

	// score the waypoints
	score(points, img);

	img.write((filename + ".bmp").c_str());
	std::cout << "Image saved" << std::endl;
}

void Decoder::drawCirc(BMP::BMP& img, Point c, int r)
{
	int len = 2 * r + 1;
	img.fill_region(c.x-r, c.y-r-1, len, 1, 0, 0, 0, 0); // top
	img.fill_region(c.x-r, c.y+r+1, len, 1, 0, 0, 0, 0); // bottom
	
	img.fill_region(c.x-r-1, c.y-r, 1, len, 0, 0, 0, 0); // top
	img.fill_region(c.x+r+1, c.y-r, 1, len, 0, 0, 0, 0); // top
}

// Evauation
bool Decoder::loadMap(char mapid)
{
	std::string addr = "maps/";
	addr += mapid;
	addr += ".txt";
	std::ifstream map(addr);
	if (!map)
	{
		std::cout << "Address " << addr << " not found";
		return false;
	}
		
	int x, y;
	while (map >> x >> y)
	{
		waypoints.emplace_back(x, y);
	}
	this->mapid = mapid;
	return true;
}

void Decoder::score(std::vector<Point> path, BMP::BMP& img)
{
	std::vector<float> scores;
	for (auto w : waypoints)
	{
		Point closest_p;
		scores.push_back(100000000000.0f); // just an absurdly large number
		for (auto p : path)
		{
			int diff_x = w.x - p.x;
			int diff_y = w.y - p.y;
			float diff_sqr = diff_x * diff_x + diff_y * diff_y;
			if (diff_sqr < scores.back())
			{
				scores.back() = diff_sqr;
				closest_p = p;
			}
		}
		img.fill_region(closest_p.x - 3, closest_p.y - 3, 7, 7, 255, 0, 255, 0); // shade points closest to each waypoint
	}
	if (scores.size() == 0)
		return;

	std::ofstream scorefile(filename + "_score.txt");
	std::cout << "Scores: \n";
	float sum = 0;
	for (float& s : scores)
	{
		s = sqrt(s);
		std::cout << s << std::endl;
		sum += s;
		scorefile << s << "\n";
	}
	sum = sum / scores.size();
	std::cout << "Avrg: " << sum;
	scorefile << sum << "\n";
	scorefile.close();
}

std::string Decoder::getFilename()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm time;
	localtime_s(&time, &tt);
	std::string filename = "data_";
	filename += op; 
	filename += "/";
	filename += mapid;
	filename += "/"
		+ std::to_string(time.tm_year + 1900) + "-"
		+ std::to_string(time.tm_mon + 1) + "-"
		+ std::to_string(time.tm_mday) + "_"
		+ std::to_string(time.tm_hour) + "-"
		+ std::to_string(time.tm_min) + "-"
		+ std::to_string(time.tm_sec);
	return filename;
}
