/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	main.cpp

Abstract:

	DSound Mixbin Test

Author:

	Robert Heitkamp (robheit) 09-Oct-2001

Environment:

	Xbox only

Notes:

	Requires user interaction for verification

Revision History:

	09-Oct-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#define DSOUND_IFACE_VERSION 4000

#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xgmath.h>
#include <macros.h>
#include "bitfont.h"
#include "meter.h"
#include "mixbinsImage.h"

//------------------------------------------------------------------------------
//	Constants:
//------------------------------------------------------------------------------
enum Mixbins
{
	FRONTLEFT,
	FRONTRIGHT,
	CENTER,
	LFE,
	BACKLEFT,
	BACKRIGHT,
	I3DL2,
	XTALK_FL,
	XTALK_FR,
	XTALK_BL,
	XTALK_BR,
	FXSEND0,
	FXSEND1,
	FXSEND2,
	FXSEND3,
	FXSEND4,
	FXSEND5,
	FXSEND6,
	FXSEND7,
	FXSEND8,
	FXSEND9,
	FXSEND10,
	FXSEND11,
	FXSEND12,
	FXSEND13,
	FXSEND14,
	FXSEND15,
	FXSEND16,
	FXSEND17,
	FXSEND18,
	FXSEND19,
	NUMMIXBINS
};

//------------------------------------------------------------------------------
//	Structures
//------------------------------------------------------------------------------
struct TextVertex
{
	FLOAT	x, y, z, rhw; // The transformed position for the vertex
	FLOAT	u, v;
};

//------------------------------------------------------------------------------
//	Constants:
//------------------------------------------------------------------------------
static const double cPi			= 3.1415926535;
static const double cIScale		= 2.0 * cPi / 48000.0;
static const double cDataScale	= 32767.0;

//------------------------------------------------------------------------------
//	Global Variables:
//------------------------------------------------------------------------------
static CMixbinMeter			g_meters[NUMMIXBINS];
static DSMIXBINVOLUMEPAIR	g_mixBinVolumePair[8];
static DSMIXBINS			g_mixBins;
static double				g_frequency;
static IDirect3D8*		    g_d3d			= NULL;
static IDirect3DDevice8*	g_pDevice		= NULL;
static LPDIRECTSOUND8		g_pDSound		= NULL;
static IDirect3DSurface8*   g_pBackBuffer	= NULL;
static LPDSEFFECTIMAGEDESC	g_pEffectsImage	= NULL;
static BitFont				g_font;

//------------------------------------------------------------------------------
//	Static Functions
//------------------------------------------------------------------------------
static HRESULT PlaySineWave(LPDIRECTSOUNDBUFFER*, DSMIXBINS*, double, WORD);
static HRESULT InitGraphics(void);
static void UpdateMeters(void);
static Mixbins ToMixbins(int);
static int ToDSound(Mixbins);
static void ValidateMeters(int);
static const char* ToText(Mixbins);

