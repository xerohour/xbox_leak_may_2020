/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	dsp.cpp

Abstract:

	GP DSP tests

Author:

	Robert Heitkamp (robheit) 30-Apr-2001

Environment:

	Xbox DVT only

Revision History:

	30-Apr-2001 robheit
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
#include "dspserver.h"
#include "bitfont.h"
#include "medialoader.h"

//------------------------------------------------------------------------------
//	Enums
//------------------------------------------------------------------------------
enum BlockType
{
	BT_HEADER,
	BT_DSPIMAGE
};

enum MixMode
{
	DRY,
	MIXED,
	WET
};

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

struct Screen
{
	WCHAR	line[256];
	Screen*	pNext;
	Screen*	pPrev;
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
static const int	cNumLines			= 17;

//------------------------------------------------------------------------------
//	Global Variables:
//------------------------------------------------------------------------------
static XMEDIAPACKET			g_xmp;
static BitFont				g_font;
static TVertex				g_prText[4];
static HANDLE				g_inputHandles[4];
static Joystick				g_joysticks[4];
static LPDIRECTSOUNDBUFFER	g_pDSBuffer		= NULL;
static XFileMediaObject*	g_pFile			= NULL;
static IDirect3D8*		    g_d3d			= NULL;
static IDirect3DDevice8*	g_pDevice		= NULL;
static IDirect3DTexture8*	g_pd3dtText		= NULL;
static MixMode				g_mixMode		= DRY;
static LPDIRECTSOUND8		g_pDSound		= NULL;
static BufferList*			g_pBufferList	= NULL;
static Screen*				g_pScreen		= NULL;
static DWORD				g_dwCount		= 0;
static BOOL					g_bDump			= TRUE;

//------------------------------------------------------------------------------
//	Static Functions
//------------------------------------------------------------------------------
static HRESULT InitBuffer(void);
static HRESULT PlaySineWave(double, DSMIXBINS*);
static HRESULT PlayTriangleWave(double, DSMIXBINS*);
static HRESULT PlaySquareWave(double, DSMIXBINS*);
static HRESULT InitGraphics(void);
static void InitInput(void);
static void ReleaseInput(UINT port);
static void ReleaseInput(void);
static void GetJoystickStates(void);
static BOOL GetJoystickState(UINT port);
static BOOL IsPortValid(UINT port);
static DWORD UINTToPort(UINT port);
static void InitInput(UINT port);
static void ReleaseBuffers(void);
static void AddBuffer(LPDIRECTSOUNDBUFFER);
static void __cdecl PreCallback(void);
static void __cdecl Callback(LPDSEFFECTIMAGEDESC pImageDes, int count, 
					 char** pEffectNames, int* pEffectIndices);
static void __cdecl SetEffectData(DWORD dwIndex, DWORD dwOffset, LPCVOID pData, 
						  DWORD dwSize, DWORD dwFlags);
static void __cdecl SetI3DL2Listener(LPDSI3DL2LISTENER pData);

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
//	dsp_BasicTest
//------------------------------------------------------------------------------
HRESULT
dsp_BasicTest(void)
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
    WCHAR					buffer[256];
	BOOL					buttonA[4];
	BOOL					buttonB[4];
	XDEVICE_PREALLOC_TYPE	devType = { XDEVICE_TYPE_GAMEPAD, 4 };
	DSMIXBINVOLUMEPAIR		mixBinVolumePair[4];
	DSMIXBINS				mixBins;
	DWORD					refCount = 0;
	Screen*					pScreen;
	Screen*					pLast;

	// Load media

	// Init the graphics
	InitGraphics();

	// Init audio
	InitBuffer();

	// Init the input devices
	XInitDevices(1, &devType);

	// Allocate the screen
	g_pScreen			= new Screen;
	g_pScreen->pNext	= g_pScreen;
	g_pScreen->pPrev	= g_pScreen;
	wsprintf(g_pScreen->line, L"");
	pLast = g_pScreen;
	for(i=1; i<cNumLines; ++i)
	{
		pScreen			= new Screen;
		pScreen->pNext	= g_pScreen;
		pScreen->pPrev	= pLast;
		pLast->pNext	= pScreen;
		pLast			= pScreen;
		wsprintf(pScreen->line, L"");
	}

//	g_dwDirectSoundDebugLevel	= 0xffffffff;

