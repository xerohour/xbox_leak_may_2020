/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Illustrates effects of various FSAA modes

Revision History:

    Derived from a DX8 sample.
--*/

#include <SDKCommon.h>
#include <xgraphics.h>
#include <TCHAR.h>

bool                       bQuit   = false;
INT                        g_iType = 0;
INT                        g_iFormat = 0;
INT                        g_iFlicker = 4;

DWORD                      dwFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE;

IDirect3D8*                g_pD3D = NULL;
HANDLE                     g_hDevice = 0;      // handle of the gamepad
BOOL                       g_KeyDown[6];
BYTE                       g_PreviousState[6];
BOOL                       g_Pause = false;
BOOL                       g_UseReset = true;  // Use Reset API instead of CreateDevice/Release

// objects that need to be released:
IDirect3DDevice8*          g_pDev  = NULL;
IDirect3DVertexBuffer8*    g_pVB   = NULL;

#define NUMPRIMS 240
#define DA (360.0/(NUMPRIMS*2.0))
#define DR (DA*3.141592654/180.0)
// Define our screen space triangles.

static struct TheVerts { float x,y,z,w; DWORD color; } Verts[NUMPRIMS*3];

//------------------------------------------------------------------------------

struct StateDescription 
{
    DWORD MultisampleFormat;
    WCHAR* Text;
};

StateDescription g_AntialiasTypes[] =
{
    D3DMULTISAMPLE_NONE,                                    L"NONE", 
    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,            L"2_SAMPLES_MULTISAMPLE_LINEAR",
    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,          L"2_SAMPELS_MULTISAMPLE_QUINCUNX",
    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR, L"2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR",
    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,   L"2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR",
    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,            L"4_SAMPLES_MULTISAMPLE_LINEAR",
    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,          L"4_SAMPLES_MULTISAMPLE_GAUSSIAN",
    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,            L"4_SAMPLES_SUPERSAMPLE_LINEAR",
    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,          L"4_SAMPLES_SUPERSAMPLE_GAUSSIAN",
    D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN,          L"9_SAMPLES_MULTISAMPLE_GAUSSIAN",
    D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN,          L"9_SAMPLES_SUPERSAMPLE_GAUSSIAN",
};

INT g_AntialiasTypesCount = sizeof(g_AntialiasTypes) / sizeof(g_AntialiasTypes[0]);

StateDescription g_AntialiasFormats[] =
{
    D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5,   L"PREFILTER_FORMAT_X1R5G5B5",
    D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5,     L"PREFILTER_FORMAT_R5G6B5",
    D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8,   L"PREFILTER_FORMAT_X8R8G8B8",
    D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8,   L"PREFILTER_FORMAT_A8R8G8B8",
};

INT g_AntialiasFormatsCount = sizeof(g_AntialiasFormats) / sizeof(g_AntialiasFormats[0]);

//------------------------------------------------------------------------------
void InitPresentationParameters(D3DPRESENT_PARAMETERS *ppp)
{
    ZeroMemory(ppp, sizeof(*ppp));

    ppp->BackBufferWidth           = 640;
    ppp->BackBufferHeight          = 480;
    ppp->BackBufferFormat          = D3DFMT_X8R8G8B8;
    ppp->BackBufferCount           = 2;
    ppp->Windowed                  = false;   // Must be false for Xbox.
    ppp->EnableAutoDepthStencil    = true;
    ppp->AutoDepthStencilFormat    = D3DFMT_D24S8;
    ppp->SwapEffect                = D3DSWAPEFFECT_DISCARD;
    ppp->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    if (g_iType != 0)
        ppp->MultiSampleType       = g_AntialiasTypes[g_iType].MultisampleFormat
                                    | g_AntialiasFormats[g_iFormat].MultisampleFormat;

    WCHAR buf[1024];
    swprintf(buf, L"Flicker: %li  FSAA type: %s | %s\n", 
             g_iFlicker,
             g_AntialiasTypes[g_iType].Text,
             g_AntialiasFormats[g_iFormat].Text);
    OutputDebugString(buf);
}