//------------------------------------------------------------------------------
//	Externals
//------------------------------------------------------------------------------
HANDLE g_hLog;

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
//	mixbins_BasicTest
//------------------------------------------------------------------------------
HRESULT
mixbins_BasicTest(void)
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
    D3DLOCKED_RECT			d3dlr;
    WCHAR					buffer[256];
	BOOL					buttonA[4];
	XDEVICE_PREALLOC_TYPE	devType = { XDEVICE_TYPE_GAMEPAD, 4 };
	DWORD					refCount = 0;
	XGMATRIX				identityMatrix;
	float					lineWidth = 1.0f;
	float					y;
	float					x;
	float					dx;
	LONGLONG				frequency;
	LONGLONG				current;
	LONGLONG				last;
	int						mixbin;
	BOOL					bMatch;
	LPDIRECTSOUNDBUFFER		pBuffer[7];
	LPDIRECTSOUNDBUFFER		pSubmixBuffer;
	int						ii;
	BOOL					bNoSignal[NUMMIXBINS];
	int						c;
	int						numChannels;
	int						loop;
	DSBUFFERDESC			dsbd;
	WAVEFORMATEX			wfx;
	BOOL					bSubmix = FALSE;

	// Query the performance frequency
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	g_frequency = (double) frequency;

	// Set the seed
	QueryPerformanceCounter((LARGE_INTEGER*)&last);
	srand((unsigned long)last);
	
	// Init the graphics
	InitGraphics();

	// Create the DSound object
	hr = DirectSoundCreate(NULL, &g_pDSound, NULL);
	if(FAILED(hr))
		return hr;

	// Download the effects image
	hr = XAudioDownloadEffectsImage("mixbinsImage", NULL, XAUDIO_DOWNLOADFX_XBESECTION,
									&g_pEffectsImage);
	if(FAILED(hr))
		return hr;
	
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
	g_pDevice->SetRenderState(D3DRS_LINEWIDTH, *((DWORD*)&lineWidth));

	// Set the heights of the mixbins
	for(i=0; i<NUMMIXBINS; ++i)
		g_meters[i].SetHeight(175.0f);

	// Speakers
	x	= 250.0f;
	dx	= 25.0f;
	y	= 50.0f;
	g_meters[FRONTLEFT].SetLocation(x, y); x += dx;
	g_meters[FRONTLEFT].SetLabel(L"FL");
	g_meters[FRONTRIGHT].SetLocation(x, y); x += dx;
	g_meters[FRONTRIGHT].SetLabel(L"FR");
	g_meters[CENTER].SetLocation(x, y); x += dx;
	g_meters[CENTER].SetLabel(L"C");
	g_meters[LFE].SetLocation(x, y); x += dx;
	g_meters[LFE].SetLabel(L"LFE");
	g_meters[BACKLEFT].SetLocation(x, y); x += dx;
	g_meters[BACKLEFT].SetLabel(L"BL");
	g_meters[BACKRIGHT].SetLocation(x, y); x += dx;
	g_meters[BACKRIGHT].SetLabel(L"BR");

	// I3DL2
	x += dx;
	g_meters[I3DL2].SetLocation(x, y); x += dx;
	g_meters[I3DL2].SetLabel(L"I3DL2");
	x += dx;

	// XTalk
	g_meters[XTALK_FL].SetLocation(x, y); x += dx;
	g_meters[XTALK_FL].SetLabel(L"XFL");
	g_meters[XTALK_FR].SetLocation(x, y); x += dx;
	g_meters[XTALK_FR].SetLabel(L"XFR");
	g_meters[XTALK_BL].SetLocation(x, y); x += dx;
	g_meters[XTALK_BL].SetLabel(L"XBL");
	g_meters[XTALK_BR].SetLocation(x, y); x += dx;
	g_meters[XTALK_BR].SetLabel(L"XBR");

	// FX Sends
	for(x=75.0f, i=FXSEND0; i<=FXSEND19; ++i, x+=25.0f)
	{
		g_meters[i].SetLocation(x, 250.0f); 
		wsprintf(buffer, L"%d", i-FXSEND0);
		g_meters[i].SetLabel(buffer);
	}

	// Setup mixbins structure for no output to mixbins
	g_mixBins.lpMixBinVolumePairs	= g_mixBinVolumePair;
	g_mixBins.dwMixBinCount			= 0;

	// Create the sine waves
	for(c=1; c<=6; ++c)
	{
		hr = PlaySineWave(&pBuffer[c], &g_mixBins, 440.0f, 1);
		if(FAILED(hr))
			return hr;
	}

	// Create the mixin buffer (with no output mixbins)
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_MIXIN;
	dsbd.dwBufferBytes	= 0;	// ??? What does this need to be
	dsbd.lpwfxFormat	= NULL;
    dsbd.lpMixBins		= &g_mixBins;

	hr = g_pDSound->CreateSoundBuffer(&dsbd, &pSubmixBuffer, NULL);
	if(FAILED(hr))
		return hr;

	for(i=0; i<NUMMIXBINS; ++i)
		bNoSignal[i] = FALSE;

	// Loop & sleep forever
	numChannels = 0;
	for(loop=0; loop<61; )
	{
		UpdateMeters();

		// Are all active meters receiving a signal?
		// Note: If submixing is enabled, none of the meters should
		// be showing anything so don't error under that situation
		if(!bSubmix)
		{
			for(i=0; i<NUMMIXBINS; ++i)
			{
				if(g_meters[i].IsActive() && (g_meters[i].GetLevel() > 0.0f) && 
				   (g_meters[i].GetCurrentLevel() == 0.0f))
				{
					bNoSignal[i] = TRUE;
				}
			}
		}

		// Draw the scene
		g_pDevice->Clear(0, NULL, 
						 D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
						 0xff000000, 1.0f, 0);

		// Draw the meters
		for(i=0; i<NUMMIXBINS; ++i)
			g_meters[i].Draw(g_pDevice, g_pBackBuffer);

		// Num channels
		wsprintf(buffer, L"Num Channels: %d", numChannels);
		g_font.DrawText(g_pBackBuffer, buffer, 75, 50, 0, 0xffffffff, 0);

		// Submix enabled
		wsprintf(buffer, L"Submix: %s", bSubmix ? L"TRUE" : L"FALSE");
		g_font.DrawText(g_pBackBuffer, buffer, 75, 70, 0, 0xffffffff, 0);

		g_pDevice->EndScene();
		g_pDevice->Present(NULL, NULL, NULL, NULL);

		QueryPerformanceCounter((LARGE_INTEGER*)&current);

		// Switch the mixbins?
		if(((double)(current - last) / g_frequency) > 1.0)
		{
			++loop;
			if(numChannels > 0)
			{
				// Check for errors
				ValidateMeters(numChannels);
				
				// Display any no signal errors
				for(i=0; i<NUMMIXBINS; ++i)
				{
					if(bNoSignal[i])
					{
						SETLOG(g_hLog, "robheit", "Mixbins", "ActiveCheck", "mixbins_BasicTest");
						DbgPrint("%dc: No signal detected on active mixbin: %s\n", numChannels+1, ToText((Mixbins)i));
						xLog(g_hLog, XLL_FAIL, "No signal detected on active mixbin: %s\n", ToText((Mixbins)i));
					}
					bNoSignal[i] = FALSE;
				}

				// Reset the mixbins
				g_mixBins.dwMixBinCount = 0;
				pBuffer[numChannels]->SetMixBins(&g_mixBins);
				
				// Reset the meters
				for(i=0; i<NUMMIXBINS; ++i)
				{
					g_meters[i].SetLevel(0.0f);
					g_meters[i].SetCurrentLevel(0.0f);
					g_meters[i].SetActive(FALSE);
					g_meters[i].ResetError();
				}
			}

			// Step to the next number of channels
			++numChannels;
			if(numChannels > 6)
				numChannels = 1;
			wsprintf(buffer, L"Num Channels: %d", numChannels);

			// How many mixbins to use?
			switch(numChannels)
			{
			case 1:
				g_mixBins.dwMixBinCount = rand() % 8 + 1;
				break;
			case 2:
				g_mixBins.dwMixBinCount = 2 * (rand() % 4 + 1);
				break;
			case 3:
				g_mixBins.dwMixBinCount = 3 * (rand() % 2 + 1);
				break;
			case 4:
				g_mixBins.dwMixBinCount = 4 * (rand() % 2 + 1);
				break;
			case 5:
				g_mixBins.dwMixBinCount = 5;
				break;
			case 6:
				g_mixBins.dwMixBinCount = 6;
				break;
			}
			
			// Select N random mixbins
			for(i=0; i<(int)g_mixBins.dwMixBinCount; ++i)
			{
				for(bMatch=TRUE; bMatch; )
				{
					bMatch	= FALSE;
					mixbin	= rand() % NUMMIXBINS;

					for(ii=0; !bMatch && ii<i; ++ii)
					{
						if(g_mixBinVolumePair[ii].dwMixBin == ToDSound((Mixbins)mixbin))
							bMatch = TRUE;
					}
				}
				g_mixBinVolumePair[i].dwMixBin	= ToDSound((Mixbins)mixbin);
				g_mixBinVolumePair[i].lVolume	= -(rand() % 3001);
			}

			// Set the mixbins and their headroom
			pBuffer[numChannels]->SetMixBins(&g_mixBins);
			for(i=0; i<(int)g_mixBins.dwMixBinCount; ++i)
				g_pDSound->SetMixBinHeadroom(g_mixBinVolumePair[i].dwMixBin, 0);

			// Set the outputbuffer 1 out of every 3 runs?
			bSubmix = !(rand() % 3);
			if(bSubmix)
				pBuffer[numChannels]->SetOutputBuffer(pSubmixBuffer);

			// Otherwise set the meters
			else
			{
				for(i=0; i<(int)g_mixBins.dwMixBinCount; ++i)
				{
					g_meters[ToMixbins(g_mixBinVolumePair[i].dwMixBin)].SetLevel((float)(10000 + g_mixBinVolumePair[i].lVolume) / 10000.0f);
					g_meters[ToMixbins(g_mixBinVolumePair[i].dwMixBin)].SetActive(TRUE);
				}
			}

			// Wait for the last data to be flushed
			Sleep(5);
			QueryPerformanceCounter((LARGE_INTEGER*)&last);
		}
	}

	// Release the buffers
	for(c=1; c<=6; ++c)
		pBuffer[c]->Release();
	pSubmixBuffer->Release();

	// Release DSound
	g_pDSound->Release();

    return S_OK;
}

