#ifndef __DRAW_H__
#define __DRAW_H__

#include <d3d8.h>
#include "bitfont.h"

#define BACKGROUND_BLUE      0x000080
#define PITCH_BLACK          0x000000
#define LABEL_WHITE          0xffffff
#define BRIGHT_RED           0xcf0000
#define DARK_RED             0x2f0000
#define BRIGHT_GREEN         0x00cf00
#define DARK_GREEN           0x003f00
#define BRIGHT_YELLOW        0xcfcf00
#define DARK_YELLOW          0x2f3f00


class CDraw
{
private:

    static IDirect3DDevice8* m_pDevice;

    IDirect3DSurface8* m_pBackBuffer;

    BitFont m_font;

public:

    CDraw(INT width = 640, INT height = 480);

    ~CDraw();

    VOID FillRect(
        INT x,
        INT y,
        INT width,
        INT height,
        D3DCOLOR color);

    VOID FillCircle(
        INT x,
        INT y,
        INT r,
        D3DCOLOR color);

     VOID FillRectNow(
        INT x,
        INT y,
        INT width,
        INT height,
        D3DCOLOR color);


    VOID DrawText(
        const WCHAR* string,
        INT x,
        INT y,
        D3DCOLOR foregroundColor,           // 0xff0000 is red
        D3DCOLOR backgroundColor = 0,
        DWORD flags = DRAWTEXT_TRANSPARENTBKGND);
    
    VOID DrawTextNow(
        const WCHAR* string,
        INT x,
        INT y,
        D3DCOLOR foregroundColor,           // 0xff0000 is red
        D3DCOLOR backgroundColor = 0,
        DWORD flags = DRAWTEXT_TRANSPARENTBKGND);

    

    VOID Present();

    BOOL IsValid() { return m_pDevice != NULL; }
};


#endif __DRAW_H__
