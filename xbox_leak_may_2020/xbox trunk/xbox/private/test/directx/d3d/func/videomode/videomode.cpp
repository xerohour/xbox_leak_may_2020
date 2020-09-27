/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

    videomode.cpp

Abstract:

    Provides a basic video mode tests to test for visible area and refresh rates

Author:

    Robert Heitkamp (robheit) 12-Apr-2001

Environment:

    Xbox only

Revision History:

    12-Apr-2001 robheit
        Initial Version

--*/

//------------------------------------------------------------------------------
//  Includes:
//------------------------------------------------------------------------------
#include <stdio.h>
#include <xtl.h>
#include "bitfont.h"

extern "C" int DebugPrint(char* format, ...);

//------------------------------------------------------------------------------
//  Constants
//------------------------------------------------------------------------------
static const float g_pi = 3.14159265359f;

//------------------------------------------------------------------------------
//  Vertex formats
//------------------------------------------------------------------------------
struct VERTEX
{
    float       x, y, z, rhw;
    D3DCOLOR    color;
};
#define FVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

//------------------------------------------------------------------------------
//  Joystick
//------------------------------------------------------------------------------
struct DMTHJoystick
{
    float   leftStickX;
    float   leftStickY;
    float   rightStickX;
    float   rightStickY;
    float   a;
    float   b;
    float   x;
    float   y;
    float   black;
    float   white;
    float   leftTrigger;
    float   rightTrigger;
    BOOL    back;
    BOOL    start;
    BOOL    dPadLeft;
    BOOL    dPadRight;
    BOOL    dPadUp;
    BOOL    dPadDown;
    BOOL    leftStick;
    BOOL    rightStick;
};
static const DWORD DMTH_JOYBUTTON_X         = 0x00000001;
static const DWORD DMTH_JOYBUTTON_Y         = 0x00000002;
static const DWORD DMTH_JOYBUTTON_WHITE     = 0x00000004;
static const DWORD DMTH_JOYBUTTON_A         = 0x00000008;
static const DWORD DMTH_JOYBUTTON_B         = 0x00000010;
static const DWORD DMTH_JOYBUTTON_BLACK     = 0x00000020;
static const DWORD DMTH_JOYBUTTON_LTRIG     = 0x00000040;
static const DWORD DMTH_JOYBUTTON_RTRIG     = 0x00000080;
static const DWORD DMTH_JOYBUTTON_LSTICK    = 0x00000100;
static const DWORD DMTH_JOYBUTTON_RSTICK    = 0x00000200;
static const DWORD DMTH_JOYBUTTON_UP        = 0x00000400;
static const DWORD DMTH_JOYBUTTON_DOWN      = 0x00000800;
static const DWORD DMTH_JOYBUTTON_LEFT      = 0x00001000;
static const DWORD DMTH_JOYBUTTON_RIGHT     = 0x00002000;
static const DWORD DMTH_JOYBUTTON_BACK      = 0x00004000;
static const DWORD DMTH_JOYBUTTON_START     = 0x00008000;
static const DWORD DMTH_JOYBUTTON_ALL       = 0xffffffff;

//------------------------------------------------------------------------------
//  Global Variables:
//------------------------------------------------------------------------------
static double               g_lastTime;
static UINT                 g_numModes;
static UINT                 g_currentMode;
static BitFont              g_font;
static UINT                 g_visibleWidth;
static UINT                 g_visibleHeight;
static UINT                 g_visibleHDTVWidth;
static UINT                 g_visibleHDTVHeight;
static double               g_frequency;
static DMTHJoystick         g_joysticks[4];
static VERTEX               g_bw1Verts[512];
static VERTEX               g_bw2Verts[512];
static VERTEX               g_redVerts[512];
static VERTEX               g_blueVerts[512];
static VERTEX               g_greenVerts[512];
static VERTEX               g_box[4];
static VERTEX               g_circle1[722];
static VERTEX               g_circle2[722];
static BOOL                 g_inputInitialized  = FALSE;
static HANDLE               g_inputHandles[4]   = {NULL, NULL, NULL, NULL};
static double               g_frameRate         = 0.0f;
static UINT                 g_frame             = 0;
static double               g_elapsedTime       = 0.0f;
static IDirect3DSurface8*   g_backBuffer        = NULL;
static IDirect3D8*          g_d3d               = NULL;
static IDirect3DDevice8*    g_device            = NULL;
static D3DDISPLAYMODE*      g_modeList          = NULL;
static D3DFORMAT*           g_depthFormat       = NULL;
static VERTEX               g_verts[5]          = 
{
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xffff0000 },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xffff0000 },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xffff0000 },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xffff0000 },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xffff0000 }
};
static VERTEX               g_hdtvVerts[5]		= 
{
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xff0000ff },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xff0000ff },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xff0000ff },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xff0000ff },
    { 0.0f, 0.0f, 0.0f, 1.0f, 0xff0000ff }
};

//------------------------------------------------------------------------------
//  Function Declarations
//------------------------------------------------------------------------------
static void DrawTestPattern(void);
static BOOL Initialize(void);
static BOOL CreateDevice(void);
static void Cleanup(void);
static void CreateTestPattern(void);
static const WCHAR* FormatToWChar(DWORD);
static const PCHAR FormatToString(DWORD);
static BOOL IsAnyButtonDown(void);
static void GetJoystickStates(void);
static BOOL GetJoystickState(UINT);
static BOOL IsPortValid(UINT);
static void OpenInput(void);
static void ReleaseInput(UINT);
static void OpenInput(UINT);
static void ReleaseInput(void);
static DWORD UINTToPort(UINT);
static DWORD HSV2D3DCOLOR(float, float, float);
static BOOL IsBButtonDown(void);
static BOOL IsAButtonDown(void);
static BOOL IsQuitButtonDown(void);

//------------------------------------------------------------------------------
//	Reboot Code
//------------------------------------------------------------------------------
typedef enum _FIRMWARE_REENTRY 
{
    HalHaltRoutine,
    HalRebootRoutine,
    HalQuickRebootRoutine,
    HalKdRebootRoutine,
    HalMaximumRoutine
} FIRMWARE_REENTRY, *PFIRMWARE_REENTRY;

extern "C" VOID HalReturnToFirmware(IN FIRMWARE_REENTRY Routine);

