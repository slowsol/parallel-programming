#pragma once

#include <vector>
#include <fstream>
#include <iostream>

class BMP
{
public:
#pragma pack(2)
	typedef struct
	{
		uint16_t bfType;
		uint32_t bfSize;
		uint16_t bfReserved1;
		uint16_t bfReserved2;
		uint32_t bfOffBits;
	} BITMAPFILEHEADER;
#pragma()

#pragma pack(2)
	typedef struct
	{
		uint32_t biSize;
		uint32_t biWidth;
		uint32_t biHeight;
		uint16_t biPlanes;
		uint16_t biBitCount;
		uint32_t biCompression;
		uint32_t biSizeImage;
		uint32_t biXPelsPerMeter;
		uint32_t biYPelsPerMeter;
		uint32_t biClrUsed;
		uint32_t biClrImportant;
	} BITMAPINFOHEADER;
#pragma()

	struct Header
	{
		BITMAPFILEHEADER bmh;
		BITMAPINFOHEADER bi;
	};

	struct RGB
	{
		uint8_t r, g, b, a;
	};

	BMP(Header header, std::vector<std::vector<RGB> > pixels);

	BMP() {};

	~BMP() {};

	uint32_t GetWidth();
	uint32_t GetHeight();

	RGB GetPixel(int x, int y);
	std::vector<std::vector<RGB> > GetPixels();
	void SetPixel(int x, int y, RGB pixel);

	Header GetHeader();

private:
	Header m_header;

	std::vector<std::vector<RGB> > m_pixels;
};