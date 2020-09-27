/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	main.cpp

Abstract:

	dsmem

Author:

	Robert Heitkamp (robheit) 8-Dec-2001

Environment:

	Xbox DVT only

Revision History:

	8-Dec-2001	robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <stdio.h>
#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>
#include "bitfont.h"

//------------------------------------------------------------------------------
//	Structures
//------------------------------------------------------------------------------
struct TVertex
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
    float u, v;         // Texture coordinates
};

struct Joystick
{
	float	leftStickX;
	float	leftStickY;
	float	rightStickX;
	float	rightStickY;
	float	a;
	float	b;
	float	x;
	float	y;
	float	black;
	float	white;
	float	leftTrigger;
	float	rightTrigger;
	BOOL	back;
	BOOL	start;
	BOOL	dPadLeft;
	BOOL	dPadRight;
	BOOL	dPadUp;
	BOOL	dPadDown;
	BOOL	leftStick;
	BOOL	rightStick;
};

struct BufferList
{
	LPDIRECTSOUNDBUFFER	pBuffer;
	BufferList*			pNext;
};

//------------------------------------------------------------------------------
//	Constants:
//------------------------------------------------------------------------------
static const double cPi					= 3.1415926535;
static const double cIScale				= 2.0 * cPi / 48000.0;
static const double cDataScale			= 32767.0;
static const DWORD	cJOYBUTTON_X		= 0x00000001;
static const DWORD	cJOYBUTTON_Y		= 0x00000002;
static const DWORD	cJOYBUTTON_WHITE	= 0x00000004;
static const DWORD	cJOYBUTTON_A		= 0x00000008;
static const DWORD	cJOYBUTTON_B		= 0x00000010;
static const DWORD	cJOYBUTTON_BLACK	= 0x00000020;
static const DWORD	cJOYBUTTON_LTRIG	= 0x00000040;
static const DWORD	cJOYBUTTON_RTRIG	= 0x00000080;
static const DWORD	cJOYBUTTON_LSTICK	= 0x00000100;
static const DWORD	cJOYBUTTON_RSTICK	= 0x00000200;
static const DWORD	cJOYBUTTON_UP		= 0x00000400;
static const DWORD	cJOYBUTTON_DOWN		= 0x00000800;
static const DWORD	cJOYBUTTON_LEFT		= 0x00001000;
static const DWORD	cJOYBUTTON_RIGHT	= 0x00002000;
static const DWORD	cJOYBUTTON_BACK		= 0x00004000;
static const DWORD	cJOYBUTTON_START	= 0x00008000;
static const DWORD	cJOYBUTTON_ALL		= 0xffffffff;
static const float  cMinThreshold		= 0.0001f;

//------------------------------------------------------------------------------
//	Global Variables:
//------------------------------------------------------------------------------
static BitFont				g_font;
static TVertex				g_prText[4];
static HANDLE				g_inputHandles[4];
static Joystick				g_joysticks[4];
static LPDIRECTSOUNDBUFFER	g_pDSBuffer		= NULL;
static IDirect3D8*		    g_d3d			= NULL;
static IDirect3DDevice8*	g_pDevice		= NULL;
static IDirect3DTexture8*	g_pd3dtText		= NULL;
static LPDIRECTSOUND8		g_pDSound		= NULL;

//------------------------------------------------------------------------------
//	Static Functions
//------------------------------------------------------------------------------
static HRESULT InitGraphics(void);
static void InitInput(void);
static void ReleaseInput(UINT port);
static void ReleaseInput(void);
static void GetJoystickStates(void);
static BOOL GetJoystickState(UINT port);
static BOOL IsPortValid(UINT port);
static DWORD UINTToPort(UINT port);
static void InitInput(UINT port);
static void Draw(void);

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

extern "C" DWORD g_dwDirectSoundDebugLevel; // = DPFLVL_DEFAULT;

