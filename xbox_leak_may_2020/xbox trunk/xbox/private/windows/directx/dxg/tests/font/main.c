/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    App to test TV output

--*/

#define XFONT_TRUETYPE 1

#include <xtl.h>
#include <xgraphics.h>
#include <xfont.h>
#include <stdio.h>
#include <xbdm.h>
#include <av.h>
#include <xfont.h>

#define THETEXT L"I'm a little teapot, short and stout."

D3DSurface *g_pBack;
D3DSurface *g_pZBuffer;
D3DSurface *g_pTextureSurface;

HANDLE g_InputHandle;

BOOL g_A;
BOOL g_B;
BOOL g_X;
BOOL g_Y;

typedef enum _WHICHFONT {

    WHICH_Default,
    WHICH_Uni0,
    WHICH_Uni2,
    WHICH_Uni4,

    WHICH_Max,

} WHICHFONT;

WHICHFONT g_WhichFont;

DWORD g_Sizes[] =
{
    16,
    24,
    32,
    40,
    48,
    56,
    60,
    65,
    70,
    75,
    80,
    90,
    100,
    110,
    150,
    180,
    200
};

DWORD g_WhichSize;

XFONT *g_pFont;

XFONT *g_pDefaultFont;
XFONT *g_pUnicodeFont;
XFONT *g_pStatusFont;
       
typedef struct _MYFORMAT
{
    D3DFORMAT Format;
    DWORD ClearValue;
    DWORD BytesPerPixel;
    WCHAR *Name;
} MYFORMAT;

D3DFORMAT g_FrameFormats[] = 
{ 
    D3DFMT_X8R8G8B8, 
    D3DFMT_A8R8G8B8, 
    D3DFMT_R5G6B5, 
    D3DFMT_X1R5G5B5 
};

DWORD g_FrameFormat;

MYFORMAT g_TextureFormats[] = 
{ 
    { D3DFMT_LIN_A1R5G5B5,     0x0300, 2, L"A1R5G5B5" },
    { D3DFMT_LIN_R5G6B5,       0x0600, 2, L"R5G6B5"   },
    { D3DFMT_LIN_A8R8G8B8, 0x8000C000, 4, L"A8R8G8B8" },
    { D3DFMT_LIN_R8B8,         0xC000, 2, L"R8B8"     },
    { D3DFMT_LIN_G8B8,         0xC000, 2, L"G8B8"     },
    { D3DFMT_LIN_X1R5G5B5,     0x0300, 2, L"X1R5G5B5" },
    { D3DFMT_LIN_A4R4G4B4,     0x80C0, 2, L"A4R4G4B4" },
    { D3DFMT_LIN_X8R8G8B8, 0x0000C000, 4, L"X8R8G8B8" },
    { D3DFMT_LIN_A8,             0x80, 1, L"A8"       },
    { D3DFMT_LIN_R5G5B5A1,     0x0600, 2, L"R5G5B5A1" }, 
    { D3DFMT_LIN_R4G4B4A4,     0x0C08, 2, L"R4G4B4A4" },
    { D3DFMT_LIN_A8B8G8R8, 0x8000C000, 4, L"A8B8G8R8" },
    { D3DFMT_LIN_B8G8R8A8, 0x00C00080, 4, L"B8G8R8A8" },
    { D3DFMT_LIN_R8G8B8A8, 0x00C00080, 4, L"R8G8B8A8" },
};

DWORD g_TextureFormat;


D3DRECT g_rect = { 40, 40, 601, 441 };

long g_x = 320;
long g_y = 240;

DWORD g_Color = 0x000000FF;

BOOL g_DrawWithTexture;

static struct TheVerts { float x,y,z,w,tu,tv; } g_Verts[] =
{
    {   0.0f,   0.0f, 0.5f, 1.0f,   0.0f,   0.0f},
    { 639.0f,   0.0f, 0.5f, 1.0f, 511.0f,   0.0f},
    { 639.0f, 479.0f, 0.5f, 1.0f, 511.0f, 511.0f},
    {   0.0f, 479.0f, 0.5f, 1.0f,   0.0f, 511.0f},
};

DWORD g_dwFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

//==============================================================================
// Graphics
//==============================================================================

//------------------------------------------------------------------------------
// Creates a texture.
//                                                                                                                                                                  
void CreateTexture()
{
    D3DTexture *pTexture;

    D3DDevice_CreateTexture(512, 512, 1, 0, g_TextureFormats[g_TextureFormat].Format, 0, &pTexture);

    if (g_pTextureSurface)
    {
        D3DSurface_Release(g_pTextureSurface);
    }

    D3DTexture_GetSurfaceLevel(pTexture, 0, &g_pTextureSurface);

    D3DDevice_SetTexture(0, (D3DBaseTexture *)pTexture);

    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);

    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

    D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    D3DDevice_SetRenderState(D3DRS_ZENABLE, FALSE);
    D3DDevice_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        
    D3DTexture_Release(pTexture);
}

//------------------------------------------------------------------------------
// Loads a font.
//
DWORD g_CurrentTextHeight;