//------------------------------------------------------------------------------
//  main
//------------------------------------------------------------------------------
void __cdecl 
main(void)
/*++

Routine Description:

    Main routine. Initializes, loops and draws

Arguments:

    None

Return Value:

    None

--*/
{
    static BOOL switchingMode = FALSE;

    // Initialize
    if(Initialize())
    {
        Cleanup();
        return; 
    }

    // Create the first device
    if(CreateDevice())
    {
        Cleanup();
        return;
    }

    // Create the data
    CreateTestPattern();
    
    // Create data for the 
    while(1)
    {
        // Poll for input
        if(IsQuitButtonDown())
			HalReturnToFirmware(HalQuickRebootRoutine);

        else if(IsAButtonDown())
        {
            if(!switchingMode)
            {
                ++g_currentMode;
                if(g_currentMode == g_numModes)
                    g_currentMode = 0;

                // Create a new device
                if(CreateDevice())
                {
                    Cleanup();
                    return;
                }

                // Create a new test pattern
                CreateTestPattern();

                switchingMode = TRUE;
            }
        }

		else if(IsBButtonDown())
		{
            if(!switchingMode)
            {
				if(g_currentMode == 0)
					g_currentMode = g_numModes-1;
				else
					--g_currentMode;

                // Create a new device
                if(CreateDevice())
                {
                    Cleanup();
                    return;
                }

                // Create a new test pattern
                CreateTestPattern();

                switchingMode = TRUE;
            }
        }
        else
            switchingMode = FALSE;

        // Draw
        DrawTestPattern();
    }
}

//------------------------------------------------------------------------------
//  Initialize
//------------------------------------------------------------------------------
static BOOL
Initialize(void)
/*++

Routine Description:

    Initializes the device

Arguments:

    None

Return Value:

    None

--*/
{
    UINT        i;
    UINT        ii;
    UINT        index;
    D3DFORMAT   formats[]   = 
    { 
        D3DFMT_D24S8, 
        D3DFMT_D16, 
        D3DFMT_F24S8, 
        D3DFMT_F16, 
        D3DFMT_D16_LOCKABLE 
    };
    UINT        numFormats = sizeof(formats) / sizeof(formats[0]);
    
    // Create an instance of a Direct3D8 object 
    g_d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if(g_d3d == NULL)
        return TRUE;

    // Query the number of display modes
    g_numModes  = g_d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT);
    if(g_numModes  == 0)
        return TRUE;

    // Enumerate the adapter modes
    g_modeList      = new D3DDISPLAYMODE [g_numModes];
    g_depthFormat   = new D3DFORMAT [g_numModes];
    for(index=0, i=0; i<g_numModes; ++i)
    {
        if(g_d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &g_modeList[index]) == D3D_OK)
        {
            // Find the first valid depth format for the mode
            for(ii=0; ii<numFormats; ++ii)
            {
                // Is it a good match?
                if(g_d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                                                 g_modeList[index].Format,
                                                 g_modeList[index].Format,
                                                 formats[ii]) == D3D_OK)
                {
                    g_depthFormat[index] = formats[ii];
                    ++index;
                    break;
                }
            }
        }
    }

    // Adjust the number of modes in the event IDirect3D8::EnumAdapterModes 
    // failed for a particular mode
    g_numModes = index;
    
    // Set the starting mode
    g_currentMode = 0;

    // Initialize input
    XDEVICE_PREALLOC_TYPE   devicePreallocType = {XDEVICE_TYPE_GAMEPAD, 4};
    XInitDevices(1, &devicePreallocType);
    OpenInput();

    // Query the performance frequency
    LONGLONG frequency;
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    g_frequency = (double)frequency;

    // Initialize the color bars
    for(ii=0, i=0; i<256; ++i)
    {
        g_bw1Verts[ii].z        = 0.0f;
        g_bw1Verts[ii].rhw      = 1.0f;
        g_bw1Verts[ii].color    = (i << 16) | (i << 8) | i;
        g_bw2Verts[ii].z        = 0.0f;
        g_bw2Verts[ii].rhw      = 1.0f;
        g_bw2Verts[ii].color    = ((255 - i) << 16) | ((255 - i) << 8) | (255 - i);
        g_redVerts[ii].z        = 0.0f;
        g_redVerts[ii].rhw      = 1.0f;
        g_redVerts[ii].color    = (i << 16);
        g_greenVerts[ii].z      = 0.0f;
        g_greenVerts[ii].rhw    = 1.0f;
        g_greenVerts[ii].color  = (i << 8);
        g_blueVerts[ii].z       = 0.0f;
        g_blueVerts[ii].rhw     = 1.0f;
        g_blueVerts[ii++].color = i;

        g_bw1Verts[ii].z        = 0.0f;
        g_bw1Verts[ii].rhw      = 1.0f;
        g_bw1Verts[ii].color    = (i << 16) | (i << 8) | i;
        g_bw2Verts[ii].z        = 0.0f;
        g_bw2Verts[ii].rhw      = 1.0f;
        g_bw2Verts[ii].color    = ((255 - i) << 16) | ((255 - i) << 8) | (255 - i);
        g_redVerts[ii].z        = 0.0f;
        g_redVerts[ii].rhw      = 1.0f;
        g_redVerts[ii].color    = (i << 16);
        g_greenVerts[ii].z      = 0.0f;
        g_greenVerts[ii].rhw    = 1.0f;
        g_greenVerts[ii].color  = (i << 8);
        g_blueVerts[ii].z       = 0.0f;
        g_blueVerts[ii].rhw     = 1.0f;
        g_blueVerts[ii++].color = i;
    }

    // Center 1:1 box
    g_box[0].z      = 0.5f;
    g_box[0].rhw    = 1.0f;
    g_box[0].color  = 0xff000000;
    g_box[1].z      = 0.5f;
    g_box[1].rhw    = 1.0f;
    g_box[1].color  = 0xff000000;
    g_box[2].z      = 0.5f;
    g_box[2].rhw    = 1.0f;
    g_box[2].color  = 0xff000000;
    g_box[3].z      = 0.5f;
    g_box[3].rhw    = 1.0f;
    g_box[3].color  = 0xff000000;

    // Center 1:1 circles
    for(i=0; i<722; ++i)
    {
        g_circle1[i].z      = 0.25f;
        g_circle1[i].rhw    = 1.0f;
        g_circle1[i].color  = 0xffffffff;
        g_circle2[i].z      = 0.0f;
        g_circle2[i].rhw    = 1.0f;
        g_circle2[i].color  = 0xffffffff;
    }

    // Query the timer
    LONGLONG    timer;
    QueryPerformanceCounter((LARGE_INTEGER*)&timer);
    g_lastTime = (double)timer / g_frequency;

    return FALSE;
}

