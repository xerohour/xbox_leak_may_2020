/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	main.cpp

Abstract:

	Test for SetRolloffCurve API

Author:

	Robert Heitkamp (robheit) 07-Feb-2002

Environment:

	Xbox only

Revision History:

	07-Feb-2002 robheit
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
#include "dsstdfx.h"
#include "medialoader.h"

//------------------------------------------------------------------------------
//	Enums
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Structures
//------------------------------------------------------------------------------
struct TVertex
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
    float u, v;         // Texture coordinates
};

struct Vertex
{
	FLOAT		x, y, z, rhw;	// The transformed position for the vertex
	D3DCOLOR	color;			// Color of vertex
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

typedef Vertex Point[6];

//------------------------------------------------------------------------------
//	Constants:
//------------------------------------------------------------------------------
static const DWORD	c_joybuttonX		= 0x00000001;
static const DWORD	c_joybuttonY		= 0x00000002;
static const DWORD	c_joybuttonWhite	= 0x00000004;
static const DWORD	c_joybuttonA		= 0x00000008;
static const DWORD	c_joybuttonB		= 0x00000010;
static const DWORD	c_joybuttonBlack	= 0x00000020;
static const DWORD	c_joybuttonLtrig	= 0x00000040;
static const DWORD	c_joybuttonRtrig	= 0x00000080;
static const DWORD	c_joybuttonLstick	= 0x00000100;
static const DWORD	c_joybuttonRstick	= 0x00000200;
static const DWORD	c_joybuttonUp		= 0x00000400;
static const DWORD	c_joybuttonDown		= 0x00000800;
static const DWORD	c_joybuttonLeft		= 0x00001000;
static const DWORD	c_joybuttonRight	= 0x00002000;
static const DWORD	c_joybuttonBack		= 0x00004000;
static const DWORD	c_joybuttonStart	= 0x00008000;
static const DWORD	c_joybuttonAll		= 0xffffffff;
static const float	c_pointSize			= 2.0f;
static const int	c_maxPoints			= 4096;

//------------------------------------------------------------------------------
//	Global Variables:
//------------------------------------------------------------------------------
static XMEDIAPACKET			g_xmp;
static BitFont				g_font;
static Vertex				g_box[4];
static TVertex				g_prText[4];
static Vertex				g_slider[6];
static int					g_numPoints;
static Vertex				g_source[2];
static HANDLE				g_inputHandles[4];
static Joystick				g_joysticks[4];
static Vertex				g_cursor[4];
static float				g_x;
static float				g_sx;
static float				g_y;
static float				g_top;
static float				g_bottom;
static float				g_left;
static float				g_right;
static float				g_dx;
static float				g_dy;
static float				g_rolloffCurve[c_maxPoints];
static BOOL					g_bDraw			= FALSE;
static Vertex*				g_pCurve		= NULL;
static Point*				g_pPoints		= NULL;
static LPDIRECTSOUNDBUFFER	g_pDSBuffer		= NULL;
static XFileMediaObject*	g_pFile			= NULL;
static IDirect3D8*		    g_d3d			= NULL;
static IDirect3DDevice8*	g_pDevice		= NULL;
static IDirect3DTexture8*	g_pd3dtText		= NULL;
static LPDIRECTSOUND8		g_pDSound		= NULL;

//------------------------------------------------------------------------------
//	Static Functions
//------------------------------------------------------------------------------
static HRESULT InitBuffer(void);
static HRESULT InitGraphics(void);
static void InitInput(void);
static void ReleaseInput(UINT port);
static void ReleaseInput(void);
static void GetJoystickStates(void);
static BOOL GetJoystickState(UINT port);
static BOOL IsPortValid(UINT port);
static DWORD UINTToPort(UINT port);
static void InitInput(UINT port);
static void CreateScene(void);
static void MoveCursor(float, float);
static void MoveSlider(float);
static void SetNumPoints(int);
static float Lint(float, float, float);
static void SetPoint(int);
static void SetRolloffCurve(void);

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
//	rolloffcurve_BasicTest
//------------------------------------------------------------------------------
HRESULT
rolloffcurve_BasicTest(void)
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
    HRESULT					hr	= S_OK;
	int						i;
    IDirect3DSurface8*		pd3ds;
    D3DLOCKED_RECT			d3dlr;
	BOOL					buttonA[4];
	WCHAR					buffer[32];
	BOOL					bDraw;
	XDEVICE_PREALLOC_TYPE	devType = { XDEVICE_TYPE_GAMEPAD, 4 };
	float					dx;
	float					dy;
	float					sx;
	BOOL					bLastAddingPoints	= FALSE;
	BOOL					bAddingPoints		= FALSE;
	float					factor				= 4.0f;
	float					tolerance			= 0.2f;
	float					numPoints			= (float)g_numPoints;
	float					fInc				= 0.2f;
	BOOL					bDPadUp[4]			= { FALSE, FALSE, FALSE, FALSE };
	BOOL					bDPadDown[4]		= { FALSE, FALSE, FALSE, FALSE };
	BOOL					bDPadLeft[4]		= { FALSE, FALSE, FALSE, FALSE };
	BOOL					bDPadRight[4]		= { FALSE, FALSE, FALSE, FALSE };