void
LoadFont()
{
    if (!g_pUnicodeFont)
    {
        g_WhichFont = WHICH_Default;
    }

    switch(g_WhichFont)
    {
    case WHICH_Default:
        g_pFont = g_pDefaultFont;
        g_WhichSize = 1;
        break;

    case WHICH_Uni0:
        g_pFont = g_pUnicodeFont;
        XFONT_SetTextAntialiasLevel(g_pFont, 0);
        break;

    case WHICH_Uni2:
        g_pFont = g_pUnicodeFont;
        XFONT_SetTextAntialiasLevel(g_pFont, 2);
        break;

    case WHICH_Uni4:
        g_pFont = g_pUnicodeFont;
        XFONT_SetTextAntialiasLevel(g_pFont, 4);
        break;
    }    

    if (g_WhichFont != WHICH_Default && g_WhichSize != g_CurrentTextHeight)
    {
        XFONT_SetTextHeight(g_pFont, g_Sizes[g_WhichSize]);
        g_CurrentTextHeight = g_WhichSize;
    }
}

//------------------------------------------------------------------------------
// Intialize D3D.
//
void
Init()
{
    D3DPRESENT_PARAMETERS d3dpp;

    // Initialize the controller.
    XInitDevices(0, NULL);

    // Make D3D go.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = g_FrameFormats[g_FrameFormat];
    d3dpp.BackBufferCount           = 1;
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;

    // Create the device.
    Direct3D_CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        NULL);

    // Get the buffer to draw into.
    D3DDevice_GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_pBack);
    D3DDevice_GetDepthStencilSurface(&g_pZBuffer);

    // Get the default font.
    XFONT_OpenDefaultFont(&g_pDefaultFont);
    XFONT_Release(g_pDefaultFont);

    XFONT_OpenDefaultFont(&g_pDefaultFont);

    XFONT_SetTextAlignment(g_pDefaultFont, XFONT_CENTER | XFONT_BASELINE);
    XFONT_SetTextColor(g_pDefaultFont, 0xC0FFFFFF);

    // Get the utility font.
    XFONT_OpenDefaultFont(&g_pStatusFont);
    XFONT_SetTextColor(g_pStatusFont, 0xFFFFFF00);

    // Get the unicode font.
    XFONT_OpenTrueTypeFont(L"d:\\media\\arialuni.ttf", 2 * 1024 * 1024, &g_pUnicodeFont);

    if (g_pUnicodeFont)
    {
        XFONT_SetTextAlignment(g_pUnicodeFont, XFONT_CENTER | XFONT_BASELINE);
        XFONT_SetTextColor(g_pUnicodeFont, 0xC0FFFFFF);
    }

    // Prepare to render.
    D3DDevice_SetVertexShader(g_dwFVF);

    // Create the texture.
    CreateTexture();

    // Set the first font.
    LoadFont();
}


