/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    draw.h

Abstract:

    function declarations for draw.cpp

Author:

    Jason Gould (a-jasgou)   May 2000
    Josh Poley (jpoley)      Oct 2000

Revision History:

--*/
#ifndef __draw_h_
#define __draw_h_

namespace Launcher {

#define _UseD3d_ //d3d works now...
// #define _UseDxConio_ //define this to use dxconio

#define SCALEX(n) ((n)*(8/7))
#define SCALEY(n) ((n)*(3/2))
#define BUILDARGB(a, r, g, b) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))

void drInit(int width = 640, int height = 480);
void drDeinit();
void drBox(float x1, float y1, float x2, float y2, DWORD col);
void drLine(float x1, float y1, float x2, float y2, DWORD col);
void drSetSize(float width = 7, float height = 15, DWORD col = 0xffffffff, DWORD bkcol = 0);
int drPrintf(float x1, float y1, WCHAR* format, ...);
void drGetSize(float*pwidth, float*pheight, DWORD*pcolor, DWORD*pbkcolor);
void drShowScreen();
void drCls();
void drQuad(float x1, float y1, DWORD c1, float x2, float y2, DWORD c2, float x3, float y3, DWORD c3, float x4, float y4, DWORD c4);
DWORD drScaledFloatToRgba(float r, float g, float b, float a);	 //floats from 0=>1
DWORD drUnscaledFloatToRgba(float r, float g, float b, float a);//floats from 0=>255
DWORD drHslToRgb(float h, float s, float l, float al);

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER
    {
    unsigned __int16 bfType;
    unsigned __int32 bfSize;
    unsigned __int32 bfReserved1;
    unsigned __int32 bfOffBits;
    } BITMAPFILEHEADER, *PBITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER
    {
    unsigned __int32 biSize;
    unsigned __int32 biWidth;
    unsigned __int32 biHeight;
    unsigned __int16 biPlanes;
    unsigned __int16 biBitCount;
    unsigned __int32 biCompression;
    unsigned __int32 biSizeImage;
    unsigned __int32 biXPelsPerMeter;
    unsigned __int32 biYPelsPerMeter;
    unsigned __int32 biClrUsed;
    unsigned __int32 biClrImportant;
    } BITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagColorTable
    {
    unsigned __int8 blue;
    unsigned __int8 green;
    unsigned __int8 red;
    unsigned __int8 reserved;
    } ColorTable;
#pragma pack(pop)

class Bitmap
    {
    public:
        BITMAPFILEHEADER bitmapHeader;
        BITMAPINFOHEADER bitmapInfo;
        ColorTable *palatte;
        unsigned char *image;

    public:
        Bitmap();
        Bitmap(char *filename);
        ~Bitmap();

    public:
        DWORD LoadFile(char *filename);
        DWORD Draw(DWORD offsetX, DWORD offsetY, DWORD trans = 0xFFFF);
    };

} // namespace Launcher

#endif // __draw_h_