    if(CDSPServer::Run(g_pDSound, 80, 1000, PreCallback, Callback, 
					   SetEffectData, SetI3DL2Listener))
	{
		// Play the buffer
		g_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

		// Play Sine Waves for test
		mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_2;
		mixBinVolumePair[0].lVolume		= 0;
		mixBins.dwMixBinCount			= 1;
		mixBins.lpMixBinVolumePairs		= mixBinVolumePair;
		PlaySineWave(100.0, &mixBins);

		mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_3;
		PlaySineWave(1000.0, &mixBins);

		mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_4;
		PlaySineWave(10000.0, &mixBins);

		// Waves for modulation
		mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_5;
		PlaySineWave(0.5, &mixBins);
		
		mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_6;
		PlayTriangleWave(0.5, &mixBins);

		mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_7;
		PlaySquareWave(0.5, &mixBins);

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

		// Loop & sleep forever
		while(1)
		{
		    g_pDevice->Clear(0, NULL, 
						     D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
						     0xff000000, 1.0f, 0);

			// Clear the texture
			g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
			for(i = 0; i < 240; i++) 
				memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
			g_pd3dtText->UnlockRect(0);

		    g_pd3dtText->GetSurfaceLevel(0, &pd3ds);

			switch(g_mixMode)
			{
			case DRY:
				g_font.DrawText(pd3ds, L"Effects Loop: Dry (0%)", 25, 25, 0, 0xff00ff00, 0);
				break;
			case MIXED:
				g_font.DrawText(pd3ds, L"Effects Loop: Mixed (50%)", 25, 25, 0, 0xff00ff00, 0);
				break;
			case WET:
				g_font.DrawText(pd3ds, L"Effects Loop: Wet (100%)", 25, 25, 0, 0xff00ff00, 0);
				break;
			}
			if(g_bDump)
				g_font.DrawText(pd3ds, L"Dump Data: Yes", 170, 25, 0, 0xffff0000, 0);
			else
				g_font.DrawText(pd3ds, L"Dump Data: No", 170, 25, 0, 0xffff0000, 0);

			// Draw the scrolling display
			for(pScreen=g_pScreen, i=0; i<cNumLines; ++i, pScreen=pScreen->pNext)
				g_font.DrawText(pd3ds, pScreen->line, 25, i*11+35, 0, 0xffffffff, 0);

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
				{
					CDSPServer::Stop();
					g_pDSBuffer->Release();
					ReleaseBuffers();
					g_pDSound->Release();
					HalReturnToFirmware(HalQuickRebootRoutine);
				}

				// Start - restart buffer
				if(g_joysticks[i].start) 
					g_pDSBuffer->Play(0, 0, DSBPLAY_FROMSTART | DSBPLAY_LOOPING);

				if(g_joysticks[i].a > cMinThreshold)
					buttonA[i] = TRUE;
				else if(buttonA[i])
				{
					buttonA[i] = FALSE;
					switch(g_mixMode)
					{
					case DRY:
						g_mixMode = MIXED;
						mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_0;
						mixBinVolumePair[0].lVolume		= 0;
						mixBinVolumePair[1].dwMixBin	= DSMIXBIN_FXSEND_1;
						mixBinVolumePair[1].lVolume		= 0;
						mixBinVolumePair[2].dwMixBin	= DSMIXBIN_FRONT_LEFT;
						mixBinVolumePair[2].lVolume		= 0;
						mixBinVolumePair[3].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
						mixBinVolumePair[3].lVolume		= 0;
						mixBins.dwMixBinCount			= 4;
						mixBins.lpMixBinVolumePairs		= mixBinVolumePair;
						g_pDSBuffer->SetMixBins(&mixBins);
						break;

					case MIXED:
						g_mixMode = WET;
						mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FXSEND_0;
						mixBinVolumePair[0].lVolume		= 0;
						mixBinVolumePair[1].dwMixBin	= DSMIXBIN_FXSEND_1;
						mixBinVolumePair[1].lVolume		= 0;
						mixBins.dwMixBinCount			= 2;
						mixBins.lpMixBinVolumePairs		= mixBinVolumePair;
						g_pDSBuffer->SetMixBins(&mixBins);
						break;

					case WET:
						g_mixMode = DRY;
						mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FRONT_LEFT;
						mixBinVolumePair[0].lVolume		= 0;
						mixBinVolumePair[1].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
						mixBinVolumePair[1].lVolume		= 0;
						mixBins.dwMixBinCount			= 2;
						mixBins.lpMixBinVolumePairs		= mixBinVolumePair;
						g_pDSBuffer->SetMixBins(&mixBins);
						break;
					}
				}

				if(g_joysticks[i].b > cMinThreshold)
					buttonB[i] = TRUE;
				else if(buttonB[i])
				{
					buttonB[i]	= FALSE;
					g_bDump		= !g_bDump;
				}
			}
		}