//------------------------------------------------------------------------------
//	dsmem_BasicTest
//------------------------------------------------------------------------------
HRESULT
dsmem_BasicTest(void)
/*++

Routine Description:

    Basic Test for dsp

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
	DSBUFFERDESC			dsbd;
	WAVEFORMATEX			wfx;
	DSCAPS					dsCaps;
	LPVOID					pBuffer;
	XDEVICE_PREALLOC_TYPE	devType = { XDEVICE_TYPE_GAMEPAD, 4 };

	// Init the graphics
	InitGraphics();

	// Init the input devices
	XInitDevices(1, &devType);
		
	ASSERT(DirectSoundCreate(NULL, &g_pDSound, NULL) == DS_OK);

	// Get the number of SGE's
	g_pDSound->GetCaps(&dsCaps);

	// It's always safe to zero out stack allocated structs
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));

	// setup the waveformat description to be PCM, 48000hz, 16bit, mono
	wfx.cbSize			= 0;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= 1;
	wfx.nSamplesPerSec	= 48000;
	wfx.wBitsPerSample	= 16;
	wfx.nBlockAlign		= 2;
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

	// setup the dsound buffer description with the waveformatex
	dsbd.dwFlags		= 0;
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.lpwfxFormat	= &wfx;
    dsbd.lpMixBins		= NULL;

	// Create the dsound buffer
	dsbd.dwBufferBytes	= (dsCaps.dwFreeBufferSGEs-1) * 4096;
	ASSERT(g_pDSound->CreateSoundBuffer(&dsbd, &g_pDSBuffer, NULL) == DS_OK);
	g_pDSBuffer->Release();

	dsbd.dwBufferBytes	= 0;
	ASSERT(g_pDSound->CreateSoundBuffer(&dsbd, &g_pDSBuffer, NULL) == DS_OK);
	pBuffer = XPhysicalAlloc(dsCaps.dwFreeBufferSGEs * 4096,MAXULONG_PTR, 0,
							 PAGE_READWRITE);
	ASSERT(g_pDSBuffer->SetBufferData(pBuffer, dsCaps.dwFreeBufferSGEs * 4096) == DS_OK);
	g_pDSBuffer->Release();
	g_pDSound->Release();

	while(1)
		Draw();

    return S_OK;
}

//------------------------------------------------------------------------------
//	dsmem_StartTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
dsmem_StartTest( 
			  IN HANDLE	LogHandle 
			  )
/*++

Routine Description:

    The harness entry into the dsp tests

Arguments:

    LogHandle - a handle to a logging object

Return Value:

    None

--*/
{
    HRESULT	hr	= S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

    //
    // Test dsp in the mannor it was meant to be called
    //

	dsmem_BasicTest();
}

//------------------------------------------------------------------------------
//	dsmem_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
dsmem_EndTest(VOID)
/*++

Routine Description:

    The exit function for the test harness

Arguments:

    None

Return Value:

    None

--*/
{
}

//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
void 
__cdecl 
main(void)
/*++

Routine Description:

    the exe entry point

Arguments:

    None

Return Value:

    None

--*/
{
    dsmem_StartTest( NULL );
    dsmem_EndTest();
}