	// Init the input devices
	XInitDevices(1, &devType);

	// Init the graphics
	InitGraphics();

	// Create the scene
	CreateScene();

	// Init audio
	InitBuffer();

	// Render state
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

	// Loop & sleep forever
	while(1)
	{
		g_pDevice->Clear(0, NULL, 
						 D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
						 0xff000000, 1.0f, 0);

		// Setup for drawing
		g_pDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

		// Draw the box
		g_pDevice->DrawPrimitiveUP(D3DPT_LINELOOP, 3, g_box, sizeof(Vertex));

		// Draw the curve
		if(g_pCurve)
			g_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, g_numPoints, g_pCurve, sizeof(Vertex));

		// Draw the "points"
		if(g_pPoints)
			g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, g_numPoints*2, g_pPoints, sizeof(Vertex));

		// Draw the slider
		g_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, g_slider, sizeof(Vertex));

		// Draw the source
		g_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, g_source, sizeof(Vertex));

		// Draw the cursor
		g_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 2, g_cursor, sizeof(Vertex));

		// Setup to draw text
		g_pDevice->SetTexture(0, g_pd3dtText);
		g_pDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);

		// Clear the texture
		g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
		for(i = 0; i < 240; i++) 
			memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
		g_pd3dtText->UnlockRect(0);

		g_pd3dtText->GetSurfaceLevel(0, &pd3ds);

		wsprintf(buffer, L"Points: %d", g_numPoints);
		g_font.DrawText(pd3ds, buffer, 25, 25, 0, 0xffffffff, 0);

		pd3ds->Release();

		g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));
		g_pDevice->SetTexture(0, NULL);

		g_pDevice->EndScene();
		g_pDevice->Present(NULL, NULL, NULL, NULL);

		// toggle bypass?
		GetJoystickStates();
		for(bAddingPoints=FALSE, bDraw=FALSE, sx=0.0f, dx=0.0f, dy=0.0f, i=0; i<4; ++i)
		{
			// Quit?
			if(g_joysticks[i].back) 
			{
				//g_pDSBuffer->Release();
				//g_pDSound->Release();
				HalReturnToFirmware(HalQuickRebootRoutine);
			}

			// Move the cursor?
			if(fabs(g_joysticks[i].leftStickX) > tolerance)
				dx += g_joysticks[i].leftStickX * factor;
			if(fabs(g_joysticks[i].leftStickY) > tolerance)
				dy += g_joysticks[i].leftStickY * factor;

			// Move the slider?
			if(fabs(g_joysticks[i].rightStickX) > tolerance)
				sx += g_joysticks[i].rightStickX * factor;

			// Draw
			if(g_joysticks[i].a >= 0.25f)
				bDraw = TRUE;

			// Add/Remove points
			if(g_joysticks[i].dPadUp)
			{
				if(!bDPadUp[i])
				{
					bDPadUp[i] = TRUE;
					numPoints = (float)floor(numPoints) + 1.0f;
				}
				else
					numPoints += fInc;
				bAddingPoints = TRUE;
			}
			else 
				bDPadUp[i] = FALSE;

			if(g_joysticks[i].dPadDown)
			{
				if(!bDPadDown[i])
				{
					bDPadDown[i] = TRUE;
					numPoints = (float)floor(numPoints) - 1.0f;
				}
				else
					numPoints -= fInc;
				if(numPoints < 0.0f)
					numPoints = 0.0f;
				bAddingPoints = TRUE;
			}
			else
				bDPadDown[i] = FALSE;

			if(g_joysticks[i].dPadRight)
			{
				if(!bDPadRight[i])
				{
					bDPadRight[i] = TRUE;
					numPoints = (float)floor(numPoints) + 10.0f;
				}
				else
					numPoints += fInc * 10.0f;
				bAddingPoints = TRUE;
			}
			else 
				bDPadRight[i] = FALSE;
		
			if(g_joysticks[i].dPadLeft)
			{
				if(!bDPadLeft[i])
				{
					bDPadLeft[i] = TRUE;
					numPoints = (float)floor(numPoints) - 10.0f;
				}
				else
					numPoints -= fInc * 10.0f;
				if(numPoints < 0.0f)
					numPoints = 0.0f;
				bAddingPoints = TRUE;
			}
			else
				bDPadLeft[i] = FALSE;
		}
		
		if((g_bDraw && !bDraw) || (!bAddingPoints && bLastAddingPoints))
			SetRolloffCurve();

		bLastAddingPoints	= bAddingPoints;
		g_bDraw				= bDraw;

		if(numPoints > (float)c_maxPoints)
			numPoints = (float)c_maxPoints;
		SetNumPoints((int)numPoints);
		MoveCursor(dx, dy);
		MoveSlider(sx);
	}

	// This will never be hit
    return hr;
}

