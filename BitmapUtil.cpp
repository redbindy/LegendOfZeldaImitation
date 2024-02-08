#include "BitmapUtil.h"

static void Compress(const char* inputPath, const char* outputPath, pixel_t colorKey);

void CompressBitmap()
{
	const char* inputPaths[] = {
		"./Resources/Title.bmp",
		"./Resources/Map.bmp",
		"./Resources/LinkFrame/LinkAttackDown0.bmp",
		"./Resources/LinkFrame/LinkAttackDown1.bmp",
		"./Resources/LinkFrame/LinkAttackDown2.bmp",
		"./Resources/LinkFrame/LinkAttackDown3.bmp",
		"./Resources/LinkFrame/LinkAttackSide0.bmp",
		"./Resources/LinkFrame/LinkAttackSide1.bmp",
		"./Resources/LinkFrame/LinkAttackSide2.bmp",
		"./Resources/LinkFrame/LinkAttackSide3.bmp",
		"./Resources/LinkFrame/LinkAttackUp0.bmp",
		"./Resources/LinkFrame/LinkAttackUp1.bmp",
		"./Resources/LinkFrame/LinkAttackUp2.bmp",
		"./Resources/LinkFrame/LinkAttackUp3.bmp",
		"./Resources/LinkFrame/MoveDown0.bmp",
		"./Resources/LinkFrame/MoveDown1.bmp",
		"./Resources/LinkFrame/MoveUp0.bmp",
		"./Resources/LinkFrame/MoveUp1.bmp",
		"./Resources/LinkFrame/MoveSide0.bmp",
		"./Resources/LinkFrame/MoveSide1.bmp",
		"./Resources/EnemyFrame/EnemyDown0.bmp",
		"./Resources/EnemyFrame/EnemyDown1.bmp",
		"./Resources/EnemyFrame/EnemySide0.bmp",
		"./Resources/EnemyFrame/EnemySide1.bmp",
		"./Resources/EnemyFrame/EnemyBullet.bmp"
	};

	const char* outputPaths[] = {
		"./Resources/Title.zc",
		"./Resources/Map.zc",
		"./Resources/LinkFrame/LinkAttackDown0.zc",
		"./Resources/LinkFrame/LinkAttackDown1.zc",
		"./Resources/LinkFrame/LinkAttackDown2.zc",
		"./Resources/LinkFrame/LinkAttackDown3.zc",
		"./Resources/LinkFrame/LinkAttackSide0.zc",
		"./Resources/LinkFrame/LinkAttackSide1.zc",
		"./Resources/LinkFrame/LinkAttackSide2.zc",
		"./Resources/LinkFrame/LinkAttackSide3.zc",
		"./Resources/LinkFrame/LinkAttackUp0.zc",
		"./Resources/LinkFrame/LinkAttackUp1.zc",
		"./Resources/LinkFrame/LinkAttackUp2.zc",
		"./Resources/LinkFrame/LinkAttackUp3.zc",
		"./Resources/LinkFrame/MoveDown0.zc",
		"./Resources/LinkFrame/MoveDown1.zc",
		"./Resources/LinkFrame/MoveUp0.zc",
		"./Resources/LinkFrame/MoveUp1.zc",
		"./Resources/LinkFrame/MoveSide0.zc",
		"./Resources/LinkFrame/MoveSide1.zc",
		"./Resources/EnemyFrame/EnemyDown0.zc",
		"./Resources/EnemyFrame/EnemyDown1.zc",
		"./Resources/EnemyFrame/EnemySide0.zc",
		"./Resources/EnemyFrame/EnemySide1.zc",
		"./Resources/EnemyFrame/EnemyBullet.zc"
	};

	Compress(inputPaths[0], outputPaths[0], 0x00000000);
	Compress(inputPaths[1], outputPaths[1], 0x00000000);

	for (int i = 2; i < 23; ++i)
	{
		Compress(inputPaths[i], outputPaths[i], COLOR_KEY);
	}
}

BYTE* LoadZC_malloc(const char* path)
{
	BYTE* retBuffer;
	FILE* fp = fopen(path, "rb");
	{
		fseek(fp, 0, SEEK_END);

		size_t fileLength = ftell(fp);

		rewind(fp);

		retBuffer = new BYTE[fileLength];
		fread(retBuffer, 1, fileLength, fp);
	}
	fclose(fp);

	return retBuffer;
}

static void Compress(const char* inputPath, const char* outputPath, pixel_t colorKey)
{
	FILE* fin = fopen(inputPath, "rb");
	FILE* fout = fopen(outputPath, "wb");
	{
		bitmap_header_t bmpHeader;
		fread(&bmpHeader, sizeof(bmpHeader), 1, fin);
		assert(bmpHeader.width <= SCREEN_WIDTH && bmpHeader.height <= SCREEN_HEIGHT);

		UINT SIZE_OF_PIXEL_DATA = bmpHeader.colorDepth / BYTE_BIT;

		BYTE* paRowData = new BYTE[SIZE_OF_PIXEL_DATA * bmpHeader.width];
		{
			for (int y = bmpHeader.height; y >= 0; --y)
			{
				fread(paRowData, SIZE_OF_PIXEL_DATA, bmpHeader.width, fin);

				BYTE* pRowData = paRowData;
				const BYTE* const P_END = paRowData + SIZE_OF_PIXEL_DATA * bmpHeader.width;

				compression_format_t cpfmt;
				cpfmt.rowNumber = y;
				cpfmt.startIndex = 0;
				cpfmt.endIndex = 0;
				cpfmt.pixel = ((pixel_t)(*pRowData) | (pixel_t)(pRowData[1] << 8) | (pixel_t)(pRowData[2] << 16));

				pRowData += SIZE_OF_PIXEL_DATA;

				while (pRowData < P_END)
				{
					pixel_t pixel = ((pixel_t)(*pRowData) | (pixel_t)(pRowData[1] << 8) | (pixel_t)(pRowData[2] << 16));

					if (cpfmt.pixel == pixel)
					{
						++cpfmt.endIndex;
					}
					else
					{
						if (cpfmt.pixel != 0 && cpfmt.pixel != colorKey)
						{
							fwrite(&cpfmt, 1, sizeof(compression_format_t), fout);
						}

						cpfmt.pixel = pixel;
						cpfmt.startIndex = cpfmt.endIndex + 1;
						cpfmt.endIndex = cpfmt.startIndex;
					}

					pRowData += SIZE_OF_PIXEL_DATA;
				}
			}
			compression_format_t cpfmtEndData = { 0, };
			fwrite(&cpfmtEndData, 1, sizeof(compression_format_t), fout);
		}
		delete[] paRowData;
	}
	fclose(fin);
	fclose(fout);
}
