/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    draw.h

Abstract:

    function declarations for draw.cpp

Author:

    Jason Gould (a-jasgou)   May 2000

Revision History:

--*/
#ifndef __draw_h_
#define __draw_h_

#define _UseD3d_ //d3d works now...
// #define _UseDxConio_ //define this to use dxconio

//#include "cpx.h"

#define SCALEX(n) ((n)*(8/7))
#define SCALEY(n) ((n)*(3/2))

void drInit(int width = 640, int height = 480);
void drDeinit();
void drBox(float x1, float y1, float x2, float y2, DWORD col);
void drLine(float x1, float y1, float x2, float y2, DWORD col);
void drSetSize(float width = 7, float height = 15, DWORD col = 0xffffffff, DWORD bkcol = 0);
int drPrintf(float x1, float y1, WCHAR* format, ...);
void drGetSize(float*pwidth, float*pheight, DWORD*pcolor, DWORD*pbkcolor);
void drShowScreen();
void drBitBlt( float fx, float fy, ULONG ulWidth, ULONG ulHeight, const BYTE* pData );
void drCls();
void drQuad(float x1, float y1, DWORD c1, float x2, float y2, DWORD c2, float x3, float y3, DWORD c3, float x4, float y4, DWORD c4);
DWORD drScaledFloatToRgba(float r, float g, float b, float a);	 //floats from 0=>1
DWORD drUnscaledFloatToRgba(float r, float g, float b, float a);//floats from 0=>255
DWORD drHslToRgb(float h, float s, float l, float al);
int drGetStrSize(LPCWSTR str, int len);

#endif