//------------------------------------------------------------------------------
//	rolloffcurve_StartTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
rolloffcurve_StartTest( 
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
    HRESULT			hr	= S_OK;
	MEDIAFILEDESC	mediaFiles[] = 
	{
		{"wav/heli.wav", "t:\\media", COPY_IF_NOT_EXIST},
		{NULL, NULL, 0}
	};

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	// Load the media content
	ASSERT(LoadMedia(mediaFiles) == S_OK);

    //
    // Test dsp in the mannor it was meant to be called
    //

	rolloffcurve_BasicTest();
}

//------------------------------------------------------------------------------
//	rolloffcurve_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
rolloffcurve_EndTest(VOID)
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
    rolloffcurve_StartTest( NULL );
    rolloffcurve_EndTest();
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

	return S_OK;
}

//------------------------------------------------------------------------------
//	InitBuffer
//------------------------------------------------------------------------------
static HRESULT
InitBuffer(void)
/*++

Routine Description:

	Init the sound buffer

Arguments:

	None

Return Value:

	S_OK, E_XXXX

--*/
{	
	DSBUFFERDESC		dsbd;
	LPCWAVEFORMATEX		pWfx;
	HRESULT				hr = S_OK;
	DSMIXBINVOLUMEPAIR	mixBinVolumePair[2];
	DSMIXBINS			mixBins;
    LPDSEFFECTIMAGEDESC	pDesc;
    DSEFFECTIMAGELOC	effectLoc = {0};

	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));

	// Create the DSound object
	hr = DirectSoundCreate(NULL, &g_pDSound, NULL);
	ASSERT(SUCCEEDED(hr));

	DirectSoundUseFullHRTF();

    effectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
    effectLoc.dwCrosstalkIndex   = I3DL2_CHAIN_XTALK;

	// Download the effects image
	ASSERT(XAudioDownloadEffectsImage("dsstdfx", &effectLoc, XAUDIO_DOWNLOADFX_XBESECTION, &pDesc) == S_OK);

	// Load the wav file
	hr = XWaveFileCreateMediaObject("t:\\media\\heli.wav", &pWfx, &g_pFile);
	ASSERT(SUCCEEDED(hr));
	ASSERT(NULL != g_pFile);

	// Create the buffer
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_CTRL3D;
	dsbd.dwBufferBytes	= 0;
	dsbd.lpwfxFormat	= (LPWAVEFORMATEX)pWfx;
	dsbd.lpMixBins		= NULL;
	
	// Create the direct sound buffer
	hr = g_pDSound->CreateSoundBuffer(&dsbd, &g_pDSBuffer, NULL);
	ASSERT(SUCCEEDED(hr));
	ASSERT(NULL != g_pDSBuffer);

	// Get the length of the media file
	hr = g_pFile->GetLength(&g_xmp.dwMaxSize);
	ASSERT(SUCCEEDED(hr));
	ASSERT(0 != g_xmp.dwMaxSize);

	if(0 == g_xmp.dwMaxSize)
		hr = E_OUTOFMEMORY;

	// Create a buffer of the size of the length
	g_xmp.pvBuffer = XPhysicalAlloc(g_xmp.dwMaxSize, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE);
	ASSERT(NULL != g_xmp.pvBuffer);

	if(NULL == g_xmp.pvBuffer)
		hr = E_OUTOFMEMORY;

	// Precache out the buffer data
	hr = g_pFile->Process(NULL, &g_xmp);
	ASSERT(SUCCEEDED(hr));

	// Set the buffer data
	hr = g_pDSBuffer->SetBufferData(g_xmp.pvBuffer, g_xmp.dwMaxSize);
	ASSERT(SUCCEEDED(hr));

	// Set the mixbins