//------------------------------------------------------------------------------
//  CreateDevice
//------------------------------------------------------------------------------
static BOOL
CreateDevice(void)
/*++

Routine Description:

    Creates a device for the current mode

Arguments:

    None

Return Value:

    TRUE on error, FALSE otherwise

--*/
{
    D3DPRESENT_PARAMETERS   d3dpp;
	CHAR					buffer[256];
    
    // Release the previous device
    if(g_device)
        g_device->Release();
    
    // Setup the present parameters
    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = g_modeList[g_currentMode].Width;
    d3dpp.BackBufferHeight                  = g_modeList[g_currentMode].Height;
    d3dpp.BackBufferFormat                  = g_modeList[g_currentMode].Format;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.Flags                             = g_modeList[g_currentMode].Flags;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = NULL;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;
    d3dpp.AutoDepthStencilFormat            = g_depthFormat[g_currentMode];
    d3dpp.FullScreen_RefreshRateInHz        = g_modeList[g_currentMode].RefreshRate;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_ONE;

    // Create the device
    if(g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, 
                           D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, 
                           &g_device) != D3D_OK)
    {
        return TRUE;
    }

    // Get the back buffer
    g_device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_backBuffer);

    // Setup a few device parameters
    float lw = 1.0f;
    g_device->SetRenderState(D3DRS_LINEWIDTH, *(DWORD*)&lw);

    // Initialize a few things
    g_frameRate     = 0.0f;
    g_frame         = 0;
    g_elapsedTime   = 0.0f;

	// Debug print relevent information
	sprintf(buffer, "videomode: %dx%d %s %dHz", d3dpp.BackBufferWidth, 
			d3dpp.BackBufferHeight,
			FormatToString(d3dpp.BackBufferFormat),
			d3dpp.FullScreen_RefreshRateInHz);

	if(g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_WIDESCREEN)
		strcat(buffer, " Widescreen");
	if(g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_INTERLACED)
		strcat(buffer, " Interlaced");
	if(g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_PROGRESSIVE)
		strcat(buffer, " Progressive");
	if(g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_LOCKABLE_BACKBUFFER)
		strcat(buffer, " LockableBackbuffer");
	if(g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_FIELD)
		strcat(buffer, " Field");
	if(g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO)
		strcat(buffer, " 10x11");
	strcat(buffer, "\n");

	DebugPrint(buffer);

    return FALSE;
}

//------------------------------------------------------------------------------
//  DrawTestPattern
//------------------------------------------------------------------------------
static void
DrawTestPattern(void)
/*++

Routine Description:

    Draws the test pattern

Arguments:

    None

Return Value:

    None

--*/
{
    WCHAR           buffer[128];
    DWORD           width;
    DWORD           height;
    LONGLONG        timer;
    double          currentTime;
    int             centerX = g_modeList[g_currentMode].Width / 2;
    int             centerY = g_modeList[g_currentMode].Height / 2;
    int             y;
	float			inc = (g_modeList[g_currentMode].Height <= 240) ? 1.0f : 1.25f;

    // Clear the viewport
    g_device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

    // Begin the scene
    g_device->BeginScene();

    // Set the vertex shader for the lines
    g_device->SetVertexShader(FVF_VERTEX);

    // Draw the visible lines
    g_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, g_verts, sizeof(VERTEX));
    g_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, g_hdtvVerts, sizeof(VERTEX));

    // Draw the color bars
    g_device->DrawPrimitiveUP(D3DPT_LINELIST, 256, g_bw1Verts, sizeof(VERTEX));
    g_device->DrawPrimitiveUP(D3DPT_LINELIST, 256, g_bw2Verts, sizeof(VERTEX));
    g_device->DrawPrimitiveUP(D3DPT_LINELIST, 256, g_redVerts, sizeof(VERTEX));
    g_device->DrawPrimitiveUP(D3DPT_LINELIST, 256, g_greenVerts, sizeof(VERTEX));
    g_device->DrawPrimitiveUP(D3DPT_LINELIST, 256, g_blueVerts, sizeof(VERTEX));

    // Draw the 1:1 box
    g_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_box, sizeof(VERTEX));

    // Draw the 1:1 circles
    g_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 720, g_circle1, sizeof(VERTEX));
    g_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 720, g_circle2, sizeof(VERTEX));

    // Start for labeling
    g_font.GetTextStringLength(&width, &height, L"XBOX");
    y = centerY - (int)((float)height * 9.0f);

    // Mode
    swprintf(buffer, L"Mode:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%d / %d", g_currentMode+1, g_numModes);
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Width
    swprintf(buffer, L"Width:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%d - %d - %d", g_modeList[g_currentMode].Width, g_visibleHDTVWidth, g_visibleWidth);
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Height
    swprintf(buffer, L"Height:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%d - %d - %d", g_modeList[g_currentMode].Height, g_visibleHDTVHeight, g_visibleHeight);
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Format
    swprintf(buffer, L"Format:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%s", FormatToWChar(g_modeList[g_currentMode].Format));
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Widescreen
    swprintf(buffer, L"Widescreen:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%s", (g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_WIDESCREEN) ? L"Yes" : L"No");
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Interlaced
    swprintf(buffer, L"Interlaced:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%s", (g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_INTERLACED) ? L"Yes" : L"No");
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Progressive
    swprintf(buffer, L"Progressive:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%s", (g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_PROGRESSIVE) ? L"Yes" : L"No");
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Lockable Backbuffer
    swprintf(buffer, L"Lockable Backbuffer:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%s", (g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_LOCKABLE_BACKBUFFER) ? L"Yes" : L"No");
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Field
    swprintf(buffer, L"Field:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%s", (g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_FIELD) ? L"Yes" : L"No");
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // 10x11 Aspect
    swprintf(buffer, L"10x11 Aspect:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    swprintf(buffer, L"%s", (g_modeList[g_currentMode].Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO) ? L"Yes" : L"No");
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * inc);

    // Refresh Rate
    swprintf(buffer, L"Refresh Rate:");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - width - 10, y, 0, 0xff000000, 0xffffffff);
    if(g_frameRate != 0.0f)
        swprintf(buffer, L"%d Hz (%.2lf fps)", g_modeList[g_currentMode].RefreshRate, g_frameRate);
    else
        swprintf(buffer, L"%d Hz", g_modeList[g_currentMode].RefreshRate);
    g_font.DrawText(g_backBuffer, buffer, centerX - 5, y, 0, 0xff0000ff, 0xffffffff);
    y += (int)((float)height * 2.0f);

    // Info
    DWORD w1, w2, w3, w4, w5;
    g_font.GetTextStringLength(&w1, &height, L"The ");
    g_font.GetTextStringLength(&w2, &height, L"red (normal) ");
    g_font.GetTextStringLength(&w3, &height, L"and ");
    g_font.GetTextStringLength(&w4, &height, L"blue (HDTV) ");
    g_font.GetTextStringLength(&w5, &height, L"boxes represent");
    width = w1 + w2 + w3 + w4 + w5;
    g_font.DrawText(g_backBuffer, L"The ", centerX - (int)(width / 2), y, 0, 0xff000000, 0xffffffff);
    g_font.DrawText(g_backBuffer, L"red (normal)", centerX - (int)(width / 2) + w1, y, 0, 0xffff0000, 0xffffffff);
    g_font.DrawText(g_backBuffer, L"and ", centerX - (int)(width / 2) + w1 + w2, y, 0, 0xff000000, 0xffffffff);
    g_font.DrawText(g_backBuffer, L"blue (HDTV)", centerX - (int)(width / 2) + w1 + w2 + w3, y, 0, 0xff0000ff, 0xffffffff);
    g_font.DrawText(g_backBuffer, L"boxes represent", 
                    centerX - (int)(width / 2) + w1 + w2 + w3 + w4, y, 0, 0xff000000, 0xffffffff);
    y += (int)((float)height * 1.0f);
    swprintf(buffer, L"the minimum screen resolutions and should be"); // completely visible for their respective modes");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - (int)(width / 2), y, 0, 0xff000000, 0xffffffff);
    y += (int)((float)height * 1.0f);

    swprintf(buffer, L"completely visible for their respective modes");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - (int)(width / 2), y, 0, 0xff000000, 0xffffffff);
    y += (int)((float)height * 1.5f);

    // Continue
    swprintf(buffer, L"Press A for next mode, or");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - (int)(width / 2), y, 0, 0xffff0000, 0xffffffff);
    y += (int)((float)height * 1.0f);

    swprintf(buffer, L"press B for previous mode");
    g_font.GetTextStringLength(&width, &height, buffer);
    g_font.DrawText(g_backBuffer, buffer, centerX - (int)(width / 2), y, 0, 0xffff0000, 0xffffffff);
    y += (int)((float)height * 1.5f);

    // End the scene
    g_device->EndScene();

    // Update the screen
    g_device->Present(NULL, NULL, NULL, NULL);
    
    // Stop timer
    QueryPerformanceCounter((LARGE_INTEGER*)&timer);
    currentTime     = (double)timer / g_frequency;
    g_elapsedTime   += currentTime - g_lastTime;
    g_lastTime      = currentTime;

    // Frame counter
    ++g_frame;

    // Frame rate calculations (twice per second)
    if(g_elapsedTime > 1.0)
    {
        g_frameRate     = (double)g_frame / g_elapsedTime;
        g_frame         = 0;
        g_elapsedTime   = 0.0;
    }
}