//------------------------------------------------------------------------------
//	InitGraphics
//------------------------------------------------------------------------------
static HRESULT
InitGraphics(void)
/*++

Routine Description:

	Initializes the graphics

Arguments:

	None

Return Value:

	None

--*/
{
	HRESULT					hr;
	D3DPRESENT_PARAMETERS	d3dpp;
    D3DLOCKED_RECT			d3dlr;
	int						i;
	
	// Create an instance of a Direct3D8 object 
	g_d3d = Direct3DCreate8(D3D_SDK_VERSION);
	if(g_d3d == NULL)
		return E_FAIL;
	
	// Setup the present parameters
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth					= 640;
	d3dpp.BackBufferHeight					= 480;
	d3dpp.BackBufferFormat					= D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount					= 1;
	d3dpp.Flags								= 0;
	d3dpp.MultiSampleType					= D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow						= NULL;
	d3dpp.Windowed							= FALSE;
	d3dpp.EnableAutoDepthStencil			= TRUE;
	d3dpp.AutoDepthStencilFormat			= D3DFMT_D24S8;
	d3dpp.Flags								= 0;
	d3dpp.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
	d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_ONE;

	// Create the device
	hr = g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, 
						     D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, 
						     &g_pDevice);
	if(FAILED(hr))
		return hr;

    // Create a buffer for the text
    hr = g_pDevice->CreateTexture(320, 240, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, 
									&g_pd3dtText);
    if(FAILED(hr))
        return hr;
	
    // Clear the texture
    g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
    for (i = 0; i < 240; i++) {
        memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
    }
    g_pd3dtText->UnlockRect(0);

	// Text plane
    g_prText[0].x = 0.0f;
    g_prText[0].y = 480.0f;
    g_prText[0].u = 0.0f;
    g_prText[0].v = 240.0f;

    g_prText[1].x = 0.0f;
    g_prText[1].y = 0.0f;
    g_prText[1].u = 0.0f;
    g_prText[1].v = 0.0f;

    g_prText[2].x = 640.0f;
    g_prText[2].y = 0.0f;
    g_prText[2].u = 320.0f;
    g_prText[2].v = 0.0f;

    g_prText[3].x = 640.0f;
    g_prText[3].y = 480.0f;
    g_prText[3].u = 320.0f;
    g_prText[3].v = 240.0f;

    for (i = 0; i < 4; i++) {
        g_prText[i].z = 0.0f;
        g_prText[i].rhw = 1.0f;
    }

	g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	g_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g_pDevice->SetTexture(0, g_pd3dtText);
	g_pDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);

	return S_OK;
}

//------------------------------------------------------------------------------
//  ::InitInput
//------------------------------------------------------------------------------
void 
InitInput(void)
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
            InitInput(i);
        else
            ReleaseInput(i);
    }
}

//------------------------------------------------------------------------------
//  ::ReleaseInput
//------------------------------------------------------------------------------
void 
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
//  ::ReleaseInput
//------------------------------------------------------------------------------
void 
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
//  ::GetJoystickStates
//------------------------------------------------------------------------------
void 
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
//  ::GetJoystickState
//------------------------------------------------------------------------------
BOOL
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
//  ::IsPortValid
//------------------------------------------------------------------------------
BOOL
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
        InitInput(port);
        return (g_inputHandles[port] != NULL) ? TRUE : FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
//  ::UINTToPort
//------------------------------------------------------------------------------
DWORD
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
    default:
        __asm int 3;
        return 0xFFFFFFFF;
    }


}

//------------------------------------------------------------------------------
//  ::InitInput
//------------------------------------------------------------------------------
static void 
InitInput(
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
//	::Draw
//------------------------------------------------------------------------------
void
Draw(void)
{
	int					i;
	BOOL				buttonA[4];
    IDirect3DSurface8*	pd3ds;
	DWORD				status;
    D3DLOCKED_RECT		d3dlr;

	// Clear the device
	g_pDevice->Clear(0, NULL, 
					 D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
					 0xff000000, 1.0f, 0);

	// Clear the texture
	g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
	for(i = 0; i < 240; i++) 
		memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
	g_pd3dtText->UnlockRect(0);

	g_pd3dtText->GetSurfaceLevel(0, &pd3ds);

	g_font.DrawText(pd3ds, L"Press Back or Start to exit", 25, 25, 0, 0xffffffff, 0);

	pd3ds->Release();

	g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));

	g_pDevice->EndScene();
	g_pDevice->Present(NULL, NULL, NULL, NULL);

	// toggle bypass?
	GetJoystickStates();
	for(i=0; i<4; ++i)
	{
		// Quit?
		if(g_joysticks[i].back) 
			HalReturnToFirmware(HalQuickRebootRoutine);
	}
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest	
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dsp )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dsp )
    EXPORT_TABLE_ENTRY( "StartTest", dsmem_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", dsmem_EndTest )
END_EXPORT_TABLE( dsp )