//	mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FRONT_LEFT;
//	mixBinVolumePair[0].lVolume		= 0;
//	mixBinVolumePair[1].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
//	mixBinVolumePair[1].lVolume		= 0;
//	mixBins.dwMixBinCount			= 2;
//	mixBins.lpMixBinVolumePairs		= mixBinVolumePair;
//	g_pDSBuffer->SetMixBins(&mixBins);

	// Release the file
	g_pFile->Release();

	// Play the buffer
	g_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

    // Source Settings
    g_pDSBuffer->SetPosition(0.0f, 0.0f, 0.0f, DS3D_DEFERRED);
    g_pDSBuffer->SetVelocity(0.0f, 0.0f, 0.0f, DS3D_DEFERRED);
    g_pDSBuffer->SetVolume(0);
	g_pDSBuffer->SetHeadroom(0);
	g_pDSBuffer->SetMinDistance(1.0f, DS3D_DEFERRED);
	g_pDSBuffer->SetMaxDistance(100.0f, DS3D_DEFERRED);

	// Listener Settings
    g_pDSound->SetOrientation(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, DS3D_DEFERRED);
    g_pDSound->SetPosition(0.0f, 0.0f, 0.0f, DS3D_DEFERRED);
    g_pDSound->SetVelocity(0.0f, 0.0f, 0.0f, DS3D_DEFERRED );

    // Commit position/velocity changes
    g_pDSound->CommitDeferredSettings();

	return hr;
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
//	CreateScene
//------------------------------------------------------------------------------
static void
CreateScene(void)
/*++

Routine Description:

	Creates all the data for the scene

Arguments:

	None

Return Value:

	None

--*/
{
	g_top		= 81.0f;
	g_bottom	= 349.0f;
	g_left		= 46.0f;
	g_right		= 594.f;
	g_dx		= g_right - g_left;
	g_dy		= g_bottom - g_top;

	// The box
	g_box[0].x		= g_left - 1.0f;		
	g_box[0].y		= g_top - 1.0f;
	g_box[0].z		= 0.0f;		
	g_box[0].rhw	= 1.0f;		
	g_box[0].color	= 0xff00ff00;		

	g_box[1].x		= g_right + 1.0f;
	g_box[1].y		= g_box[0].y;		
	g_box[1].z		= g_box[0].z;		
	g_box[1].rhw	= g_box[0].rhw;		
	g_box[1].color	= g_box[0].color;		

	g_box[2].x		= g_box[1].x;		
	g_box[2].y		= g_bottom + 1.0f;
	g_box[2].z		= g_box[0].z;		
	g_box[2].rhw	= g_box[0].rhw;		
	g_box[2].color	= g_box[0].color;		

	g_box[3].x		= g_box[0].x;		
	g_box[3].y		= g_box[2].y;		
	g_box[3].z		= g_box[0].z;		
	g_box[3].rhw	= g_box[0].rhw;		
	g_box[3].color	= g_box[0].color;		

	// Cursor
	g_x					= (g_right + g_left) / 2.0f;
	g_y					= (g_bottom + g_top) / 2.0f;
	g_cursor[0].x		= g_x;
	g_cursor[0].y		= g_y - 5.0f;
	g_cursor[0].z		= 0.0f;
	g_cursor[0].rhw		= 1.0f;
	g_cursor[0].color	= 0xffffffff;
	g_cursor[1].x		= g_x;
	g_cursor[1].y		= g_y + 5.0f;
	g_cursor[1].z		= g_cursor[0].z;
	g_cursor[1].rhw		= g_cursor[0].rhw;
	g_cursor[1].color	= g_cursor[0].color;
	g_cursor[2].x		= g_x - 5.0f;
	g_cursor[2].y		= g_y;
	g_cursor[2].z		= g_cursor[0].z;
	g_cursor[2].rhw		= g_cursor[0].rhw;
	g_cursor[2].color	= g_cursor[0].color;
	g_cursor[3].x		= g_x + 5.0f;
	g_cursor[3].y		= g_y;
	g_cursor[3].z		= g_cursor[0].z;
	g_cursor[3].rhw		= g_cursor[0].rhw;
	g_cursor[3].color	= g_cursor[0].color;

	// Slider
	g_slider[0].x		= g_box[0].x;
	g_slider[0].y		= g_box[2].y + 20.0f;
	g_slider[0].z		= 0.0f;
	g_slider[0].rhw		= 1.0f;
	g_slider[0].color	= 0xff0000ff;
	g_slider[1].x		= g_slider[0].x;
	g_slider[1].y		= g_slider[0].y + 40.0f;
	g_slider[1].z		= g_slider[0].z;
	g_slider[1].rhw		= g_slider[0].rhw;
	g_slider[1].color	= g_slider[0].color;
	g_slider[2].x		= g_box[1].x;
	g_slider[2].y		= g_slider[0].y;
	g_slider[2].z		= g_slider[0].z;
	g_slider[2].rhw		= g_slider[0].rhw;
	g_slider[2].color	= g_slider[0].color;
	g_slider[3].x		= g_slider[2].x;
	g_slider[3].y		= g_slider[1].y;
	g_slider[3].z		= g_slider[0].z;
	g_slider[3].rhw		= g_slider[0].rhw;
	g_slider[3].color	= g_slider[0].color;
	g_slider[4].x		= g_slider[0].x;
	g_slider[4].y		= (g_slider[1].y + g_slider[0].y) / 2.0f;
	g_slider[4].z		= g_slider[0].z;
	g_slider[4].rhw		= g_slider[0].rhw;
	g_slider[4].color	= g_slider[0].color;
	g_slider[5].x		= g_slider[2].x;
	g_slider[5].y		= g_slider[4].y;
	g_slider[5].z		= g_slider[0].z;
	g_slider[5].rhw		= g_slider[0].rhw;
	g_slider[5].color	= g_slider[0].color;

	// Slider bar (source)
	g_sx				= g_slider[0].x + 1.0f;
	g_source[0].x		= g_sx;
	g_source[0].y		= g_slider[0].y + 5.0f;
	g_source[0].z		= 0.0f;
	g_source[0].rhw		= 1.0f;
	g_source[0].color	= 0xffffffff;
	g_source[1].x		= g_source[0].x;
	g_source[1].y		= g_slider[1].y - 5.0f;
	g_source[1].z		= g_source[0].z;
	g_source[1].rhw		= g_source[0].rhw;
	g_source[1].color	= g_source[0].color;
}

