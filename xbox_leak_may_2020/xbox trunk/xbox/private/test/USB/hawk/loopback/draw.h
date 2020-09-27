#ifndef __DRAW_H__
#define __DRAW_H__

#include <d3d8.h>
#include "bitfont.h"

#define BACKDROP_BLUE 0x000080
#define PITCH_BLACK 0x0
#define LABEL_WHITE 0xffffff
#define DISCONNECTED_BLUE 0x000040
#define CONNECTED_YELLOW 0xffff00

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
