// header.h : include file for standard system include files,
// or project specific include files
//

#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>

#define ERR_CAPTION (TEXT("¿À·ù"))

enum
{
	SCREEN_WIDTH = 512, 
	SCREEN_HEIGHT = 512
};

#include "Vector.h"
#include "BitmapUtil.h"