		// Cleanup - not that this will ever happen...
		g_pDSBuffer->Release();
		ReleaseBuffers();
		g_pDSound->Release();
	}

	CDSPServer::Stop();

    return S_OK;
}

//------------------------------------------------------------------------------
//	dsp_StartTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
dsp_StartTest( 
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
	MEDIAFILEDESC	mediaFiles[] = 
	{
		{"wav/piano.wav", "t:\\media", COPY_IF_NOT_EXIST},
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
    //
    // Test dsp in the mannor it was meant to be called
    //

	dsp_BasicTest();
}

//------------------------------------------------------------------------------
//	dsp_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
dsp_EndTest(VOID)
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
    dsp_StartTest( NULL );
    dsp_EndTest();
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
	d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;

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

	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));

	hr = DirectSoundCreate(NULL, &g_pDSound, NULL);
	ASSERT(SUCCEEDED(hr));

	// Load the ball wave file, set up the DSBUFFERDESC
	//hr = XWaveFileCreateMediaObject("t:\\media\\HeavyGuitar.wav", &pWfx, &g_pFile);
	//hr = XWaveFileCreateMediaObject("t:\\media\\acoustic.wav", &pWfx, &g_pFile);
	hr = XWaveFileCreateMediaObject("t:\\media\\piano.wav", &pWfx, &g_pFile);

	ASSERT(SUCCEEDED(hr));
	ASSERT(NULL != g_pFile);

	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= 0;
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
	mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FRONT_LEFT;
	mixBinVolumePair[0].lVolume		= 0;
	mixBinVolumePair[1].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
	mixBinVolumePair[1].lVolume		= 0;
	mixBins.dwMixBinCount			= 2;
	mixBins.lpMixBinVolumePairs		= mixBinVolumePair;
	g_pDSBuffer->SetMixBins(&mixBins);
	g_pFile->Release();

	return hr;
}

//------------------------------------------------------------------------------
//	::PlaySineWave
//------------------------------------------------------------------------------
static HRESULT 
PlaySineWave(
			 IN double		frequency,	// Hertz
			 IN DSMIXBINS*	pMixBins	// Mixbins to output to
			 )
