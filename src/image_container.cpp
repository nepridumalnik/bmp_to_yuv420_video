#include "includes.hpp"
#include "image_container.hpp"

void Image_Container::load_bmp_tag(const char *bmpfilename, const char *yuvfilename)
{
	WORD lineSize;
	std::ifstream input(bmpfilename, std::ios::binary);
	if (!input.is_open())
		throw std::runtime_error("Unable to open file\n");

	input.read((char *)&bmp_file_header, sizeof(BMPFileHeader));
	input.read((char *)&bmp_header_info, sizeof(BMPHeaderInfo));

	if (bmp_file_header.bfType != 0x4D42 || bmp_header_info.biBitCount != 0x18)
		throw std::runtime_error("Unknown file format\n");

	databuf = (BYTE *)malloc(sizeof(BYTE) * bmp_header_info.biSizeImage);
	lineSize = bmp_header_info.biSizeImage / bmp_header_info.biHeight;

	for (int i = bmp_header_info.biSizeImage - lineSize; i >= 0; i -= lineSize)
		input.read((char *)&databuf[i], lineSize);

	input.close();
	distribute_threads();
}

void Image_Container::write(const char *yuvfilename)
{
	unsigned long count = 0;
	DWORD lineSize = bmp_header_info.biSizeImage / bmp_header_info.biHeight;
	std::ofstream output(yuvfilename);
	if (!output.is_open())
		throw std::runtime_error("Can not open file\n");
	for (unsigned long i = 0; i < bmp_header_info.biSizeImage; i += 3)
	{
		output.write((char *)&(yuv_canva[i].Y), sizeof(BYTE));
		count++;
		if (count == (bmp_header_info.biWidth))
		{
			count = 0;
			if ((i + 3) % lineSize != 0)
				i = i + (lineSize - (i + 1) % lineSize) - 2;
		}
	}
	for (unsigned long i = 0; i < bmp_header_info.biSizeImage; i += 6)
	{
		output.write((char *)&(yuv_canva[i].U), sizeof(BYTE));
		count++;
		if (count == (bmp_header_info.biWidth) / 2)
		{
			count = 0;
			if ((i + 3) % lineSize == 0 && (i + 3) >= lineSize)
				i = i + lineSize;
			else
				i = i + (lineSize - (i + 1) % lineSize) - 2 + lineSize;
		}
	}
	for (unsigned long i = 0; i < bmp_header_info.biSizeImage; i += 6)
	{
		output.write((char *)&(yuv_canva[i].V), sizeof(BYTE));
		count++;
		if (count == (bmp_header_info.biWidth) / 2)
		{
			count = 0;
			if ((i + 3) % lineSize == 0 && (i + 3) >= lineSize)
				i = i + lineSize;
			else
				i = i + (lineSize - (i + 1) % lineSize) - 2 + lineSize;
		}
	}
	output.close();
}