//------------------------------------------------------------------------------
//	mixbins_StartTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
mixbins_StartTest( 
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

	mixbins_BasicTest();
}

//------------------------------------------------------------------------------
//	mixbins_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
mixbins_EndTest(VOID)
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
    mixbins_StartTest(NULL);
    mixbins_EndTest();

	// This will only be called when the test runs as an xbe rather than a lib
	HalReturnToFirmware(HalQuickRebootRoutine);
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

	// Get the back buffer
    g_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer);

	return hr;
}

//------------------------------------------------------------------------------
//	::PlaySineWave
//------------------------------------------------------------------------------
static HRESULT 
PlaySineWave(
			 OUT LPDIRECTSOUNDBUFFER*	pBuffer,		// Output buffer
			 IN DSMIXBINS*				pMixBins,		// Mixbins to output to
			 IN double					frequency,		// Hertz
			 IN	WORD					numChannels		// Number of channels
			 )
/*++

Routine Description:

	Plays a sine wave

Arguments:

	OUT pBuffer -		DSound buffer constructed
	IN pMixBins -		Mixbins to output to
	IN Frequency -		Frequency of sine wave 
	IN numChannels -	Number of channels

Return Value:

	S_OK on success, any other error on failure

--*/
{
	DWORD				i;
	DWORD				index;
	DWORD				size;
	double				dF;
	double				dInc;
    HRESULT				hr			= S_OK;
	short*				pData		= NULL;
	DWORD				dwBytes;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
	SHORT				value;
	DWORD				c;
		
	// It's always safe to zero out stack allocated structs
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));

	// setup the waveformat description to be PCM, 48000hz, 16bit, mono
	wfx.cbSize			= 0;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= numChannels;
	wfx.nSamplesPerSec	= 48000;
	wfx.wBitsPerSample	= 16;
	wfx.nBlockAlign		= 2;
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

	// setup the dsound buffer description with the waveformatex
	dsbd.dwBufferBytes	= (DWORD)((double)wfx.nAvgBytesPerSec / frequency) * wfx.nChannels;
	if(dsbd.dwBufferBytes % 2)
		++dsbd.dwBufferBytes;
	dsbd.dwFlags		= 0;
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.lpwfxFormat	= &wfx;
    dsbd.lpMixBins		= pMixBins;

	// Create the dsound buffer
	hr = g_pDSound->CreateSoundBuffer(&dsbd, pBuffer, NULL);

	// continue forever (or until an error occurs)
	if(SUCCEEDED(hr))
	{
		// Get a buffer from dsound using lock
		hr = (*pBuffer)->Lock(0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0);
		if(SUCCEEDED(hr))
		{
			size	= (dsbd.dwBufferBytes / 2) / numChannels;
			dInc	= (2.0 * cPi) / (double)size;
			for(dF=0.0, index=0, i=0;  i<size; ++i, dF+=dInc)
			{
				value = (SHORT)(sin(dF) * cDataScale);
				for(c=0; c<numChannels; ++c)
					pData[index++] = value;
			}
		}
    }
    
	// Set the headroom to see true levels
	if(SUCCEEDED(hr))
		hr = (*pBuffer)->SetHeadroom(0);

	// start the dsound buffer playing in a loop mode
	if(SUCCEEDED(hr))
		hr = (*pBuffer)->Play(0, 0, DSBPLAY_LOOPING);

    // leave with the buffer still playing..
    return hr;
}