//------------------------------------------------------------------------------
//  Cleaup
//------------------------------------------------------------------------------
static void
Cleanup(void)
/*++

Routine Description:

    Frees all global resources

Arguments:

    None

Return Value:

    None

--*/
{
    // Mode list
    if(g_modeList)
    {
        delete [] g_modeList;
        g_modeList = NULL;
    }

    // Depth format list
    if(g_depthFormat)
    {
        delete [] g_depthFormat;
        g_depthFormat = NULL;
    }

    // IDirect3DDevice8
    if(g_device)
    {
        g_device->Release();
        g_device = NULL;
    }

    // IDirect3D8
    if(g_d3d)
    {
        g_d3d->Release();
        g_d3d = NULL;
    }

    // Input devices
    ReleaseInput();
}

//------------------------------------------------------------------------------
//  CreateTestPattern
//------------------------------------------------------------------------------
static void
CreateTestPattern(void)
/*++

Routine Description:

    Creats the test pattern data

Arguments:

    None

Return Value:

    None

--*/
{
    UINT    i;
    UINT    ii;
    float   y;
    float   x;
    float   visibleWidth    = (float)g_modeList[g_currentMode].Width * 0.85f;
    float   visibleHeight   = (float)g_modeList[g_currentMode].Height * 0.85f;
    float   dx              = ((float)g_modeList[g_currentMode].Width - visibleWidth) / 2.0f;
    float   dy              = ((float)g_modeList[g_currentMode].Height - visibleHeight) / 2.0f;
    float   x0              = dx;
    float   x1              = dx + visibleWidth - 1.0f;
    float   y0              = dy;
    float   y1              = dy + visibleHeight - 1.0f;
	float	yInc;
	
    // Visible vertices
    g_verts[0].x    = x0;
    g_verts[0].y    = y0;
    g_verts[1].x    = x1;
    g_verts[1].y    = y0;
    g_verts[2].x    = x1;
    g_verts[2].y    = y1;
    g_verts[3].x    = x0;
    g_verts[3].y    = y1;
    g_verts[4].x    = x0;
    g_verts[4].y    = y0;

    // Visible region
    g_visibleWidth  = (UINT)visibleWidth;
    g_visibleHeight = (UINT)visibleHeight;

    visibleWidth    = (float)g_modeList[g_currentMode].Width * 0.95f;
    visibleHeight   = (float)g_modeList[g_currentMode].Height * 0.95f;
    dx              = ((float)g_modeList[g_currentMode].Width - visibleWidth) / 2.0f;
    dy              = ((float)g_modeList[g_currentMode].Height - visibleHeight) / 2.0f;
    x0              = dx;
    x1              = dx + visibleWidth - 1.0f;
    y0              = dy;
    y1              = dy + visibleHeight - 1.0f;

    // Visible HDTV vertices
    g_hdtvVerts[0].x    = x0;
    g_hdtvVerts[0].y    = y0;
    g_hdtvVerts[1].x    = x1;
    g_hdtvVerts[1].y    = y0;
    g_hdtvVerts[2].x    = x1;
    g_hdtvVerts[2].y    = y1;
    g_hdtvVerts[3].x    = x0;
    g_hdtvVerts[3].y    = y1;
    g_hdtvVerts[4].x    = x0;
    g_hdtvVerts[4].y    = y0;

    // Visible region
    g_visibleHDTVWidth  = (UINT)visibleWidth;
    g_visibleHDTVHeight = (UINT)visibleHeight;

	if(g_modeList[g_currentMode].Height > 256)
	{
		y = (float)((g_modeList[g_currentMode].Height - 256) / 2);
		yInc = 1.0;
	}
	else
	{
		y = (float)(g_modeList[g_currentMode].Height / 2) - ((float)g_modeList[g_currentMode].Height * 0.25f);
		yInc = (float)g_modeList[g_currentMode].Height * 0.5f / 256.0f;
	}
    x = (float)((g_modeList[g_currentMode].Width - g_visibleWidth) / 2);

    // Color bars (centered)
    for(ii=0, i=0; i<256; ++i, y+=yInc)
    {
        g_bw1Verts[ii].x    = g_modeList[g_currentMode].Width - x - 20.0f;
        g_bw1Verts[ii].y    = y;
        g_bw2Verts[ii].x    = g_modeList[g_currentMode].Width - x - 60.0f;
        g_bw2Verts[ii].y    = y;
        g_redVerts[ii].x    = x + 20.0f;
        g_redVerts[ii].y    = y;
        g_greenVerts[ii].x  = x + 50.0f;
        g_greenVerts[ii].y  = y;
        g_blueVerts[ii].x   = x + 80.0f;
        g_blueVerts[ii++].y = y;

        g_bw1Verts[ii].x    = g_modeList[g_currentMode].Width - x - 50.0f;
        g_bw1Verts[ii].y    = y;
        g_bw2Verts[ii].x    = g_modeList[g_currentMode].Width - x - 90.0f;
        g_bw2Verts[ii].y    = y;
        g_redVerts[ii].x    = x + 40.0f;
        g_redVerts[ii].y    = y;
        g_greenVerts[ii].x  = x + 70.0f;
        g_greenVerts[ii].y  = y;
        g_blueVerts[ii].x   = x + 100.0f;
        g_blueVerts[ii++].y = y;
    }

    // Center 1:1 box
    x   = (float)g_modeList[g_currentMode].Height * 0.6f;
    y   = x / 2.0f;
    dx  = (float)(g_modeList[g_currentMode].Width / 2);
    dy  = (float)(g_modeList[g_currentMode].Height / 2);
    g_box[0].x  = dx - y;
    g_box[0].y  = dy - y;
    g_box[1].x  = dx + y;
    g_box[1].y  = dy - y;
    g_box[2].x  = dx - y;
    g_box[2].y  = dy + y;
    g_box[3].x  = dx + y;
    g_box[3].y  = dy + y;

    // Center 1:1 circles
    x               = (float)g_modeList[g_currentMode].Height * 0.55f / 2.0f;
    g_circle1[0].x  = dx;
    g_circle1[0].y  = dy;
    g_circle2[0].x  = dx;
    g_circle2[0].y  = dy;
    for(x0=0.0f, x1=0.0f, i=1; i<722; ++i, x1+=0.5f, x0+=(0.5f * g_pi / 180.0f))
    {
        g_circle1[i].x  = (float)(cos(x0) * y + g_circle1[0].x);
        g_circle1[i].y  = (float)(sin(x0) * y + g_circle1[0].y);
        g_circle1[i].color = HSV2D3DCOLOR(x1, 1.0, 1.0);
        g_circle2[i].x  = (float)(cos(x0) * x + g_circle2[0].x);
        g_circle2[i].y  = (float)(sin(x0) * x + g_circle2[0].y);
    }
}

