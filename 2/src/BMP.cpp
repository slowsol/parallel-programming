#include "BMP.h"

BMP::BMP(Header header, std::vector<std::vector<RGB> > pixels)
	:m_header(header)
	,m_pixels(pixels)
{
}

uint32_t BMP::GetWidth()
{
	return m_header.bi.biWidth;
}

uint32_t BMP::GetHeight()
{
	return m_header.bi.biHeight;
}

BMP::RGB BMP::GetPixel(int x, int y)
{
	return m_pixels[y][x];
}

std::vector<std::vector<BMP::RGB> > BMP::GetPixels()
{
	return m_pixels;
}

void BMP::SetPixel(int x, int y, RGB pixel)
{
	m_pixels[y][x] = pixel;
}

BMP::Header BMP::GetHeader()
{
	return m_header;
}