//------------------------------------------------------------------------------
//	UpdateMeters
//------------------------------------------------------------------------------
static void
UpdateMeters(void)
/*++

Routine Description:

	Updates the meter values

Arguments:

	None

Return Value:

	None

--*/
{
	LPCDSFX_RMS_PARAMS	pRmsParams;
	float				fValue;
	int					i;

	// Speakers
	pRmsParams = (LPCDSFX_RMS_PARAMS)g_pEffectsImage->aEffectMaps[Graph0_Speakers].lpvStateSegment; 
	for(i=FRONTLEFT; i<=BACKRIGHT; ++i)
		g_meters[i].SetCurrentLevel((float)(pRmsParams->dwRMSValues[i-FRONTLEFT]/pow(2,23)));

	// I3DL2 & XTalk
	pRmsParams = (LPCDSFX_RMS_PARAMS)g_pEffectsImage->aEffectMaps[Graph0_I3DL2_XTalk].lpvStateSegment; 
	for(i=I3DL2; i<=XTALK_BR; ++i)
		g_meters[i].SetCurrentLevel((float)(pRmsParams->dwRMSValues[i-I3DL2]/pow(2,23)));

	// FXSend 0 - 5
	pRmsParams = (LPCDSFX_RMS_PARAMS)g_pEffectsImage->aEffectMaps[Graph0_FXSend0_To_5].lpvStateSegment; 
	for(i=FXSEND0; i<=FXSEND5; ++i)
		g_meters[i].SetCurrentLevel((float)(pRmsParams->dwRMSValues[i-FXSEND0]/pow(2,23)));
	
	// FXSend 6 - 11
	pRmsParams = (LPCDSFX_RMS_PARAMS)g_pEffectsImage->aEffectMaps[Graph0_FXSend6_To_11].lpvStateSegment; 
	for(i=FXSEND6; i<=FXSEND11; ++i)
		g_meters[i].SetCurrentLevel((float)(pRmsParams->dwRMSValues[i-FXSEND6]/pow(2,23)));
	
	// FXSend 12 - 17
	pRmsParams = (LPCDSFX_RMS_PARAMS)g_pEffectsImage->aEffectMaps[Graph0_FXSend12_To_17].lpvStateSegment; 
	for(i=FXSEND12; i<=FXSEND17; ++i)
		g_meters[i].SetCurrentLevel((float)(pRmsParams->dwRMSValues[i-FXSEND12]/pow(2,23)));
	
	// FXSend 18 - 19
	pRmsParams = (LPCDSFX_RMS_PARAMS)g_pEffectsImage->aEffectMaps[Graph0_FXSend18_To_19].lpvStateSegment; 
	for(i=FXSEND18; i<=FXSEND19; ++i)
		g_meters[i].SetCurrentLevel((float)(pRmsParams->dwRMSValues[i-FXSEND18]/pow(2,23)));
}