/*++

Routine Description:

	Plays a sine wave

Arguments:

	IN Frequency -	Frequency of sine wave 
	IN pMixBins -	Mixbins to output to

Return Value:

	S_OK on success, any other error on failure

--*/
{
	DWORD				i;
	DWORD				size;
	double				dF;
	double				dInc;
    HRESULT				hr			= S_OK;
	LPDIRECTSOUNDBUFFER	pDSBuffer	= NULL;
	short*				pData		= NULL;
	DWORD				dwBytes;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
		
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
	dsbd.dwBufferBytes	= (DWORD)((double)wfx.nAvgBytesPerSec / frequency);
	if(dsbd.dwBufferBytes % 2)
		++dsbd.dwBufferBytes;
	dsbd.dwFlags		= 0;
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.lpwfxFormat	= &wfx;
    dsbd.lpMixBins		= pMixBins;

	// Create the dsound buffer
	hr = g_pDSound->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL);

	// continue forever (or until an error occurs)
	if(SUCCEEDED(hr))
	{
		AddBuffer(pDSBuffer);

		// get a buffer from dsound using lock
		hr = pDSBuffer->Lock(0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0);
		if(SUCCEEDED(hr))
		{
			size	= dsbd.dwBufferBytes / 2;
			dInc	= (2.0 * cPi) / (double)size;
			for(dF=0.0, i=0;  i<size; ++i, dF+=dInc)
				pData[i] = (SHORT)(sin(dF) * cDataScale);
		}
    }
    
	// start the dsound buffer playing in a loop mode
	if(SUCCEEDED(hr))
		hr = pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

    // leave with the buffer still playing..
    return hr;
}

//------------------------------------------------------------------------------
//	::PlayTriangleWave
//------------------------------------------------------------------------------
static HRESULT 
PlayTriangleWave(
				 IN double		frequency,	// Hertz
				 IN DSMIXBINS*	pMixBins	// Mixbins to output to
				 )
/*++

Routine Description:

	Plays a triangle wave

Arguments:

	IN Frequency -	Frequency of wave 
	IN pMixBins -	Mixbins to output to

Return Value:

	S_OK on success, any other error on failure

--*/
{
	DWORD				i;
	DWORD				size;
	double				dF;
	double				dInc;
    HRESULT				hr			= S_OK;
	LPDIRECTSOUNDBUFFER	pDSBuffer	= NULL;
	short*				pData		= NULL;
	DWORD				dwBytes;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
		
	// It's always safe to zero out stack allocated structs
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	ZeroMemory(&wfx, sizeof(DSBUFFERDESC));

	// setup the waveformat description to be PCM, 48000hz, 16bit, mono
	wfx.cbSize			= 0;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= 1;
	wfx.nSamplesPerSec	= 48000;
	wfx.wBitsPerSample	= 16;
	wfx.nBlockAlign		= 2;
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

	// setup the dsound buffer description with the waveformatex
	dsbd.dwBufferBytes	= (DWORD)((double)wfx.nAvgBytesPerSec / frequency);
	if(dsbd.dwBufferBytes % 2)
		++dsbd.dwBufferBytes;
	dsbd.dwFlags		= 0;
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.lpwfxFormat	= &wfx;
    dsbd.lpMixBins		= pMixBins;

	// Create the dsound buffer
	hr = g_pDSound->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL);

	// continue forever (or until an error occurs)
	if(SUCCEEDED(hr))
	{
		AddBuffer(pDSBuffer);

		// get a buffer from dsound using lock
		hr = pDSBuffer->Lock(0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0);
		if(SUCCEEDED(hr))
		{
			size	= dsbd.dwBufferBytes / 2;
			dInc	= (cDataScale * 2.0) / ((double)size / 2.0);
			for(dF=-cDataScale, i=0;  i<size/2; ++i, dF+=dInc)
				pData[i] = (short)dF;
			for(;  i<size; ++i, dF-=dInc)
				pData[i] = (short)dF;
		}
    }
    
	// start the dsound buffer playing in a loop mode
	if(SUCCEEDED(hr))
		hr = pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

    // leave with the buffer still playing..
    return hr;
}

//------------------------------------------------------------------------------
//	::PlaySquareWave
//------------------------------------------------------------------------------
static HRESULT 
PlaySquareWave(
			   IN double		frequency,	// Hertz
			   IN DSMIXBINS*	pMixBins	// Mixbins to output to
			   )