void Image_Container::write_yuvideo(const char *yuvfilename)
{
	unsigned long count = 0;
	DWORD lineSize = bmp_header_info.biSizeImage / bmp_header_info.biHeight;

	std::ifstream input(yuvfilename, std::ios::binary);
	if (!input.is_open())
		throw std::runtime_error("Can not open file\n");

	unsigned long long counter = 0;
	char e;
	while (!input.eof())
	{
		counter++;
		input.read(&e, sizeof(char));
	}
	input.close();
	unsigned long long new_counter = 0;
	std::ofstream output(yuvfilename);
	if (!output.is_open())
		throw std::runtime_error("Can not open file\n");
	while (true)
	{
		for (unsigned long i = 0; i < bmp_header_info.biSizeImage; i += 3)
		{
			new_counter++;
			if (new_counter >= counter)
			{
				goto HERE;
			}
			output.write((char *)&(yuv_canva[i].Y), sizeof(BYTE));

			count++;
			if (count == (bmp_header_info.biWidth))
			{
				count = 0;
				if ((i + 3) % lineSize != 0)
					i = i + (lineSize - (i + 1) % lineSize) - 2;
			}
		}
		for (unsigned long i = 0; i < bmp_header_info.biSizeImage; i += 6)
		{
			new_counter++;
			if (new_counter >= counter)
			{
				goto HERE;
			}
			output.write((char *)&(yuv_canva[i].U), sizeof(BYTE));
			count++;
			if (count == (bmp_header_info.biWidth) / 2)
			{
				count = 0;
				if ((i + 3) % lineSize == 0 && (i + 3) >= lineSize)
					i = i + lineSize;
				else
					i = i + (lineSize - (i + 1) % lineSize) - 2 + lineSize;
			}
		}
		for (unsigned long i = 0; i < bmp_header_info.biSizeImage; i += 6)
		{
			new_counter++;
			if (new_counter >= counter)
			{
				goto HERE;
			}
			output.write((char *)&(yuv_canva[i].V), sizeof(BYTE));
			count++;
			if (count == (bmp_header_info.biWidth) / 2)
			{
				count = 0;
				if ((i + 3) % lineSize == 0 && (i + 3) >= lineSize)
					i = i + lineSize;
				else
					i = i + (lineSize - (i + 1) % lineSize) - 2 + lineSize;
			}
		}
	}
HERE:
	output.close();
}

void Image_Container::distribute_threads()
{
	const char NUMBE_OF_THREADS = 6;
	yuv_canva = new YUV[bmp_header_info.biSizeImage];
	unsigned int elements_per_thread = bmp_header_info.biSizeImage / NUMBE_OF_THREADS;
	unsigned long start = 0;
	unsigned long end = start + elements_per_thread;

	std::vector<std::thread *> th(NUMBE_OF_THREADS);
	for (int i = 0; i < NUMBE_OF_THREADS; i++)
	{
		th[i] = new std::thread(
			[start, end, this] {
				convert_rgb2yuv(start, end);
			});
		start = end + 1;
		end += bmp_header_info.biSizeImage / NUMBE_OF_THREADS + 1;
		if (end > bmp_header_info.biSizeImage)
			end = bmp_header_info.biSizeImage;
	}
	for (int i = 0; i < th.size(); i++)
	{
		th[i]->join();
	}
	th.clear();
}

inline void Image_Container::convert_rgb2yuv(unsigned long start, unsigned long end)
{
	DWORD lineSize = bmp_header_info.biSizeImage / bmp_header_info.biHeight;
	if (start > 0)
		start -= 4;
	if (end < bmp_header_info.biSizeImage)
		end += lineSize;
	unsigned long count = 0;
	for (unsigned long i = start; i < end; i += 3)
	{
		yuv_canva[i].Y = 0.257 * databuf[i + 2] + 0.504 * databuf[i + 1] + 0.098 * databuf[i] + 16;
		count++;
		if (count == (bmp_header_info.biWidth))
		{
			count = 0;
			if ((i + 3) % lineSize != 0)
				i = i + (lineSize - (i + 1) % lineSize) - 2;
		}
	}
	for (unsigned long i = start; i < end; i += 6)
	{
		yuv_canva[i].U = -0.148 * databuf[i + 2] - 0.291 * databuf[i + 1] + 0.439 * databuf[i] + 128;
		count++;
		if (count == (bmp_header_info.biWidth) / 2)
		{
			count = 0;
			if ((i + 3) % lineSize == 0 && (i + 3) >= lineSize)
				i = i + lineSize;
			else
				i = i + (lineSize - (i + 1) % lineSize) - 2 + lineSize;
		}
	}
	for (unsigned long i = start; i < end; i += 6)
	{
		yuv_canva[i].V = 0.439 * databuf[i + 2] - 0.368 * databuf[i + 1] - 0.071 * databuf[i] + 128;
		count++;
		if (count == (bmp_header_info.biWidth) / 2)
		{
			count = 0;
			if ((i + 3) % lineSize == 0 && (i + 3) >= lineSize)
				i = i + lineSize;
			else
				i = i + (lineSize - (i + 1) % lineSize) - 2 + lineSize;
		}
	}
}