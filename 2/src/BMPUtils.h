#include "BMP.h"

#include <algorithm>
#include <cmath>

class BMPUtils
{
public:
	static BMP ReadFromPath(std::string path);
	static void WriteToPath(BMP* bmp, std::string path);

	static void BlurByWidth(BMP* originalBMP, BMP* blurredBMP, int startWidth, int endWidth, int radius = 3);
};