//------------------------------------------------------------------------------
//	ToMixbin
//------------------------------------------------------------------------------
static Mixbins 
ToMixbins(
		  IN int	dSoundMixbin
		  )
/*++

Routine Description:

	Converts a DSound mixbin constant to a Mixbin constant

Arguments:

	IN dSoundMixbin -	DSound mixbin constant

Return Value:

	a Mixbin constant

--*/
{
	switch(dSoundMixbin)
	{
	case DSMIXBIN_FRONT_LEFT:
		return FRONTLEFT;
	case DSMIXBIN_FRONT_RIGHT:
		return FRONTRIGHT;
	case DSMIXBIN_FRONT_CENTER:
		return CENTER;
	case DSMIXBIN_LOW_FREQUENCY:
		return LFE;
	case DSMIXBIN_BACK_LEFT:
		return BACKLEFT;
	case DSMIXBIN_BACK_RIGHT:
		return BACKRIGHT;
	case DSMIXBIN_XTLK_FRONT_LEFT:
		return XTALK_FL;
	case DSMIXBIN_XTLK_FRONT_RIGHT:
		return XTALK_FR;
	case DSMIXBIN_XTLK_BACK_LEFT:
		return XTALK_BL;
	case DSMIXBIN_XTLK_BACK_RIGHT:
		return XTALK_BR;
	case DSMIXBIN_I3DL2:
		return I3DL2;
	case DSMIXBIN_FXSEND_0:
		return FXSEND0;
	case DSMIXBIN_FXSEND_1:
		return FXSEND1;
	case DSMIXBIN_FXSEND_2:
		return FXSEND2;
	case DSMIXBIN_FXSEND_3:
		return FXSEND3;
	case DSMIXBIN_FXSEND_4:
		return FXSEND4;
	case DSMIXBIN_FXSEND_5:
		return FXSEND5;
	case DSMIXBIN_FXSEND_6:
		return FXSEND6;
	case DSMIXBIN_FXSEND_7:
		return FXSEND7;
	case DSMIXBIN_FXSEND_8:
		return FXSEND8;
	case DSMIXBIN_FXSEND_9:
		return FXSEND9;
	case DSMIXBIN_FXSEND_10:
		return FXSEND10;
	case DSMIXBIN_FXSEND_11:
		return FXSEND11;
	case DSMIXBIN_FXSEND_12:
		return FXSEND12;
	case DSMIXBIN_FXSEND_13:
		return FXSEND13;
	case DSMIXBIN_FXSEND_14:
		return FXSEND14;
	case DSMIXBIN_FXSEND_15:
		return FXSEND15;
	case DSMIXBIN_FXSEND_16:
		return FXSEND16;
	case DSMIXBIN_FXSEND_17:
		return FXSEND17;
	case DSMIXBIN_FXSEND_18:
		return FXSEND18;
	case DSMIXBIN_FXSEND_19:
		return FXSEND19;
	}
	return NUMMIXBINS;
}