/*++

Routine Description:

	Plays a square wave

Arguments:

	IN Frequency -	Frequency of wave 
	IN pMixBins -	Mixbins to output to

Return Value:

	S_OK on success, any other error on failure

--*/
{
	DWORD				i;
	DWORD				size;
    HRESULT				hr			= S_OK;
	LPDIRECTSOUNDBUFFER	pDSBuffer	= NULL;
	short*				pData		= NULL;
	DWORD				dwBytes;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
		
	// It's always safe to zero out stack allocated structs
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	ZeroMemory(&wfx, sizeof(DSBUFFERDESC));

	// setup the waveformat description to be PCM, 48000hz, 16bit, mono
	wfx.cbSize			= 0;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= 1;
	wfx.nSamplesPerSec	= 48000;
	wfx.wBitsPerSample	= 16;
	wfx.nBlockAlign		= 2;
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

	// setup the dsound buffer description with the waveformatex
	dsbd.dwBufferBytes	= (DWORD)((double)wfx.nAvgBytesPerSec / frequency);
	if(dsbd.dwBufferBytes % 2)
		++dsbd.dwBufferBytes;
	dsbd.dwFlags		= 0;
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.lpwfxFormat	= &wfx;
    dsbd.lpMixBins		= pMixBins;

	// Create the dsound buffer
	hr = g_pDSound->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL);

	// continue forever (or until an error occurs)
	if(SUCCEEDED(hr))
	{
		AddBuffer(pDSBuffer);

		// get a buffer from dsound using lock
		hr = pDSBuffer->Lock(0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0);
		if(SUCCEEDED(hr))
		{
			size	= dsbd.dwBufferBytes / 2;
			for(i=0;  i<size/2; ++i)
				pData[i] = -(short)cDataScale;
			for(;  i<size; ++i)
				pData[i] = (short)cDataScale;
		}
    }
    
	// start the dsound buffer playing in a loop mode
	if(SUCCEEDED(hr))
		hr = pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

    // leave with the buffer still playing..
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
//	ReleaseBuffers
//------------------------------------------------------------------------------
void
ReleaseBuffers(void)
/*++

Routine Description:

	Releases all DSound buffers

Arguments:

	None

Return Value:

	None

--*/
{
	BufferList*	pNext;

	while(g_pBufferList)
	{
		g_pBufferList->pBuffer->Release();
		pNext = g_pBufferList->pNext;
		delete g_pBufferList;
		g_pBufferList = pNext;
	}
}

//------------------------------------------------------------------------------
//	AddBuffer
//------------------------------------------------------------------------------
void
AddBuffer(
		  IN LPDIRECTSOUNDBUFFER	pBuffer
		  )
/*++

Routine Description:

	Adds a DSound buffer to the buffer list

Arguments:

	IN pBuffer -	Buffer to add

Return Value:

	None

--*/
{
	BufferList*	pbl = new BufferList;

	pbl->pBuffer	= pBuffer;
	pbl->pNext		= g_pBufferList;
	g_pBufferList	= pbl;
}


//------------------------------------------------------------------------------
//	PreCallback
//------------------------------------------------------------------------------
void __cdecl 
PreCallback(void)
/*++

Routine Description:

	Callback before download effects image

Arguments:

	None

Return Value:

	None

--*/
{
	if(g_bDump)
	{
		wsprintf(g_pScreen->line, L"%lu: PreCallback(void)", g_dwCount++);
		g_pScreen = g_pScreen->pNext;
	}
}

//------------------------------------------------------------------------------
//	Callback
//------------------------------------------------------------------------------
void __cdecl 
Callback(
	     IN LPDSEFFECTIMAGEDESC	pImageDes, 
	     IN int					count, 
	     IN char**				pEffectNames, 
	     IN int*				pEffectIndices
	     )
/*++

Routine Description:

	Callback after download effects image

Arguments:

	IN pImageDes -		Pointer to the DSEFFECTIMAGEDESC 
						structure that is returned by the 
						method, which describes the DSP 
						scratch image. 
	IN count -			Number of effects in image
	IN pEffectNames -	Array of effect name strings 
						(each null terminated)
	IN pEffectIndices -	Array of effect indices

Return Value:

	None

--*/
{
	int		i;
	WCHAR	buffer[256];

	if(g_bDump)
	{
		wsprintf(g_pScreen->line, L"%lu: Callback()", g_dwCount++);
		g_pScreen = g_pScreen->pNext;

		for(i=0; i<count; ++i)
		{
			MultiByteToWideChar(CP_ACP, 0, pEffectNames[i], -1, buffer, 256);
			wsprintf(g_pScreen->line, L"        %d: %s", pEffectIndices[i], buffer);
			g_pScreen = g_pScreen->pNext;
		}
	}
}