//------------------------------------------------------------------------------
bool Init()
{
    // Create D3D 8.
    if(g_pD3D == NULL)
    {
        g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
        if (g_pD3D == NULL)
            return false;
    }

    // Set the screen mode.
    D3DPRESENT_PARAMETERS d3dpp;
    InitPresentationParameters(&d3dpp);

    // Create the device.
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
                             D3DDEVTYPE_HAL,
                             0,
                             D3DCREATE_HARDWARE_VERTEXPROCESSING,
                             &d3dpp,
                             &g_pDev) != S_OK)
        return false;

    // Create the vertex buffer.
    void* pVerts;

    UINT v = 0;
    for(UINT i=0; i<NUMPRIMS; i++)
    {
        Verts[v].x = 320.0f;
        Verts[v].y = 240.0f;
        Verts[v].z = 0.5f;
        Verts[v].w = 1.0f;
        Verts[v].color = 0xffffffff;
        v++;

        Verts[v].x = (FLOAT)(320. + 320.*(cos(i*2.0*DR)));
        Verts[v].y = (FLOAT)(240. + 320.*(sin(i*2.0*DR)));
        Verts[v].z = 0.5f;
        Verts[v].w = 1.0f;
        Verts[v].color = 0xffffffff;
        v++;

        Verts[v].x = (FLOAT)(320. + 320.*(cos((i*2.0+1.0)*DR)));
        Verts[v].y = (FLOAT)(240. + 320.*(sin((i*2.0+1.0)*DR)));
        Verts[v].z = 0.5f;
        Verts[v].w = 1.0f;
        Verts[v].color = 0xffffffff;
        v++;
    }
    if ( g_pDev->CreateVertexBuffer( sizeof(Verts), D3DUSAGE_WRITEONLY, dwFVF, D3DPOOL_MANAGED, &g_pVB ) != S_OK)
        return false;

    g_pVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );
    memcpy( (void*)pVerts, (void*)Verts, sizeof(Verts) );
    g_pVB->Unlock();

    g_pDev->SetFlickerFilter(g_iFlicker);

    return true;
}

//------------------------------------------------------------------------------
void Uninit()
{
    g_pVB->Release();
    g_pVB = NULL;

    g_pDev->Release();
    g_pDev = NULL;

    g_pD3D->Release();
    g_pD3D = NULL;
}

//------------------------------------------------------------------------------
bool InitGammaRamp()
{
    D3DGAMMARAMP ramp;
    DWORD i;

    for (i = 0; i <= 255; i++)
    {
        FLOAT f = i / 255.0f;

        // sRGB is a gamma of 2.2, but we'll approximate with 2:

        f = (FLOAT) sqrt(f);

        // Normalize and round:

        BYTE b = (BYTE) (f * 255.0f + 0.5f);

        ramp.red[i] = b;
        ramp.green[i] = b;
        ramp.blue[i] = b;
    }

    g_pDev->SetGammaRamp(0, &ramp);

    return true;
}

//------------------------------------------------------------------------------
void Paint()
{
    DWORD minFilter;
    DWORD magFilter;
    DWORD fillMode;
    DWORD lighting;
    DWORD colorKeyOp;
    DWORD shader;

    // Exercise the synchornization code.
    TheVerts* pVerts;
    
    g_pVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );

    // Transform the verticies to make the triangle spin so we can verify that
    // this actually works over a period of time.
    //
    static DWORD Time = GetTickCount();
    static DWORD CurrentTime;
     
    // If paused, just use the same time as last time:
    if (!g_Pause)
        CurrentTime = GetTickCount();

    // 180 seconds per rotation
    float spin = 2 * 3.14159f * (float)(CurrentTime - Time) / 180000.0f;

    DWORD i;

    for (i = 0; i < NUMPRIMS*3; i++)
    {

        float x = Verts[i].x;
        float y = Verts[i].y;

        x = x - 320.0f;
        y = y - 240.0f;

        pVerts[i].x = x * (float)cos(spin) - y * (float)sin(spin);
        pVerts[i].y = x * (float)sin(spin) + y * (float)cos(spin);

        pVerts[i].x += 320.0f;
        pVerts[i].y += 240.0f;
    }

    g_pVB->Unlock();

    // Clear the frame buffer, Zbuffer.
    g_pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00000000, 1.0, 0);

    // Draw the vertex streamsetup in stream 0.
    g_pDev->BeginScene();

    g_pDev->SetStreamSource( 0, g_pVB, sizeof(Verts[0]) );
    g_pDev->SetVertexShader( dwFVF );
    g_pDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, NUMPRIMS );      // Draw a single triangle.

    g_pDev->EndScene();

    // Set some state to ensure that Present() doesn't nuke it:
    g_pDev->SetVertexShader( D3DFVF_XYZ );