//------------------------------------------------------------------------------
//	ToDSound
//------------------------------------------------------------------------------
static int
ToDSound(
		 IN Mixbins mixbin
		 )
/*++

Routine Description:

	Converts a Mixbin mixbin constant to a DSound constant

Arguments:

	IN mixbin -	Mixbin mixbin constant

Return Value:

	a DSound constant

--*/
{
	switch(mixbin)
	{
	case FRONTLEFT:
		return DSMIXBIN_FRONT_LEFT;
	case FRONTRIGHT:
		return DSMIXBIN_FRONT_RIGHT;
	case CENTER:
		return DSMIXBIN_FRONT_CENTER;
	case LFE:
		return DSMIXBIN_LOW_FREQUENCY;
	case BACKLEFT:
		return DSMIXBIN_BACK_LEFT;
	case BACKRIGHT:
		return DSMIXBIN_BACK_RIGHT;
	case XTALK_FL:
		return DSMIXBIN_XTLK_FRONT_LEFT;
	case XTALK_FR:
		return DSMIXBIN_XTLK_FRONT_RIGHT;
	case XTALK_BL:
		return DSMIXBIN_XTLK_BACK_LEFT;
	case XTALK_BR:
		return DSMIXBIN_XTLK_BACK_RIGHT;
	case I3DL2:
		return DSMIXBIN_I3DL2;
	case FXSEND0:
		return DSMIXBIN_FXSEND_0;
	case FXSEND1:
		return DSMIXBIN_FXSEND_1;
	case FXSEND2:
		return DSMIXBIN_FXSEND_2;
	case FXSEND3:
		return DSMIXBIN_FXSEND_3;
	case FXSEND4:
		return DSMIXBIN_FXSEND_4;
	case FXSEND5:
		return DSMIXBIN_FXSEND_5;
	case FXSEND6:
		return DSMIXBIN_FXSEND_6;
	case FXSEND7:
		return DSMIXBIN_FXSEND_7;
	case FXSEND8:
		return DSMIXBIN_FXSEND_8;
	case FXSEND9:
		return DSMIXBIN_FXSEND_9;
	case FXSEND10:
		return DSMIXBIN_FXSEND_10;
	case FXSEND11:
		return DSMIXBIN_FXSEND_11;
	case FXSEND12:
		return DSMIXBIN_FXSEND_12;
	case FXSEND13:
		return DSMIXBIN_FXSEND_13;
	case FXSEND14:
		return DSMIXBIN_FXSEND_14;
	case FXSEND15:
		return DSMIXBIN_FXSEND_15;
	case FXSEND16:
		return DSMIXBIN_FXSEND_16;
	case FXSEND17:
		return DSMIXBIN_FXSEND_17;
	case FXSEND18:
		return DSMIXBIN_FXSEND_18;
	case FXSEND19:
		return DSMIXBIN_FXSEND_19;
	}
	return DSMIXBIN_LAST+1;
}

