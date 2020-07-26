#pragma once
#include "includes.hpp"

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#pragma pack(1)

struct BMPFileHeader
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffsetBytes;
};

struct BMPHeaderInfo
{
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPixelsPerMeter;
	DWORD biYPixelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
};
struct RGB
{
	BYTE B;
	BYTE G;
	BYTE R;
	BYTE Reserved;
};
struct YUV
{
	BYTE Y;
	BYTE U;
	BYTE V;
};
#pragma pack()