//------------------------------------------------------------------------------
//	MoveCursor
//------------------------------------------------------------------------------
static void
MoveCursor(
		   IN float	dx,
		   IN float	dy
		   )
/*++

Routine Description:

	Moves the cursor within the box

Arguments:

	IN dx -	Pixels to move in X
	IN dy -	Pixels to move in Y

Return Value:

	None

--*/
{
	int	index;

	g_x += dx;
	g_y += dy;

	if(g_x >= g_box[2].x)
		g_x = g_box[2].x - 1.0f;
	else if(g_x <= g_box[0].x)
		g_x = g_box[0].x + 1.0f;

	if(g_y >= g_box[2].y)
		g_y = g_box[2].y - 1.0f;
	else if(g_y <= g_box[0].y)
		g_y = g_box[0].y + 1.0f;

	g_cursor[0].x		= g_x;
	g_cursor[0].y		= g_y - 5.0f;
	g_cursor[1].x		= g_x;
	g_cursor[1].y		= g_y + 5.0f;
	g_cursor[2].x		= g_x - 5.0f;
	g_cursor[2].y		= g_y;
	g_cursor[3].x		= g_x + 5.0f;
	g_cursor[3].y		= g_y;

	// Draw?
	if(g_bDraw && g_pCurve && g_pPoints)
	{
		index = (int)(((g_x - g_left) / g_dx) * (float)g_numPoints + 0.5f);

		if(index < 1)
			index = 1;
		else if(index > g_numPoints)
			index = g_numPoints;

		g_pCurve[index].y = g_y;
		SetPoint(index-1);
	}
}

