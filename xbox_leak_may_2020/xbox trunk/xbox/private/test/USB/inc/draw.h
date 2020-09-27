#ifndef __DRAW_H__
#define __DRAW_H__

#include <d3d8.h>
#include <xbfont.h>

#define PITCH_BLACK          0xff000000
#define INVALID_GRAY         0xff101010
#define LABEL_WHITE          0xffffffff
#define CONNECTED_YELLOW     0xffffff00
#define ACCEPTED_GREEN       0xff009f00
#define REJECTED_RED         0xff9f0000
#define BRIGHT_BLUE          0xff2020ff


class CDraw
{

private:

    static IDirect3DDevice8* m_pDevice;
    IDirect3DSurface8* m_pBackBuffer;

public:

    CDraw(INT width = 640, INT height = 480);

    ~CDraw();

    VOID FillRect(
        INT x,
        INT y,
        INT width,
        INT height,
        D3DCOLOR color);
   
    CXBFont *CreateFont(const CHAR* strFontResourceFileName);

    VOID Present();

    BOOL IsValid() { return m_pDevice != NULL; }
};


#endif __DRAW_H__
