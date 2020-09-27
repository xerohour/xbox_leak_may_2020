/*--
Copyright (c) 2000 Microsoft Corporation - Xbox SDK

Module Name:

    draw.cpp

Abstract:

    Simple sample for drawing text on the screen.

Revision History:

    Derived from a DX8 sample.
--*/

#include <xtl.h>

#include "d3d8.h"
#include "bitfont.h"

//------------------------------------------------------------------------------
// Draw
//
// Handy little D3D encapsulation for simple 2D drawing.

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

//------------------------------------------------------------------------------
// Draw::m_pDevice
//
// Instantiate static class member, initialized to NULL.

IDirect3DDevice8* Draw::m_pDevice;

//------------------------------------------------------------------------------
// Draw constructor

Draw::Draw(INT width, INT height)
{
    m_pBackBuffer = NULL;

    // If we're invoked multiple times, we inherit the old mode (largely
    // because we don't support multiple mode sets on Xbox yet):

    if (m_pDevice == NULL)
    {
        // Create D3D 8.

        IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
        if (pD3D == NULL)
            return;

        // Set the screen mode.

        D3DPRESENT_PARAMETERS d3dpp;
        ZeroMemory(&d3dpp, sizeof(d3dpp));

        d3dpp.BackBufferWidth           = width;
        d3dpp.BackBufferHeight          = height;
        d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
        d3dpp.BackBufferCount           = 1;
        d3dpp.Windowed                  = false;    // Must be false for Xbox
        d3dpp.EnableAutoDepthStencil    = true;
        d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
        d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
        d3dpp.FullScreen_RefreshRateInHz= 60;
        d3dpp.hDeviceWindow             = NULL;

        // Create the device.

        if (pD3D->CreateDevice
        (
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            NULL,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp,
            &m_pDevice
        ) != S_OK)
        {
            pD3D->Release();

            return;
        }

        // Now we no longer need the D3D interface so let's free it.

        pD3D->Release();
    }

    m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
}

//------------------------------------------------------------------------------
// Draw::~Draw

Draw::~Draw()
{
    if (m_pBackBuffer)
        m_pBackBuffer->Release();
}

//------------------------------------------------------------------------------
// Draw::FillRect

VOID Draw::FillRect(
    INT x,
    INT y,
    INT width,
    INT height,
    D3DCOLOR color)
{
    D3DRECT rect;

    if (m_pDevice != NULL)
    {
        rect.x1 = x;
        rect.y1 = y;
        rect.x2 = x + width;
        rect.y2 = y + height;

        m_pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
    }
}

//------------------------------------------------------------------------------
// Draw::DrawText

VOID Draw::DrawText(
    const WCHAR* string,
    INT x,
    INT y,
    D3DCOLOR foregroundColor,   // 0xff0000 is red
    D3DCOLOR backgroundColor,
    DWORD flags)
{
    if (m_pBackBuffer != NULL)
    {
        m_font.DrawText(m_pBackBuffer, string, x, y, flags, foregroundColor,
                        backgroundColor);
    }
}

//------------------------------------------------------------------------------
// Draw::Present
//
// Make the current back-buffer visible

VOID Draw::Present()
{
    if (m_pDevice != NULL)
    {
        m_pDevice->Present(NULL, NULL, NULL, NULL);
    }
}

//------------------------------------------------------------------------------
// Test

extern "C" VOID DrawMessage(int x, int y, LPCWSTR pcszText, int iProgPercent, int xProg, int yProg)
{
    Draw draw;

    draw.FillRect(0, 0, 640, 480, 0x000080);            // Draw border in dark blue
    draw.FillRect(20, 20, 600, 440, 0);                 // Fill interior with black
    draw.DrawText(pcszText, x, y, 0xbbbbff);

    if (-1 != iProgPercent)
    {
        //
        // Draw progress bar background
        //

        draw.FillRect(xProg, yProg, 210, 40, 0x000080);

        //
        // Draw progress bar
        //

        draw.FillRect(xProg + 5, yProg + 5, 2 * min(iProgPercent, 100), 30, 0xbbbbff);
    }
    draw.Present();                                     // Show the buffer
}