//------------------------------------------------------------------------------
//	SetEffectData
//------------------------------------------------------------------------------
void __cdecl 
SetEffectData(
			  IN DWORD		dwIndex, 
			  IN DWORD		dwOffset, 
			  IN LPCVOID	pData, 
			  IN DWORD		dwSize, 
			  IN DWORD		dwFlags
			  )
/*++

Routine Description:

	Called after DSound::SetEffectData

Arguments:

	IN dwIndex -	Effect index
	IN dwOffset -	Offset into pData
	IN pData -		Effect data 
	IN dwSize -		Size of pData 
	IN dwFlags -	Flags

Return Value:

	None

--*/
{
	DWORD	i;
	WCHAR	buffer[3];

	if(g_bDump)
	{
		wsprintf(g_pScreen->line, L"%lu: SetEffectData()", g_dwCount++);
		g_pScreen = g_pScreen->pNext;
		if(dwSize == sizeof(DWORD))
		{
			wsprintf(g_pScreen->line, L"        index: %lu, offset: %lu, data: %lu, size: %lu, flags: %lu",
					 dwIndex, dwOffset, *(DWORD*)pData, dwSize, dwFlags);
			g_pScreen = g_pScreen->pNext;
		}
		else
		{
			wsprintf(g_pScreen->line, L"        dwIndex = %lu", dwIndex);
			g_pScreen = g_pScreen->pNext;
			wsprintf(g_pScreen->line, L"        dwOffset = %lu", dwOffset);
			g_pScreen = g_pScreen->pNext;
			wsprintf(g_pScreen->line, L"        pData = ");
			for(i=0; i<dwSize && i<250; ++i)
			{
				wsprintf(buffer, L"%02x", *(((char*)pData)+i));
				lstrcat(g_pScreen->line, buffer);
			}
			g_pScreen = g_pScreen->pNext;
			wsprintf(g_pScreen->line, L"        dwSize = %lu", dwSize);
			g_pScreen = g_pScreen->pNext;
			wsprintf(g_pScreen->line, L"        dwFlags = %lu", dwFlags);
			g_pScreen = g_pScreen->pNext;
		}
	}
}

//------------------------------------------------------------------------------
//	SetI3DL2Listener
//------------------------------------------------------------------------------
void __cdecl 
SetI3DL2Listener(
				 IN LPDSI3DL2LISTENER pData
				 )
/*++

Routine Description:

	Called after DSound::SetI3DL2Listener

Arguments:

	IN pData -	New I3DL2 settings

Return Value:

	None

--*/
{
	if(g_bDump)
	{
		wsprintf(g_pScreen->line, L"%lu: SetI3DL2Listener()", g_dwCount++);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        lRoom = %ld", pData->lRoom);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        lRoomHF = %ld", pData->lRoomHF);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flRoomRolloffFactor = %f", pData->flRoomRolloffFactor);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flDecayTime = %f", pData->flDecayTime);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flDecayHFRatio = %f", pData->flDecayHFRatio);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        lReflections = %ld", pData->lReflections);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flReflectionsDelay = %f", pData->flReflectionsDelay);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        lReverb = %ld", pData->lReverb);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flReverbDelay = %f", pData->flReverbDelay);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flDiffusion = %f", pData->flDiffusion);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flDensity = %f", pData->flDensity);
		g_pScreen = g_pScreen->pNext;
		wsprintf(g_pScreen->line, L"        flHFReference = %f", pData->flHFReference);
		g_pScreen = g_pScreen->pNext;
	}
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest	
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dsp )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dsp )
    EXPORT_TABLE_ENTRY( "StartTest", dsp_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", dsp_EndTest )
END_EXPORT_TABLE( dsp )
