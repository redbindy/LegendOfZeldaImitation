#pragma once

#include "framework.h"

enum
{
	BYTE_BIT = 8, 
	COLOR_KEY = 0x00747474
};

typedef UINT pixel_t;

#pragma pack(push, 1)
typedef struct
{
	WORD signature;
	DWORD size;
	WORD reserved0;
	WORD reserved1;
	DWORD offset;
	DWORD headerSize;
	int width;
	int height;
	WORD colorPlane;
	WORD colorDepth; // bits per pixel
	DWORD compressionMethod;
	DWORD imageSize;
	int horizontalResolution;
	int verticalResolution;
	DWORD colorPaletteCount;
	DWORD importantColorCount;
} bitmap_header_t;
#pragma pack(pop)
static_assert(sizeof(bitmap_header_t) == 54, "Only use 54bytes header");

#pragma pack(push, 1)
typedef struct
{
	WORD rowNumber;
	pixel_t pixel;
	WORD startIndex;
	WORD endIndex;
} compression_format_t;
#pragma pack(pop)

void CompressBitmap();
BYTE* LoadZC_malloc(const char* path);