g_pDev->SetVertexShader( 0 );

    g_pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    g_pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pDev->SetTextureStageState(0, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_KILL);
    g_pDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    g_pDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    g_pDev->SetShaderConstantMode(D3DSCM_192CONSTANTS);

    // Flip the buffers.
    g_pDev->Present(NULL, NULL, NULL, NULL);

    // Check the state.
    g_pDev->SetShaderConstantMode(D3DSCM_96CONSTANTS);

    g_pDev->GetVertexShader(&shader);
    g_pDev->GetRenderState(D3DRS_FILLMODE, &fillMode);
    g_pDev->GetRenderState(D3DRS_LIGHTING, &lighting);
    g_pDev->GetTextureStageState(0, D3DTSS_COLORKEYOP, &colorKeyOp);
    g_pDev->GetTextureStageState(0, D3DTSS_MINFILTER, &minFilter);
    g_pDev->GetTextureStageState(0, D3DTSS_MAGFILTER, &magFilter);

    if ((shader != 0) ||
        (fillMode != D3DFILL_WIREFRAME) ||
        (lighting != FALSE) ||
        (colorKeyOp != D3DTCOLORKEYOP_KILL) ||
        (minFilter != D3DTEXF_POINT) || 
        (magFilter != D3DTEXF_POINT))
    {
        _asm int 3;
    }

    // Reset the state.
    g_pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_pDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    g_pDev->SetTextureStageState(0, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
}

//------------------------------------------------------------------------------
INT CheckInput()
{
    DWORD dwInsertions, dwRemovals, i;
    INT key = -1;
    
    //
    //  Check to see if a port 0 gamepad has come or gone.
    //
    if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
    {
        if(dwRemovals&XDEVICE_PORT0_MASK)
        {
            if(g_hDevice)
            {
                XInputClose(g_hDevice);
                g_hDevice = NULL;
            }
        }
        if(dwInsertions&XDEVICE_PORT0_MASK)
        {
            g_hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
        }
    }

    // If we have a device, get its state.
    if (g_hDevice)
    {
        XINPUT_STATE InputState;

        XInputGetState(g_hDevice, &InputState);
        // check to see if we need to change modes
        for (i = 0; i < 6; i++)
        {
            if ((InputState.Gamepad.bAnalogButtons[i] == 0) != 
                (g_PreviousState[i] == 0))
            {
                g_KeyDown[i] = !g_KeyDown[i];
                if (g_KeyDown[i])
                    key = i;
            }
            g_PreviousState[i] = InputState.Gamepad.bAnalogButtons[i];
        }
    }

    return key;
}

//-----------------------------------------------------------------------------
// Name: AddModulus()
//-----------------------------------------------------------------------------
INT AddModulus(INT Value, INT Increment, INT Max)
{
    Value += Increment;
    if (Value >= Max)
        Value -= Max;
    if (Value < 0)
        Value += Max;

    return Value;
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
void __cdecl main()
{
    D3DPRESENT_PARAMETERS pp;
    INT keyDown;

    //Initialize core peripheral port support
    XInitDevices(0,NULL);
    
    // Initialize the gamepad
    if(XDEVICE_PORT0_MASK & XGetDevices(XDEVICE_TYPE_GAMEPAD))
    {
        g_hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
    }

    if (!Init() || !InitGammaRamp())
    {
        OutputDebugString( L"Could not initialize\n");
        _asm int 3;
    }

    while (TRUE)
    {
        while (TRUE)
        {
            Paint();

            keyDown = CheckInput();

            if (keyDown == XINPUT_GAMEPAD_BLACK)
                g_Pause = !g_Pause;
            else if (keyDown == XINPUT_GAMEPAD_A)
            {
                g_iType = AddModulus(g_iType, 1, g_AntialiasTypesCount);
                break;
            }
            else if (keyDown == XINPUT_GAMEPAD_B)
            {
                g_iType = AddModulus(g_iType, -1, g_AntialiasTypesCount);
                break;
            }
            else if (keyDown == XINPUT_GAMEPAD_X)
            {
                g_iFlicker = AddModulus(g_iFlicker, 1, 5);
                break;
            }
            else if (keyDown == XINPUT_GAMEPAD_Y)
            {
                g_iFlicker = AddModulus(g_iFlicker, -1, 5);
                break;
            }
        }

        g_pDev->PersistDisplay();

        if (g_UseReset)
        {
            InitPresentationParameters(&pp);
            if (g_pDev->Reset(&pp) != S_OK)
            {
                OutputDebugString( L"Could not Reset\n");
                _asm int 3;
            }
        }
        else
        {
            Uninit();
            if (!Init() || !InitGammaRamp())
            {
                OutputDebugString( L"Could not initialize\n");
                _asm int 3;
            }
        }

        g_pDev->SetFlickerFilter(g_iFlicker);
    }
}