//------------------------------------------------------------------------------
//  FormatToWChar
//------------------------------------------------------------------------------
static const WCHAR* 
FormatToWChar(
              IN DWORD  format
              )
/*++

Routine Description:

    Converts a format to a WCHAR* for text output

Arguments:

    IN format - Format

Return Value:

    constant WCHAR*

--*/
{
    switch(format)
    {
    case D3DFMT_A8R8G8B8:
        return L"D3DFMT_A8R8G8B8";
    case D3DFMT_X8R8G8B8: // D3DFMT_X8L8V8U8
        return L"D3DFMT_X8R8G8B8 / D3DFMT_X8L8V8U8";
    case D3DFMT_R5G6B5:
        return L"D3DFMT_R5G6B5";
    case D3DFMT_X1R5G5B5:
        return L"D3DFMT_X1R5G5B5";
    case D3DFMT_A1R5G5B5:
        return L"D3DFMT_A1R5G5B5";
    case D3DFMT_A4R4G4B4:
        return L"D3DFMT_A4R4G4B4";
    case D3DFMT_A8:
        return L"D3DFMT_A8";
    case D3DFMT_A8B8G8R8:
        return L"D3DFMT_A8B8G8R8";
    case D3DFMT_B8G8R8A8: // D3DFMT_Q8W8V8U8
        return L"D3DFMT_B8G8R8A8 / D3DFMT_Q8W8V8U8";
    case D3DFMT_R4G4B4A4:
        return L"D3DFMT_R4G4B4A4";
    case D3DFMT_R5G5B5A1:
        return L"D3DFMT_R5G5B5A1";
    case D3DFMT_R8G8B8A8:
        return L"D3DFMT_R8G8B8A8";
    case D3DFMT_R8B8:
        return L"D3DFMT_R8B8";
    case D3DFMT_G8B8: // D3DFMT_V8U8:
        return L"D3DFMT_G8B8 / D3DFMT_V8U8";
    case D3DFMT_P8:
        return L"D3DFMT_P8";
    case D3DFMT_L8:
        return L"D3DFMT_L8";
    case D3DFMT_A8L8:
        return L"D3DFMT_A8L8";
    case D3DFMT_AL8:
        return L"D3DFMT_AL8";
    case D3DFMT_L16:
        return L"D3DFMT_L16";
    case D3DFMT_L6V5U5:
        return L"D3DFMT_L6V5U5";
    case D3DFMT_V16U16:
        return L"D3DFMT_V16U16";
    case D3DFMT_D16_LOCKABLE: // D3DFMT_D16
        return L"D3DFMT_D16_LOCKABLE / D3DFMT_D16";
    case D3DFMT_D24S8:
        return L"D3DFMT_D24S8";
    case D3DFMT_F16:
        return L"D3DFMT_F16";
    case D3DFMT_F24S8:
        return L"D3DFMT_F24S8";
    case D3DFMT_UYVY:
        return L"D3DFMT_UYVY";
    case D3DFMT_YUY2:
        return L"D3DFMT_YUY2";
    case D3DFMT_DXT1:
        return L"D3DFMT_DXT1";
    case D3DFMT_DXT2: // D3DFMT_DXT3
        return L"D3DFMT_DXT2 / D3DFMT_DXT3";
    case D3DFMT_DXT4: // D3DFMT_DXT5
        return L"D3DFMT_DXT4 / D3DFMT_DXT5";
    case D3DFMT_LIN_A1R5G5B5:
        return L"D3DFMT_LIN_A1R5G5B5";
    case D3DFMT_LIN_A4R4G4B4:
        return L"D3DFMT_LIN_A4R4G4B4";
    case D3DFMT_LIN_A8:
        return L"D3DFMT_LIN_A8";
    case D3DFMT_LIN_A8B8G8R8:
        return L"D3DFMT_LIN_A8B8G8R8";
    case D3DFMT_LIN_A8R8G8B8:
        return L"D3DFMT_LIN_A8R8G8B8";
    case D3DFMT_LIN_B8G8R8A8:
        return L"D3DFMT_LIN_B8G8R8A8";
    case D3DFMT_LIN_G8B8:
        return L"D3DFMT_LIN_G8B8";
    case D3DFMT_LIN_R4G4B4A4:
        return L"D3DFMT_LIN_R4G4B4A4";
    case D3DFMT_LIN_R5G5B5A1:
        return L"D3DFMT_LIN_R5G5B5A1";
    case D3DFMT_LIN_R5G6B5:
        return L"D3DFMT_LIN_R5G6B5";
    case D3DFMT_LIN_R6G5B5:
        return L"D3DFMT_LIN_R6G5B5";
    case D3DFMT_LIN_R8B8:
        return L"D3DFMT_LIN_R8B8";
    case D3DFMT_LIN_R8G8B8A8:
        return L"D3DFMT_LIN_R8G8B8A8";
    case D3DFMT_LIN_X1R5G5B5:
        return L"D3DFMT_LIN_X1R5G5B5";
    case D3DFMT_LIN_X8R8G8B8:
        return L"D3DFMT_LIN_X8R8G8B8";
    case D3DFMT_LIN_A8L8:
        return L"D3DFMT_LIN_A8L8";
    case D3DFMT_LIN_AL8:
        return L"D3DFMT_LIN_AL8";
    case D3DFMT_LIN_L16:
        return L"D3DFMT_LIN_L16";
    case D3DFMT_LIN_L8:
        return L"D3DFMT_LIN_L8";
    case D3DFMT_LIN_D24S8:
        return L"D3DFMT_LIN_D24S8";
    case D3DFMT_LIN_F24S8:
        return L"D3DFMT_LIN_F24S8";
    case D3DFMT_LIN_D16:
        return L"D3DFMT_LIN_D16";
    case D3DFMT_LIN_F16:
        return L"D3DFMT_LIN_F16";
    case D3DFMT_VERTEXDATA:
        return L"D3DFMT_VERTEXDATA";
    case D3DFMT_INDEX16:
        return L"D3DFMT_INDEX16";
    }

    return L"Unknown";
}