//------------------------------------------------------------------------------
//	ToDSound
//------------------------------------------------------------------------------
static const char*
ToText(
	   IN Mixbins mixbin
	   )
/*++

Routine Description:

	Converts a Mixbin mixbin constant to a string

Arguments:

	IN mixbin -	Mixbin mixbin constant

Return Value:

	A string

--*/
{
	switch(mixbin)
	{
	case FRONTLEFT:
		return "DSMIXBIN_FRONT_LEFT";
	case FRONTRIGHT:
		return "DSMIXBIN_FRONT_RIGHT";
	case CENTER:
		return "DSMIXBIN_FRONT_CENTER";
	case LFE:
		return "DSMIXBIN_LOW_FREQUENCY";
	case BACKLEFT:
		return "DSMIXBIN_BACK_LEFT";
	case BACKRIGHT:
		return "DSMIXBIN_BACK_RIGHT";
	case XTALK_FL:
		return "DSMIXBIN_XTLK_FRONT_LEFT";
	case XTALK_FR:
		return "DSMIXBIN_XTLK_FRONT_RIGHT";
	case XTALK_BL:
		return "DSMIXBIN_XTLK_BACK_LEFT";
	case XTALK_BR:
		return "DSMIXBIN_XTLK_BACK_RIGHT";
	case I3DL2:
		return "DSMIXBIN_I3DL2";
	case FXSEND0:
		return "DSMIXBIN_FXSEND_0";
	case FXSEND1:
		return "DSMIXBIN_FXSEND_1";
	case FXSEND2:
		return "DSMIXBIN_FXSEND_2";
	case FXSEND3:
		return "DSMIXBIN_FXSEND_3";
	case FXSEND4:
		return "DSMIXBIN_FXSEND_4";
	case FXSEND5:
		return "DSMIXBIN_FXSEND_5";
	case FXSEND6:
		return "DSMIXBIN_FXSEND_6";
	case FXSEND7:
		return "DSMIXBIN_FXSEND_7";
	case FXSEND8:
		return "DSMIXBIN_FXSEND_8";
	case FXSEND9:
		return "DSMIXBIN_FXSEND_9";
	case FXSEND10:
		return "DSMIXBIN_FXSEND_10";
	case FXSEND11:
		return "DSMIXBIN_FXSEND_11";
	case FXSEND12:
		return "DSMIXBIN_FXSEND_12";
	case FXSEND13:
		return "DSMIXBIN_FXSEND_13";
	case FXSEND14:
		return "DSMIXBIN_FXSEND_14";
	case FXSEND15:
		return "DSMIXBIN_FXSEND_15";
	case FXSEND16:
		return "DSMIXBIN_FXSEND_16";
	case FXSEND17:
		return "DSMIXBIN_FXSEND_17";
	case FXSEND18:
		return "DSMIXBIN_FXSEND_18";
	case FXSEND19:
		return "DSMIXBIN_FXSEND_19";
	}
	return "UNKNOWN";
}

//------------------------------------------------------------------------------
//	ValidateMeters
//------------------------------------------------------------------------------
static void
ValidateMeters(
			   IN int numChannels
			   )
/*++

Routine Description:

	Checks all the meters and reports errors

Arguments:

	IN numChannels - Number of channels

Return Value:

	None

--*/
{
	int	i;

	for(i=0; i<NUMMIXBINS; ++i)
	{
		if(g_meters[i].GetError())
		{
			SETLOG(g_hLog, "robheit", "Mixbins", "ErrorCheck", "ValidateMeters");
		    DbgPrint("%dc: Signal detected on unset mixbin: %s\n", numChannels, ToText((Mixbins)i));
			xLog(g_hLog, XLL_FAIL, "Signal detected on unset mixbin: %s\n", ToText((Mixbins)i));
		}
	}
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest	
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( mixbins )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mixbins )
    EXPORT_TABLE_ENTRY( "StartTest", mixbins_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", mixbins_EndTest )
END_EXPORT_TABLE( mixbins )
