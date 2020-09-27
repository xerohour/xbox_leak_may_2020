/*--
Copyright (c) 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Simple sample for drawing text on the screen.

Revision History:

    Derived from a DX8 sample.
--*/

#include <xtl.h>
#include "draw.h"

//------------------------------------------------------------------------------
// CDraw
//
// Handy little D3D encapsulation for simple 2D drawing.


//------------------------------------------------------------------------------
// CDraw::m_pDevice
//
// Instantiate static class member, initialized to NULL.

IDirect3DDevice8* CDraw::m_pDevice;

//------------------------------------------------------------------------------
// CDraw constructor

CDraw::CDraw(INT width, INT height)
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
// CDraw::~CDraw

CDraw::~CDraw()
{
    if (m_pBackBuffer)
        m_pBackBuffer->Release();
}

//------------------------------------------------------------------------------
// CDraw::FillRect

VOID CDraw::FillRect(
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

VOID CDraw::FillRectNow(
    INT x,
    INT y,
    INT width,
    INT height,
    D3DCOLOR color)
{
    FillRect(x,y,width,height,color);
    Present();
    FillRect(x,y,width,height,color);
}
//------------------------------------------------------------------------------
// CDraw::DrawText

VOID CDraw::DrawText(
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

VOID CDraw::DrawTextNow(
    const WCHAR* string,
    INT x,
    INT y,
    D3DCOLOR foregroundColor,   // 0xff0000 is red
    D3DCOLOR backgroundColor,
    DWORD flags)
{
    DrawText(string,x,y,foregroundColor,backgroundColor,flags);
    Present();
    DrawText(string,x,y,foregroundColor,backgroundColor,flags);
}
//------------------------------------------------------------------------------
// CDraw::Present
//
// Make the current back-buffer visible

VOID CDraw::Present()
{
    if (m_pDevice != NULL)
    {
        m_pDevice->Present(NULL, NULL, NULL, NULL);
    }
}

