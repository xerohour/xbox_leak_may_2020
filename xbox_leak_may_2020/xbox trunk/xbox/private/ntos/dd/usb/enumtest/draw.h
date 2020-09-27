#ifndef __DRAW_H__
#define __DRAW_H__

#include <d3d8.h>
#include "bitfont.h"

class Draw
{
private:

    static IDirect3DDevice8* m_pDevice;

    IDirect3DSurface8* m_pBackBuffer;

    BitFont m_font;

public:

    Draw(INT width = 640, INT height = 480);

    ~Draw();

    VOID FillRect(
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

    VOID Present();

    BOOL IsValid() { return m_pDevice != NULL; }
};


#endif __DRAW_H__