//------------------------------------------------------------------------------
// Draw one frame.
//
void
Paint()
{    
    D3DLOCKED_RECT rect;
    DWORD *pDW;
    WORD *pW;
    BYTE *pB;
    DWORD x, y;

    WCHAR Buffer[80];

    if (g_DrawWithTexture)
    {
        D3DSurface_LockRect(g_pTextureSurface, &rect, NULL, 0);
         
        switch (g_TextureFormats[g_TextureFormat].BytesPerPixel)
        {
        case 4:
            for (y = 0; y < 512; y++)
            {
                pDW = (DWORD *)((BYTE *)rect.pBits + rect.Pitch * y);

                for (x = 0; x < 512; x++)
                {
                    *(pDW++) = (DWORD)g_TextureFormats[g_TextureFormat].ClearValue;
                }
            }
            break;

        case 2:
            for (y = 0; y < 512; y++)
            {
                pW = (WORD *)((BYTE *)rect.pBits + rect.Pitch * y);

                for (x = 0; x < 512; x++)
                {
                    *(pW++) = (WORD)g_TextureFormats[g_TextureFormat].ClearValue;
                }
            }
            break;

        case 1:
            for (y = 0; y < 512; y++)
            {
                pB = (BYTE *)((BYTE *)rect.pBits + rect.Pitch * y);

                for (x = 0; x < 512; x++)
                {
                    *(pB++) = (BYTE)g_TextureFormats[g_TextureFormat].ClearValue;
                }
            }
            break;
        }

        XFONT_SetClippingRectangle(g_pFont, NULL);
        XFONT_TextOut(g_pFont, g_pTextureSurface, THETEXT, -1, g_x, g_y);

//        D3DDevice_SetRenderTarget(g_pBack, g_pZBuffer);

        D3DDevice_Clear(0, NULL, D3DCLEAR_TARGET, g_Color, 1.0, 0);
        D3DDevice_Clear(1, &g_rect, D3DCLEAR_TARGET, g_Color & 0x00808080, 1.0, 0);

        D3DDevice_DrawVerticesUP(D3DPT_TRIANGLEFAN, sizeof(g_Verts) / sizeof(g_Verts[0]), g_Verts, sizeof(g_Verts[0]));
    }
    else
    {
        D3DDevice_Clear(0, NULL, D3DCLEAR_TARGET, g_Color, 1.0, 0);
        D3DDevice_Clear(1, &g_rect, D3DCLEAR_TARGET, g_Color & 0x00808080, 1.0, 0);

        XFONT_SetClippingRectangle(g_pFont, &g_rect);
        XFONT_TextOut(g_pFont, g_pBack, THETEXT, -1, g_x, g_y);
    }

    wcscpy(Buffer, L"FONT: ");
    switch(g_WhichFont)
    {
    case WHICH_Default:
        wcscat(Buffer, L"DEFAULT");
        break;
    case WHICH_Uni0:
        wcscat(Buffer, L"UNICODE-0");
        break;
    case WHICH_Uni2:
        wcscat(Buffer, L"UNICODE-2");
        break;
    case WHICH_Uni4:
        wcscat(Buffer, L"UNICODE-4");
        break;
    }

    XFONT_TextOut(g_pStatusFont, g_pBack, Buffer, -1, 50, 50);

    wcscpy(Buffer, L"BACK BUFFER: ");
    switch(g_FrameFormat)
    {
    case 0:
        wcscat(Buffer, L"X8R8G8B8");
        break;
    case 1:
        wcscat(Buffer, L"A8R8G8B8");
        break; 
    case 2:
        wcscat(Buffer, L"R5G6B5");
        break;
    case 3:
        wcscat(Buffer, L"X1R5G5B5");
        break;
    }

    XFONT_TextOut(g_pStatusFont, g_pBack, Buffer, -1, 280, 50);

    wsprintf(Buffer, L"SIZE: %d", g_Sizes[g_WhichSize]);

    XFONT_TextOut(g_pStatusFont, g_pBack, Buffer, -1, 50, 76);

    if (g_DrawWithTexture)
    {
        wcscpy(Buffer, L"TEXTURE: ");
        wcscat(Buffer, g_TextureFormats[g_TextureFormat].Name);

        XFONT_TextOut(g_pStatusFont, g_pBack, Buffer, -1, 280, 76);
    }

    // Show it.
    D3DDevice_Swap(0);
}

//------------------------------------------------------------------------------
// Handle controller input.
//
void ProcessInput()
{
    XINPUT_STATE InputState;
    DWORD A, B, X, Y;
    D3DPRESENT_PARAMETERS d3dpp;

    if (!(XGetDevices(XDEVICE_TYPE_GAMEPAD) & XDEVICE_PORT0_MASK))        
    {
        if (g_InputHandle)
        {
            XInputClose(g_InputHandle);
            g_InputHandle = NULL;
        }

        return;
    }

    if (!g_InputHandle)
    {
        g_InputHandle = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, 0, NULL);
    }

    // Query the input state
    if (XInputGetState(g_InputHandle, &InputState) != ERROR_SUCCESS)
    {
        return;
    }

    A = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] & 0x80;
    B = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] & 0x80;
    X = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] & 0x80;
    Y = InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] & 0x80;

    g_x = 400 * InputState.Gamepad.sThumbLX / 32768 + 400;
    g_y = 300 * -InputState.Gamepad.sThumbLY / 32768 + 300;

    if (A)
    {
        if (!g_A)
        {
            g_WhichFont = (g_WhichFont + 1) % WHICH_Max;
        }
    }

    g_A = A;

    if (B)
    {
        if (!g_B)
        {
            g_WhichSize = (g_WhichSize + 1) % (sizeof(g_Sizes) / 4);
        }
    }

    g_B = B;

    if (X)
    {
        if (!g_X)
        {
            g_DrawWithTexture = !g_DrawWithTexture;
        }
    }

    g_X = X;

    if (Y)
    {
        if (!g_Y)
        {            
            if (g_DrawWithTexture)
            {
                g_TextureFormat = (g_TextureFormat + 1) % (sizeof(g_TextureFormats) / sizeof(g_TextureFormats[0]));

                CreateTexture();
            }
            else
            {
                g_FrameFormat = (g_FrameFormat + 1) %  (sizeof(g_FrameFormats) / sizeof(g_FrameFormats[0]));

                ZeroMemory(&d3dpp, sizeof(d3dpp));

                d3dpp.BackBufferWidth            = 640;
                d3dpp.BackBufferHeight           = 480;
                d3dpp.BackBufferFormat           = g_FrameFormats[g_FrameFormat];
                d3dpp.BackBufferCount            = 1;
                d3dpp.EnableAutoDepthStencil     = TRUE;
                d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;

                D3DDevice_Reset(&d3dpp);
            }
        }
    }

    g_Y = Y;

    LoadFont();
}

//==============================================================================
// Main goo.
//==============================================================================

//------------------------------------------------------------------------------
// Main entrypoint.
//
void __cdecl main()
{
    Init();

    for(;;)
    {
        ProcessInput();

        Paint();

        D3DDevice_BlockUntilIdle();
    }
}