//------------------------------------------------------------------------------
//  FormatToString
//------------------------------------------------------------------------------
static const PCHAR
FormatToString(
               IN DWORD  format
               )
/*++

Routine Description:

    Converts a format to a string for text output

Arguments:

    IN format - Format

Return Value:

    PCHAR

--*/
{
    switch(format)
    {
    case D3DFMT_A8R8G8B8:
        return "D3DFMT_A8R8G8B8";
    case D3DFMT_X8R8G8B8: // D3DFMT_X8L8V8U8
        return "D3DFMT_X8R8G8B8 / D3DFMT_X8L8V8U8";
    case D3DFMT_R5G6B5:
        return "D3DFMT_R5G6B5";
    case D3DFMT_X1R5G5B5:
        return "D3DFMT_X1R5G5B5";
    case D3DFMT_A1R5G5B5:
        return "D3DFMT_A1R5G5B5";
    case D3DFMT_A4R4G4B4:
        return "D3DFMT_A4R4G4B4";
    case D3DFMT_A8:
        return "D3DFMT_A8";
    case D3DFMT_A8B8G8R8:
        return "D3DFMT_A8B8G8R8";
    case D3DFMT_B8G8R8A8: // D3DFMT_Q8W8V8U8
        return "D3DFMT_B8G8R8A8 / D3DFMT_Q8W8V8U8";
    case D3DFMT_R4G4B4A4:
        return "D3DFMT_R4G4B4A4";
    case D3DFMT_R5G5B5A1:
        return "D3DFMT_R5G5B5A1";
    case D3DFMT_R8G8B8A8:
        return "D3DFMT_R8G8B8A8";
    case D3DFMT_R8B8:
        return "D3DFMT_R8B8";
    case D3DFMT_G8B8: // D3DFMT_V8U8:
        return "D3DFMT_G8B8 / D3DFMT_V8U8";
    case D3DFMT_P8:
        return "D3DFMT_P8";
    case D3DFMT_L8:
        return "D3DFMT_L8";
    case D3DFMT_A8L8:
        return "D3DFMT_A8L8";
    case D3DFMT_AL8:
        return "D3DFMT_AL8";
    case D3DFMT_L16:
        return "D3DFMT_L16";
    case D3DFMT_L6V5U5:
        return "D3DFMT_L6V5U5";
    case D3DFMT_V16U16:
        return "D3DFMT_V16U16";
    case D3DFMT_D16_LOCKABLE: // D3DFMT_D16
        return "D3DFMT_D16_LOCKABLE / D3DFMT_D16";
    case D3DFMT_D24S8:
        return "D3DFMT_D24S8";
    case D3DFMT_F16:
        return "D3DFMT_F16";
    case D3DFMT_F24S8:
        return "D3DFMT_F24S8";
    case D3DFMT_UYVY:
        return "D3DFMT_UYVY";
    case D3DFMT_YUY2:
        return "D3DFMT_YUY2";
    case D3DFMT_DXT1:
        return "D3DFMT_DXT1";
    case D3DFMT_DXT2: // D3DFMT_DXT3
        return "D3DFMT_DXT2 / D3DFMT_DXT3";
    case D3DFMT_DXT4: // D3DFMT_DXT5
        return "D3DFMT_DXT4 / D3DFMT_DXT5";
    case D3DFMT_LIN_A1R5G5B5:
        return "D3DFMT_LIN_A1R5G5B5";
    case D3DFMT_LIN_A4R4G4B4:
        return "D3DFMT_LIN_A4R4G4B4";
    case D3DFMT_LIN_A8:
        return "D3DFMT_LIN_A8";
    case D3DFMT_LIN_A8B8G8R8:
        return "D3DFMT_LIN_A8B8G8R8";
    case D3DFMT_LIN_A8R8G8B8:
        return "D3DFMT_LIN_A8R8G8B8";
    case D3DFMT_LIN_B8G8R8A8:
        return "D3DFMT_LIN_B8G8R8A8";
    case D3DFMT_LIN_G8B8:
        return "D3DFMT_LIN_G8B8";
    case D3DFMT_LIN_R4G4B4A4:
        return "D3DFMT_LIN_R4G4B4A4";
    case D3DFMT_LIN_R5G5B5A1:
        return "D3DFMT_LIN_R5G5B5A1";
    case D3DFMT_LIN_R5G6B5:
        return "D3DFMT_LIN_R5G6B5";
    case D3DFMT_LIN_R6G5B5:
        return "D3DFMT_LIN_R6G5B5";
    case D3DFMT_LIN_R8B8:
        return "D3DFMT_LIN_R8B8";
    case D3DFMT_LIN_R8G8B8A8:
        return "D3DFMT_LIN_R8G8B8A8";
    case D3DFMT_LIN_X1R5G5B5:
        return "D3DFMT_LIN_X1R5G5B5";
    case D3DFMT_LIN_X8R8G8B8:
        return "D3DFMT_LIN_X8R8G8B8";
    case D3DFMT_LIN_A8L8:
        return "D3DFMT_LIN_A8L8";
    case D3DFMT_LIN_AL8:
        return "D3DFMT_LIN_AL8";
    case D3DFMT_LIN_L16:
        return "D3DFMT_LIN_L16";
    case D3DFMT_LIN_L8:
        return "D3DFMT_LIN_L8";
    case D3DFMT_LIN_D24S8:
        return "D3DFMT_LIN_D24S8";
    case D3DFMT_LIN_F24S8:
        return "D3DFMT_LIN_F24S8";
    case D3DFMT_LIN_D16:
        return "D3DFMT_LIN_D16";
    case D3DFMT_LIN_F16:
        return "D3DFMT_LIN_F16";
    case D3DFMT_VERTEXDATA:
        return "D3DFMT_VERTEXDATA";
    case D3DFMT_INDEX16:
        return "D3DFMT_INDEX16";
    }

    return "Unknown";
}