//------------------------------------------------------------------------------
//	MoveSlider
//------------------------------------------------------------------------------
static void
MoveSlider(
		   IN float	sx
		   )
/*++

Routine Description:

	Moves the slider (source)

Arguments:

	IN sx -	Pixels to move in X

Return Value:

	None

--*/
{
	float	x;

	g_sx += sx;

	if(g_sx >= g_box[2].x)
		g_sx = g_box[2].x - 1.0f;
	else if(g_sx <= g_box[0].x)
		g_sx = g_box[0].x + 1.0f;

	g_source[0].x	= g_sx;
	g_source[1].x	= g_source[0].x;

	x = 100.0f * (g_sx - g_box[0].x) / (g_box[2].x - g_box[0].x);
	if(x < 1.0f)
		x = 1.0f;
	g_pDSBuffer->SetPosition(x, 0.0f, 0.0f, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	SetNumPoints
//------------------------------------------------------------------------------
static void
SetNumPoints(
			 int	numPoints
			 )
/*++

Routine Description:

	Sets the number of points in the rolloff curve

Arguments:

	IN numPoints -	Number of points

Return Value:

	None

--*/
{
	float	fInc;
	int		i;

	if((numPoints < 0) || (numPoints == g_numPoints))
		return;

	if(g_pCurve)
	{
		delete [] g_pCurve;
		g_pCurve = NULL;
	}
	if(g_pPoints)
	{
		delete [] g_pPoints;
		g_pPoints = NULL;
	}

	g_numPoints = numPoints;
	if(g_numPoints)
	{
		g_pCurve			= new Vertex [g_numPoints+1];
		g_pPoints			= new Point [g_numPoints];
		g_pCurve[0].x		= g_left;
		g_pCurve[0].y		= g_top;
		g_pCurve[0].z		= 0.0f;
		g_pCurve[0].rhw		= 1.0f;
		g_pCurve[0].color	= 0xffffff00;

		g_pCurve[g_numPoints].x		= g_right;
		g_pCurve[g_numPoints].y		= g_bottom;
		g_pCurve[g_numPoints].z		= g_pCurve[0].z;
		g_pCurve[g_numPoints].rhw	= g_pCurve[0].rhw;
		g_pCurve[g_numPoints].color	= g_pCurve[0].color;
		SetPoint(g_numPoints-1);

		fInc = g_dx / g_numPoints;
		for(i=1; i<g_numPoints; ++i)
		{
			g_pCurve[i].x		= ((float)i * fInc) + g_left;
			g_pCurve[i].y		= Lint(g_pCurve[0].y, g_pCurve[g_numPoints].y, (float)i / (float)g_numPoints);
			g_pCurve[i].z		= g_pCurve[0].z;
			g_pCurve[i].rhw		= g_pCurve[0].rhw;
			g_pCurve[i].color	= g_pCurve[0].color;
			SetPoint(i-1);
		}
	}
}

//------------------------------------------------------------------------------
//	Lint
//------------------------------------------------------------------------------
static float
Lint(
	 IN float	fA,
	 IN float	fB,
	 IN float	fT
	 )
/*++

Routine Description:

	Performs a linear interpolation between a and b

Arguments:

	IN fA -	A
	IN fB -	B
	IN fT -	T

Return Value:

	The linear interpolated value

--*/
{
	return (((fB - fA) * fT) + fA);
}

//------------------------------------------------------------------------------
//	SetPoint
//------------------------------------------------------------------------------
static void
SetPoint(
		 IN int	index
		 )
/*++

Routine Description:

	Sets a point value based on the curve

Arguments:

	IN index -	Index of point to set

Return Value:

	None

--*/
{
	g_pPoints[index][0].x		= g_pCurve[index+1].x - c_pointSize;
	g_pPoints[index][0].y		= g_pCurve[index+1].y - c_pointSize;
	g_pPoints[index][0].z		= 0.0f;
	g_pPoints[index][0].rhw		= 1.0f;
	g_pPoints[index][0].color	= 0xffff0000;

	g_pPoints[index][2].x		= g_pCurve[index+1].x - c_pointSize;
	g_pPoints[index][2].y		= g_pCurve[index+1].y + c_pointSize;
	g_pPoints[index][2].z		= g_pPoints[index][0].z;
	g_pPoints[index][2].rhw		= g_pPoints[index][0].rhw;
	g_pPoints[index][2].color	= g_pPoints[index][0].color;

	g_pPoints[index][1].x		= g_pCurve[index+1].x + c_pointSize;
	g_pPoints[index][1].y		= g_pCurve[index+1].y - c_pointSize;
	g_pPoints[index][1].z		= g_pPoints[index][0].z;
	g_pPoints[index][1].rhw		= g_pPoints[index][0].rhw;
	g_pPoints[index][1].color	= g_pPoints[index][0].color;

	g_pPoints[index][3].x		= g_pCurve[index+1].x + c_pointSize;
	g_pPoints[index][3].y		= g_pCurve[index+1].y + c_pointSize;
	g_pPoints[index][3].z		= g_pPoints[index][0].z;
	g_pPoints[index][3].rhw		= g_pPoints[index][0].rhw;
	g_pPoints[index][3].color	= g_pPoints[index][0].color;

	g_pPoints[index][4].x		= g_pPoints[index][2].x;
	g_pPoints[index][4].y		= g_pPoints[index][2].y;
	g_pPoints[index][4].z		= g_pPoints[index][0].z;
	g_pPoints[index][4].rhw		= g_pPoints[index][0].rhw;
	g_pPoints[index][4].color	= g_pPoints[index][0].color;

	g_pPoints[index][5].x		= g_pPoints[index][1].x;
	g_pPoints[index][5].y		= g_pPoints[index][1].y;
	g_pPoints[index][5].z		= g_pPoints[index][0].z;
	g_pPoints[index][5].rhw		= g_pPoints[index][0].rhw;
	g_pPoints[index][5].color	= g_pPoints[index][0].color;
}

//------------------------------------------------------------------------------
//	SetRolloffCurve
//------------------------------------------------------------------------------
static void
SetRolloffCurve(void)
/*++

Routine Description:

	Sets the rolloff curve for the voice

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	if(g_pCurve)
	{
		for(i=0; i<g_numPoints; ++i)
		{
			g_rolloffCurve[i] = 1.0f - ((g_pCurve[i+1].y - g_top) / g_dy);
			if(g_rolloffCurve[i] > 1.0f)
				g_rolloffCurve[i] = 1.0f;
			else if(g_rolloffCurve[i] < 0.0f)
				g_rolloffCurve[i] = 0.0f;
		}
		g_pDSBuffer->SetRolloffCurve(g_rolloffCurve, g_numPoints, DS3D_IMMEDIATE);
	}

	else
		g_pDSBuffer->SetRolloffCurve(NULL, 0, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest	
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( rolloffcurve )
#pragma data_seg()

BEGIN_EXPORT_TABLE( rolloffcurve )
    EXPORT_TABLE_ENTRY( "StartTest", rolloffcurve_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", rolloffcurve_EndTest )
END_EXPORT_TABLE( rolloffcurve )
