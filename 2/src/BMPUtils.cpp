#include "BMPUtils.h"

BMP BMPUtils::ReadFromPath(std::string path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);

	BMP::Header header;

	if (!file)
	{
		std::cout << "Failed to read from " + path + "." << std::endl;

		return BMP();
	}

	file.read(reinterpret_cast<char*>(&header.bmh), sizeof(header.bmh));
	file.read(reinterpret_cast<char*>(&header.bi), sizeof(header.bi));

	if (header.bmh.bfType != 0x4d42)
	{
		std::cout << "Invalid bfType." << std::endl;

		return BMP();
	}

	if (header.bi.biCompression != 0)
	{
		std::cout << "Uncompressed files aren't supported." << std::endl;

		return BMP();
	}

	file.seekg(header.bmh.bfOffBits, std::ios::beg);

	std::vector<std::vector<BMP::RGB> > pixels(header.bi.biWidth, std::vector<BMP::RGB>(header.bi.biWidth));

	for (unsigned int i = 0; i < header.bi.biHeight; i++) {
		for (unsigned int j = 0; j < header.bi.biWidth; j++) {
			file.read(reinterpret_cast<char*>(&pixels[i][j].r), sizeof(uint8_t));
			file.read(reinterpret_cast<char*>(&pixels[i][j].g), sizeof(uint8_t));
			file.read(reinterpret_cast<char*>(&pixels[i][j].b), sizeof(uint8_t));

			if (header.bi.biBitCount == 32)
			{
				file.read(reinterpret_cast<char*>(&pixels[i][j].a), sizeof(uint8_t));
			}
		}
	}

	return BMP(header, pixels);
}

void BMPUtils::WriteToPath(BMP* bmp, std::string path)
{
	std::ofstream fout(path, std::ios::out | std::ios::binary);

    auto info = bmp->GetHeader();

	fout.write(reinterpret_cast<char*>(&info), sizeof(BMP::Header));
	fout.seekp(info.bmh.bfOffBits, std::ios::beg);

	std::vector<std::vector<BMP::RGB> > pixels = bmp->GetPixels();

	for (std::vector<BMP::RGB> pixelLine : pixels)
	{
		for (BMP::RGB pixel : pixelLine)
		{
			fout.write(reinterpret_cast<char*>(&pixel.r), sizeof(uint8_t));
			fout.write(reinterpret_cast<char*>(&pixel.g), sizeof(uint8_t));
			fout.write(reinterpret_cast<char*>(&pixel.b), sizeof(uint8_t));

			if (bmp->GetHeader().bi.biBitCount == 32)
			{
				fout.write(reinterpret_cast<char*>(&pixel.a), sizeof(uint8_t));
			}
		}
	}
}

void BMPUtils::BlurByWidth(BMP* originalBMP, BMP* blurredBMP, int startWidth, int endWidth, int radius)
{
	float rs = std::ceil(radius * 2.57);

	for (int i = 0; i < originalBMP->GetHeight() - 1; ++i)
	{
		for (int j = startWidth; j <= endWidth; ++j)
		{
			double r = 0, g = 0, b = 0;
			double wsum = 0;

			for (int iy = i - rs; iy < i + rs + 1; ++iy)
			{
				for (int ix = j - rs; ix < j + rs + 1; ++ix)
				{
					auto x = std::min(static_cast<int>(endWidth) - 1, std::max(0, ix));
					auto y = std::min(static_cast<int>(originalBMP->GetHeight()) - 1, std::max(0, iy));

					auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
					auto wght = std::exp(-dsq / (2.0 * radius * radius)) / (3.1415926535897932 * 2.0 * radius * radius);

					BMP::RGB pixel = originalBMP->GetPixel(x, y);

					r += pixel.r * wght;
					g += pixel.g * wght;
					b += pixel.b * wght;

					wsum += wght;
				}
			}

			BMP::RGB pixel = originalBMP->GetPixel(j, i);
			BMP::RGB blurredPixel = BMP::RGB();

			blurredPixel.r = std::round(r / wsum);
			blurredPixel.g = std::round(g / wsum);
			blurredPixel.b = std::round(b / wsum);

			blurredBMP->SetPixel(j, i, blurredPixel);
		}
	}
}