//------------------------------------------------------------------------------
//  IsAnyButtonDown
//------------------------------------------------------------------------------
static BOOL
IsAnyButtonDown(void)
/*++

Routine Description:

    Returns TRUE if any controller button is down

Arguments:

    None

Return Value:

    TRUE if any controller button is down (at least 50%), FALSE otherwise

--*/
{
    UINT i;

    GetJoystickStates();

    for(i=0; i<4; ++i)
    {
        // Only check valid joysticks
        if(g_inputHandles[i])
        {
            if((g_joysticks[i].x > 0.25f) ||
               (g_joysticks[i].y > 0.25f) ||
               (g_joysticks[i].white > 0.25f) ||
               (g_joysticks[i].a > 0.25f) ||
               (g_joysticks[i].b > 0.25f) ||
               (g_joysticks[i].black > 0.25f) ||
               (g_joysticks[i].leftTrigger > 0.25f) ||
               (g_joysticks[i].rightTrigger > 0.25f) ||
               (g_joysticks[i].leftStick > 0.25f) ||
               (g_joysticks[i].rightStick > 0.25f) ||
               (g_joysticks[i].dPadUp == TRUE) ||
               (g_joysticks[i].dPadDown == TRUE) ||
               (g_joysticks[i].dPadLeft == TRUE) ||
               (g_joysticks[i].dPadRight == TRUE) ||
               (g_joysticks[i].back == TRUE) ||
               (g_joysticks[i].start == TRUE))
            {
               return TRUE;
            }
        }
    }
    return FALSE;
}

//------------------------------------------------------------------------------
//  IsAButtonDown
//------------------------------------------------------------------------------
static BOOL
IsAButtonDown(void)
/*++

Routine Description:

    Returns TRUE if the A button is down on one controller

Arguments:

    None

Return Value:

    TRUE if any controller button is down (at least 50%), FALSE otherwise

--*/
{
    UINT	i;
	BOOL	down	= FALSE;

    GetJoystickStates();

    for(i=0; i<4; ++i)
    {
        // Only check valid joysticks
        if(g_inputHandles[i])
        {
            if((g_joysticks[i].x > 0.25f) ||
               (g_joysticks[i].y > 0.25f) ||
               (g_joysticks[i].white > 0.25f) ||
               (g_joysticks[i].b > 0.25f) ||
               (g_joysticks[i].black > 0.25f) ||
               (g_joysticks[i].leftTrigger > 0.25f) ||
               (g_joysticks[i].rightTrigger > 0.25f) ||
               (g_joysticks[i].leftStick > 0.25f) ||
               (g_joysticks[i].rightStick > 0.25f) ||
               (g_joysticks[i].dPadUp == TRUE) ||
               (g_joysticks[i].dPadDown == TRUE) ||
               (g_joysticks[i].dPadLeft == TRUE) ||
               (g_joysticks[i].dPadRight == TRUE) ||
               (g_joysticks[i].back == TRUE) ||
               (g_joysticks[i].start == TRUE))
            {
               return FALSE;
            }
			else if(g_joysticks[i].a > 0.25f)
			{
				if(down)
					return FALSE;
				down = TRUE;
			}
		}
    }
    return down;
}

//------------------------------------------------------------------------------
//  IsBButtonDown
//------------------------------------------------------------------------------
static BOOL
IsBButtonDown(void)
/*++

Routine Description:

    Returns TRUE if any controller button is down

Arguments:

    None

Return Value:

    TRUE if any controller button is down (at least 50%), FALSE otherwise

--*/
{
    UINT	i;
	BOOL	down	= FALSE;

    GetJoystickStates();

    for(i=0; i<4; ++i)
    {
        // Only check valid joysticks
        if(g_inputHandles[i])
        {
            if((g_joysticks[i].x > 0.25f) ||
               (g_joysticks[i].y > 0.25f) ||
               (g_joysticks[i].white > 0.25f) ||
               (g_joysticks[i].a > 0.25f) ||
               (g_joysticks[i].black > 0.25f) ||
               (g_joysticks[i].leftTrigger > 0.25f) ||
               (g_joysticks[i].rightTrigger > 0.25f) ||
               (g_joysticks[i].leftStick > 0.25f) ||
               (g_joysticks[i].rightStick > 0.25f) ||
               (g_joysticks[i].dPadUp == TRUE) ||
               (g_joysticks[i].dPadDown == TRUE) ||
               (g_joysticks[i].dPadLeft == TRUE) ||
               (g_joysticks[i].dPadRight == TRUE) ||
               (g_joysticks[i].back == TRUE) ||
               (g_joysticks[i].start == TRUE))
            {
               return FALSE;
            }
			else if(g_joysticks[i].b > 0.25f)
			{
				if(down)
					return FALSE;
				down = TRUE;
			}
		}
    }
    return down;
}

//------------------------------------------------------------------------------
//  IsQuitButtonDown
//------------------------------------------------------------------------------
static BOOL
IsQuitButtonDown(void)
/*++

Routine Description:

    Returns TRUE if the back or start buttons are down on one controller

Arguments:

    None

Return Value:

    TRUE if the back or start buttons are down on one controller

--*/
{
    UINT	i;

    GetJoystickStates();

    for(i=0; i<4; ++i)
    {
        if(g_inputHandles[i] && ((g_joysticks[i].back == TRUE) ||
								 (g_joysticks[i].start == TRUE)))
		{
			return TRUE;
		}
    }
    return FALSE;
}

//------------------------------------------------------------------------------
//  GetJoystickStates
//------------------------------------------------------------------------------
static void 
GetJoystickStates(void)
/*++

Routine Description:

    Polls the state of all attached joysticks, storing the results in 
    g_joysticks

Arguments:

    None

Return Value:

    None

--*/
{
    UINT i;

    // Get the state of all ports
    for(i=0; i<4; ++i)
        GetJoystickState(i);
}

//------------------------------------------------------------------------------
//  GetJoystickState
//------------------------------------------------------------------------------
static BOOL
GetJoystickState(
                 IN UINT    port
                 )
/*++

Routine Description:

    Polls the state of a single joystick, storing the result in 
    g_joysticks[port]

Arguments:

    IN port -   Port to poll

Return Value:

    TRUE for a valid poll, FLASE otherwise

--*/
{
    XINPUT_STATE    istate;

    // Simple error checking
    if(!IsPortValid(port))
        return FALSE;

    // Query the input state
    if(XInputGetState(g_inputHandles[port], &istate) != ERROR_SUCCESS) 
        return FALSE;

    // Joystick stick states
    g_joysticks[port].leftStickX    =  (float)istate.Gamepad.sThumbLX / 32768.0f;
    g_joysticks[port].leftStickY    = -(float)istate.Gamepad.sThumbLY / 32768.0f;
    g_joysticks[port].rightStickX   =  (float)istate.Gamepad.sThumbRX / 32768.0f;
    g_joysticks[port].rightStickY   = -(float)istate.Gamepad.sThumbRY / 32768.0f;

    // Analog buttons
    g_joysticks[port].x             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] / 255.0f;
    g_joysticks[port].y             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] / 255.0f;
    g_joysticks[port].white         = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] / 255.0f;
    g_joysticks[port].a             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] / 255.0f;
    g_joysticks[port].b             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] / 255.0f;
    g_joysticks[port].black         = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] / 255.0f;
    g_joysticks[port].leftTrigger   = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] / 255.0f;
    g_joysticks[port].rightTrigger  = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / 255.0f;

    // Digital buttons
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
        g_joysticks[port].leftStick = TRUE;
    else
        g_joysticks[port].leftStick = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
        g_joysticks[port].rightStick = TRUE;
    else
        g_joysticks[port].rightStick = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) 
        g_joysticks[port].dPadUp = TRUE;
    else
        g_joysticks[port].dPadUp = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
        g_joysticks[port].dPadDown = TRUE;
    else
        g_joysticks[port].dPadDown = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) 
        g_joysticks[port].dPadLeft = TRUE;
    else
        g_joysticks[port].dPadLeft = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) 
        g_joysticks[port].dPadRight = TRUE;
    else
        g_joysticks[port].dPadRight = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) 
        g_joysticks[port].back = TRUE;
    else
        g_joysticks[port].back = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_START) 
        g_joysticks[port].start = TRUE;
    else
        g_joysticks[port].start = FALSE;

    return TRUE;
}

//------------------------------------------------------------------------------
//  IsPortValid
//------------------------------------------------------------------------------
static BOOL
IsPortValid(
            IN UINT port
            )
/*++

Routine Description:

    Returns TRUE if the port number is valid

Arguments:

    IN port -   Port to query

Return Value:

    TRUE if the port is valid (has a controller attached) FALSE otherwise

--*/
{
    if(g_inputHandles[port] == NULL)
    {
        OpenInput(port);
        return (g_inputHandles[port] != NULL) ? TRUE : FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
//  OpenInput
//------------------------------------------------------------------------------
static void 
OpenInput(void)
/*++

Routine Description:

    Initializes the DirectInput device for joystick polling

Arguments:

    None

Return Value:

    None

--*/
{
    DWORD   dwInputDevices;
    UINT    i;
    
    // Get the devices
    dwInputDevices = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    // Save time and drop out now if no joysticks are plugged in
    if(!dwInputDevices)
    {
        ReleaseInput();
        return;
    }
    
    // Create a device for each port
    for(i=0; i<4; ++i) 
    {
        if(dwInputDevices & (1 << i)) 
            OpenInput(i);
        else
            ReleaseInput(i);
    }
}

//------------------------------------------------------------------------------
//  ReleaseInput
//------------------------------------------------------------------------------
static void 
ReleaseInput(
             IN UINT port
             )
/*++

Routine Description:

    Releases the DirectInput devices

Arguments:

    IN port -   Port to release

Return Value:

    None

--*/
{
    if(g_inputHandles[port]) 
    {
        XInputClose(g_inputHandles[port]);
        g_inputHandles[port] = NULL;
    }
}

//------------------------------------------------------------------------------
//  OpenInput
//------------------------------------------------------------------------------
static void 
OpenInput(
          IN UINT port
          )
/*++

Routine Description:

    Initializes the DirectInput device for joystick polling

Arguments:

    IN port -   Port to initialize

Return Value:

    None

--*/
{
    // If the port has already been initialized, just return
    if(g_inputHandles[port] != NULL)
        return;
    
    // Create a device
    g_inputHandles[port] = XInputOpen(XDEVICE_TYPE_GAMEPAD, UINTToPort(port),
                                      0, NULL);
}

//------------------------------------------------------------------------------
//  ReleaseInput
//------------------------------------------------------------------------------
static void 
ReleaseInput(void)
/*++

Routine Description:

    Releases the DirectInput devices

Arguments:

    None

Return Value:

    None

--*/
{
    UINT    i;

    for(i=0; i<4; ++i)
        ReleaseInput(i);
}

//------------------------------------------------------------------------------
//  UINTToPort
//------------------------------------------------------------------------------
static DWORD
UINTToPort(
           IN UINT port
           )
/*++

Routine Description:

    Converts an unsigned integer in the range of 0-3 to a XDEVICE port

Arguments:

    IN port -   Value to convert

Return Value:

    A port XDEVICE_PORT0 - XDEVICE_PORT3

--*/
{
    switch(port)
    {
    case 0:
        return XDEVICE_PORT0;
    case 1:
        return XDEVICE_PORT1;
    case 2:
        return XDEVICE_PORT2;
    case 3:
        return XDEVICE_PORT3;
    }

    // This should never happen
    __asm int 3;
    return 0xFFFFFFFF;
}

//------------------------------------------------------------------------------
//  HSV2D3DCOLOR
//------------------------------------------------------------------------------
static DWORD
HSV2D3DCOLOR(
             IN float   h,
             IN float   s,
             IN float   v
             )
/*++

Routine Description:

    Converts a color in HSV space to a D3DCOLOR

Arguments:

    IN h    - Hue (0.0 - 360.0)
    IN s    - Saturation (0.0 - 1.0)
    IN v    - Value (0.0 - 1.0)

Return Value:

    A D3DCOLOR (rgb)

--*/
{
    int     i;
    float   p;
    float   q;
    float   t;
    float   f;
    DWORD   r = 0;
    DWORD   g = 0;
    DWORD   b = 0;

    while(h >= 360.0f)
        h -= 360.0f;
    h = h / 60.0f;
    i = (int)floor(h);
    f = h - i;
    p = v * (1.0f - s);
    q = v * (1.0f - (s * f));
    t = v * (1.0f - (s * (1.0f - f)));
    switch(i)
    {
    case 0:
        r = (DWORD)(v * 255.0f);
        g = (DWORD)(t * 255.0f);
        b = (DWORD)(p * 255.0f);
        break;
    case 1:
        r = (DWORD)(q * 255.0f);
        g = (DWORD)(v * 255.0f);
        b = (DWORD)(p * 255.0f);
        break;
    case 2:
        r = (DWORD)(p * 255.0f);
        g = (DWORD)(v * 255.0f);
        b = (DWORD)(t * 255.0f);
        break;
    case 3:
        r = (DWORD)(p * 255.0f);
        g = (DWORD)(q * 255.0f);
        b = (DWORD)(v * 255.0f);
        break;
    case 4:
        r = (DWORD)(t * 255.0f);
        g = (DWORD)(p * 255.0f);
        b = (DWORD)(v * 255.0f);
        break;
    case 5:
        r = (DWORD)(v * 255.0f);
        g = (DWORD)(p * 255.0f);
        b = (DWORD)(q * 255.0f);
        break;
    }

    // Bounds check
    if(r > 0xff)
        r = 0xff;
    if(g > 0xff)
        g = 0xff;
    if(b > 0xff)
        b = 0xff;

    return 0xff000000 | (r << 16) | (g << 8) | b;
}


