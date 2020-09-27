/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	main.cpp

Abstract:

	DSound Mixbin Test

Author:

	Robert Heitkamp (robheit) 06-Feb-2001

Environment:

	Xbox only

Notes:

	Requires user interaction for verification

Revision History:

	06-Feb-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xgmath.h>
#include <xboxp.h>
#include <macros.h>
#include "bitfont.h"
#include "meter.h"

//------------------------------------------------------------------------------
//	Constants:
//------------------------------------------------------------------------------
enum Mixbin
{
	FRONT_LEFT,
	FRONT_RIGHT,
	CENTER,
	LFE,
	BACK_LEFT,
	BACK_RIGHT
};
	
enum MeterIndex
{
	AnalogLeftTotalPeak,
	AnalogRightTotalPeak,
	AnalogLeftTotalRMS,
	AnalogRightTotalRMS,
	DigitalFrontLeftPeak,
	DigitalFrontCenterPeak,
	DigitalFrontRightPeak,
	DigitalBackLeftPeak,
	DigitalBackRightPeak,
	DigitalLowFrequencyPeak,
	DigitalFrontLeftRMS,
	DigitalFrontCenterRMS,
	DigitalFrontRightRMS,
	DigitalBackLeftRMS,
	DigitalBackRightRMS,
	DigitalLowFrequencyRMS,
	NumMeters
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
static CMixbinMeter			g_meters[16];
static DSMIXBINVOLUMEPAIR	g_mixBinVolumePair;
static DSMIXBINS			g_mixBins;
static double				g_frequency;
static IDirect3D8*		    g_d3d			= NULL;
static IDirect3DDevice8*	g_pDevice		= NULL;
static LPDIRECTSOUND8		g_pDSound		= NULL;
static IDirect3DSurface8*   g_pBackBuffer	= NULL;
static LPDSEFFECTIMAGEDESC	g_pEffectsImage	= NULL;
static BitFont				g_font;
static BOOL					g_bReset		= TRUE;
static BOOL					g_bKeepRunning	= TRUE;

//------------------------------------------------------------------------------
//	Static Functions
//------------------------------------------------------------------------------
static HRESULT CreateSineWave(LPDIRECTSOUNDBUFFER*, DSMIXBINS*, double, WORD);
static HRESULT InitGraphics(void);
static void UpdateMeters(void);
static void ValidateMeters(void);
static const char* MeterIndexToText(MeterIndex);
static float ToDB(DWORD	dwVal);

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
//	outputlevels_BasicTest
//------------------------------------------------------------------------------
HRESULT
outputlevels_BasicTest(void)
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
	int						i;
	float					y;
	float					x;
	float					dx;
	LONGLONG				frequency;
	LONGLONG				current;
	LONGLONG				last;
	int						mixbin;
	BOOL					bNoSignal[NumMeters];
	int						c;
	int						numChannels;
	int						loop;
	DSBUFFERDESC			dsbd;
	WAVEFORMATEX			wfx;
	int						play;
	DWORD					dwStatus;
	int						l;
    HRESULT					hr			= S_OK;
	float					lineWidth	= 1.0f;
	BOOL					bSubmix		= FALSE;
	LPDIRECTSOUNDBUFFER		pBuffer[6]	= { NULL, NULL, NULL, NULL, NULL, NULL };
	float					freq[6]		= { 110.0f, 220.0f, 440.0f, 55.0f, 880.0f, 1760.0f };
	BOOL					bPlay[6]	= { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

	// Query the performance frequency
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	g_frequency = (double) frequency;

	// Set the seed
	QueryPerformanceCounter((LARGE_INTEGER*)&last);
	srand((unsigned long)last);
	
	// Init the graphics
	InitGraphics();

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
	for(i=0; i<NumMeters; ++i)
	{
		g_meters[i].SetHeight(170.0f);
		g_meters[i].SetWidth(45.0f);
	}

	// Speakers
	dx	= 60.0f;
	x	= 65.0f;
	y	= 55.0f;
	g_meters[AnalogLeftTotalPeak].SetLocation(x, y); x += dx;
	g_meters[AnalogLeftTotalPeak].SetLabel(L"Left");
	g_meters[AnalogLeftTotalPeak].SetLabelColor(0xffff0000);
	g_meters[AnalogLeftTotalPeak].SetLevel(1.0f);
	g_meters[AnalogRightTotalPeak].SetLocation(x, y); x += dx;
	g_meters[AnalogRightTotalPeak].SetLabel(L"Right");
	g_meters[AnalogRightTotalPeak].SetLabelColor(0xffff0000);
	g_meters[AnalogRightTotalPeak].SetLevel(1.0f);
	x += dx-10;
	g_meters[DigitalFrontLeftPeak].SetLocation(x, y); x += dx;
	g_meters[DigitalFrontLeftPeak].SetLabel(L"Front Left");
	g_meters[DigitalFrontLeftPeak].SetLabelColor(0xff0000ff);
	g_meters[DigitalFrontLeftPeak].SetLevel(1.0f);
	g_meters[DigitalFrontCenterPeak].SetLocation(x, y); x += dx;
	g_meters[DigitalFrontCenterPeak].SetLabel(L"Center");
	g_meters[DigitalFrontCenterPeak].SetLabelColor(0xff0000ff);
	g_meters[DigitalFrontCenterPeak].SetLevel(1.0f);
	g_meters[DigitalFrontRightPeak].SetLocation(x, y); x += dx;
	g_meters[DigitalFrontRightPeak].SetLabel(L"Front Right");
	g_meters[DigitalFrontRightPeak].SetLabelColor(0xff0000ff);
	g_meters[DigitalFrontRightPeak].SetLevel(1.0f);
	g_meters[DigitalBackLeftPeak].SetLocation(x, y); x += dx;
	g_meters[DigitalBackLeftPeak].SetLabel(L"Back Left");
	g_meters[DigitalBackLeftPeak].SetLabelColor(0xff0000ff);
	g_meters[DigitalBackLeftPeak].SetLevel(1.0f);
	g_meters[DigitalBackRightPeak].SetLocation(x, y); x += dx;
	g_meters[DigitalBackRightPeak].SetLabel(L"Back Right");
	g_meters[DigitalBackRightPeak].SetLabelColor(0xff0000ff);
	g_meters[DigitalBackRightPeak].SetLevel(1.0f);
	g_meters[DigitalLowFrequencyPeak].SetLocation(x, y); x += dx;
	g_meters[DigitalLowFrequencyPeak].SetLabel(L"LFE");
	g_meters[DigitalLowFrequencyPeak].SetLabelColor(0xff0000ff);
	g_meters[DigitalLowFrequencyPeak].SetLevel(1.0f);

	x	= 65.0f;
	y	= 255.0f;
	g_meters[AnalogLeftTotalRMS].SetLocation(x, y); x += dx;
	g_meters[AnalogLeftTotalRMS].SetLabel(L"Left");
	g_meters[AnalogLeftTotalRMS].SetLabelColor(0xffff0000);
	g_meters[AnalogLeftTotalRMS].SetLevel(1.0f);
	g_meters[AnalogRightTotalRMS].SetLocation(x, y); x += dx;
	g_meters[AnalogRightTotalRMS].SetLabel(L"Right");
	g_meters[AnalogRightTotalRMS].SetLabelColor(0xffff0000);
	g_meters[AnalogRightTotalRMS].SetLevel(1.0f);
	x += dx-10;
	g_meters[DigitalFrontLeftRMS].SetLocation(x, y); x += dx;
	g_meters[DigitalFrontLeftRMS].SetLabel(L"Front Left");
	g_meters[DigitalFrontLeftRMS].SetLabelColor(0xff0000ff);
	g_meters[DigitalFrontLeftRMS].SetLevel(1.0f);
	g_meters[DigitalFrontCenterRMS].SetLocation(x, y); x += dx;
	g_meters[DigitalFrontCenterRMS].SetLabel(L"Center");
	g_meters[DigitalFrontCenterRMS].SetLabelColor(0xff0000ff);
	g_meters[DigitalFrontCenterRMS].SetLevel(1.0f);
	g_meters[DigitalFrontRightRMS].SetLocation(x, y); x += dx;
	g_meters[DigitalFrontRightRMS].SetLabel(L"Front Right");
	g_meters[DigitalFrontRightRMS].SetLabelColor(0xff0000ff);
	g_meters[DigitalFrontRightRMS].SetLevel(1.0f);
	g_meters[DigitalBackLeftRMS].SetLocation(x, y); x += dx;
	g_meters[DigitalBackLeftRMS].SetLabel(L"Back Left");
	g_meters[DigitalBackLeftRMS].SetLabelColor(0xff0000ff);
	g_meters[DigitalBackLeftRMS].SetLevel(1.0f);
	g_meters[DigitalBackRightRMS].SetLocation(x, y); x += dx;
	g_meters[DigitalBackRightRMS].SetLabel(L"Back Right");
	g_meters[DigitalBackRightRMS].SetLabelColor(0xff0000ff);
	g_meters[DigitalBackRightRMS].SetLevel(1.0f);
	g_meters[DigitalLowFrequencyRMS].SetLocation(x, y); x += dx;
	g_meters[DigitalLowFrequencyRMS].SetLabel(L"LFE");
	g_meters[DigitalLowFrequencyRMS].SetLabelColor(0xff0000ff);
	g_meters[DigitalLowFrequencyRMS].SetLevel(1.0f);

	// Setup mixbins structure for no output to mixbins
	g_mixBins.lpMixBinVolumePairs	= &g_mixBinVolumePair;
	g_mixBins.dwMixBinCount			= 1;
	g_mixBinVolumePair.lVolume		= 0;

	while(g_bKeepRunning)
	{
		// Setup the speaker config
#if 0
		DSSPEAKER_ENABLE_AC3
		DSSPEAKER_ENABLE_DTS
		DSSPEAKER_MONO
		DSSPEAKER_STEREO
		DSSPEAKER_MONO
		XSetValue(XC_AUDIO_FLAGS, REG_DWORD, dwFlags, 4);
#endif
		// Create the DSound object
		hr = DirectSoundCreate(NULL, &g_pDSound, NULL);
		if(FAILED(hr))
			return hr;

		// Create the sine waves
		g_mixBinVolumePair.dwMixBin	= DSMIXBIN_FRONT_LEFT;
		hr = CreateSineWave(&pBuffer[FRONT_LEFT], &g_mixBins, freq[FRONT_LEFT], 1);
		if(SUCCEEDED(hr))
		{
			g_mixBinVolumePair.dwMixBin	= DSMIXBIN_FRONT_RIGHT;
			hr = CreateSineWave(&pBuffer[FRONT_RIGHT], &g_mixBins, freq[FRONT_RIGHT], 1);
			if(SUCCEEDED(hr))
			{
				g_mixBinVolumePair.dwMixBin	= DSMIXBIN_FRONT_CENTER;
				hr = CreateSineWave(&pBuffer[CENTER], &g_mixBins, freq[CENTER], 1);
				if(SUCCEEDED(hr))
				{
					g_mixBinVolumePair.dwMixBin	= DSMIXBIN_LOW_FREQUENCY;
					hr = CreateSineWave(&pBuffer[LFE], &g_mixBins, freq[LFE], 1);
					if(SUCCEEDED(hr))
					{
						g_mixBinVolumePair.dwMixBin	= DSMIXBIN_BACK_LEFT;
						hr = CreateSineWave(&pBuffer[BACK_LEFT], &g_mixBins, freq[BACK_LEFT], 1);
						if(SUCCEEDED(hr))
						{
							g_mixBinVolumePair.dwMixBin	= DSMIXBIN_BACK_RIGHT;
							hr = CreateSineWave(&pBuffer[BACK_RIGHT], &g_mixBins, freq[BACK_RIGHT], 1);
						}
					}
				}
			}
		}

		if(FAILED(hr))
		{
			for(i=0; i<6; ++i)
			{
				if(pBuffer[i])
					pBuffer[i]->Release();
			}
			g_pDSound->Release();
			return hr;
		}

		for(i=0; i<NumMeters; ++i)
			bNoSignal[i] = FALSE;

		for(l=0; l<100; )
		{
			UpdateMeters();

			// Are all active meters receiving a signal?
			for(i=0; i<NumMeters; ++i)
			{
				if(g_meters[i].IsActive() && (g_meters[i].GetLevel() > 0.0f) && 
				   (g_meters[i].GetCurrentLevel() == 0.0f))
				{
					bNoSignal[i] = TRUE;
				}
			}

			// Draw the scene
			g_pDevice->Clear(0, NULL, 
							 D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
							 0xff000000, 1.0f, 0);

			// Draw the meters
			for(i=0; i<NumMeters; ++i)
				g_meters[i].Draw(g_pDevice, g_pBackBuffer);

			// Draw the labels
			g_font.DrawText(g_pBackBuffer, L"Peak",    300,  40, 0, 0xffffffff, 0);
			g_font.DrawText(g_pBackBuffer, L"RMS",     310, 240, 0, 0xffffffff, 0);
			g_font.DrawText(g_pBackBuffer, L"Analog",  100, 225, 0, 0xffff0000, 0);
			g_font.DrawText(g_pBackBuffer, L"Analog",  100, 425, 0, 0xffff0000, 0);
			g_font.DrawText(g_pBackBuffer, L"Digital", 390, 225, 0, 0xff0000ff, 0);
			g_font.DrawText(g_pBackBuffer, L"Digital", 390, 425, 0, 0xff0000ff, 0);

			g_pDevice->EndScene();
			g_pDevice->Present(NULL, NULL, NULL, NULL);

			QueryPerformanceCounter((LARGE_INTEGER*)&current);
			
			DirectSoundDoWork();

			// Change playing mixbins?
			if(((double)(current - last) / g_frequency) > 2.0)
			{
				++l;

				// Check for errors
				ValidateMeters();
					
				// Display any no signal errors
				for(i=0; i<NumMeters; ++i)
				{
					if(bNoSignal[i])
					{
						SETLOG(g_hLog, "robheit", "OutputLevels", "ActiveCheck", "outputlevels_BasicTest");
						DbgPrint("No signal detected on active mixbin: %s\n", MeterIndexToText((MeterIndex)i));
						xLog(g_hLog, XLL_FAIL, "No signal detected on active mixbin: %s\n", MeterIndexToText((MeterIndex)i));
					}
					bNoSignal[i] = FALSE;
				}

				// Reset the meters
				for(i=0; i<NumMeters; ++i)
				{
					g_meters[i].SetCurrentLevel(0.0f);
					g_meters[i].SetActive(FALSE);
					g_meters[i].ResetError();
					g_meters[i].ResetPeak();
				}
				
				// Stop all the buffers
				for(i=0; i<6; ++i)
				{
					pBuffer[i]->Stop();
					DirectSoundDoWork();
				}

				do
				{
					for(play=0, i=0; (play == 0) && (i < 6); ++i)
					{
						pBuffer[i]->GetStatus(&dwStatus);
						if(dwStatus != 0)
						{
							DirectSoundDoWork();
							++play;
						}
					}
				}
				while(play);

				g_bReset = TRUE;

				for(i=0; i<6; ++i)
					bPlay[i] = FALSE;

				// Play the buffers
				play = rand() % 7;
				while(play)
				{
					i = rand() % 6;
					if(!bPlay[i])			
					{
						bPlay[i] = TRUE;
						--play;
					}
				}

				// Set the state of the meters
				g_meters[AnalogLeftTotalPeak].SetActive(bPlay[FRONT_LEFT] || bPlay[CENTER] || bPlay[BACK_LEFT] || bPlay[BACK_RIGHT]);
				g_meters[AnalogRightTotalPeak].SetActive(bPlay[FRONT_RIGHT]|| bPlay[CENTER] || bPlay[BACK_LEFT] || bPlay[BACK_RIGHT]);
				g_meters[AnalogLeftTotalRMS].SetActive(bPlay[FRONT_LEFT] || bPlay[CENTER] || bPlay[BACK_LEFT] || bPlay[BACK_RIGHT]);
				g_meters[AnalogRightTotalRMS].SetActive(bPlay[FRONT_RIGHT] || bPlay[CENTER] || bPlay[BACK_LEFT] || bPlay[BACK_RIGHT]);
				g_meters[DigitalFrontLeftPeak].SetActive(bPlay[FRONT_LEFT]);
				g_meters[DigitalFrontCenterPeak].SetActive(bPlay[CENTER]);
				g_meters[DigitalFrontRightPeak].SetActive(bPlay[FRONT_RIGHT]);
				g_meters[DigitalBackLeftPeak].SetActive(bPlay[BACK_LEFT]);
				g_meters[DigitalBackRightPeak].SetActive(bPlay[BACK_RIGHT]);
				g_meters[DigitalLowFrequencyPeak].SetActive(bPlay[LFE]);
				g_meters[DigitalFrontLeftRMS].SetActive(bPlay[FRONT_LEFT]);
				g_meters[DigitalFrontCenterRMS].SetActive(bPlay[CENTER]);
				g_meters[DigitalFrontRightRMS].SetActive(bPlay[FRONT_RIGHT]);
				g_meters[DigitalBackLeftRMS].SetActive(bPlay[BACK_LEFT]);
				g_meters[DigitalBackRightRMS].SetActive(bPlay[BACK_RIGHT]);
				g_meters[DigitalLowFrequencyRMS].SetActive(bPlay[LFE]);

				for(i=0; i<6; ++i)
				{
					if(bPlay[i])
						pBuffer[i]->Play(0, 0, DSBPLAY_LOOPING);
				}

				// Wait for the last data to be flushed
				DirectSoundDoWork();
				Sleep(10);
				QueryPerformanceCounter((LARGE_INTEGER*)&last);
			}
		}

		// Release the buffers
		for(c=0; c<6; ++c)
			pBuffer[c]->Release();

		// Release DSound
		g_pDSound->Release();
	}

    return S_OK;
}

//------------------------------------------------------------------------------
//	outputlevels_StartTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
outputlevels_StartTest( 
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
	DWORD	type;
	DWORD	value;
    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	// Read the initial values
//	XQueryValue(XC_AUDIO_FLAGS, &type, &value, 4, NULL);

	outputlevels_BasicTest();

	// Reset the values
//	XSetValue(XC_AUDIO_FLAGS, REG_DWORD, &value, 4);
}

//------------------------------------------------------------------------------
//	outputlevels_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
outputlevels_EndTest(VOID)
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
    outputlevels_StartTest(NULL);
    outputlevels_EndTest();

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

	// Get the back buffer
    g_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer);

	return hr;
}

//------------------------------------------------------------------------------
//	::CreateSineWave
//------------------------------------------------------------------------------
static HRESULT 
CreateSineWave(
			   OUT LPDIRECTSOUNDBUFFER*	pBuffer,		// Output buffer
			   IN DSMIXBINS*			pMixBins,		// Mixbins to output to
			   IN double				frequency,		// Hertz
			   IN WORD					numChannels		// Number of channels
			   )
/*++

Routine Description:

	Creates a sine wave

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
	DSOUTPUTLEVELS	ol;
	static int		count = 0;

	// Only update every 3 frames (about 20 times a second)
	if(++count == 3)
		count = 0;
	else
		return;

	if(SUCCEEDED(g_pDSound->GetOutputLevels(&ol, g_bReset)))
	{
		g_meters[AnalogLeftTotalPeak].SetCurrentLevel(ToDB(ol.dwAnalogLeftTotalPeak));
		g_meters[AnalogRightTotalPeak].SetCurrentLevel(ToDB(ol.dwAnalogRightTotalPeak));
		g_meters[DigitalFrontLeftPeak].SetCurrentLevel(ToDB(ol.dwDigitalFrontLeftPeak));
		g_meters[DigitalFrontCenterPeak].SetCurrentLevel(ToDB(ol.dwDigitalFrontCenterPeak));
		g_meters[DigitalFrontRightPeak].SetCurrentLevel(ToDB(ol.dwDigitalFrontRightPeak));
		g_meters[DigitalBackLeftPeak].SetCurrentLevel(ToDB(ol.dwDigitalBackLeftPeak));
		g_meters[DigitalBackRightPeak].SetCurrentLevel(ToDB(ol.dwDigitalBackRightPeak));
		g_meters[DigitalLowFrequencyPeak].SetCurrentLevel(ToDB(ol.dwDigitalLowFrequencyPeak));

		g_meters[AnalogLeftTotalRMS].SetCurrentLevel(ToDB(ol.dwAnalogLeftTotalRMS));
		g_meters[AnalogRightTotalRMS].SetCurrentLevel(ToDB(ol.dwAnalogRightTotalRMS));
		g_meters[DigitalFrontLeftRMS].SetCurrentLevel(ToDB(ol.dwDigitalFrontLeftRMS));
		g_meters[DigitalFrontCenterRMS].SetCurrentLevel(ToDB(ol.dwDigitalFrontCenterRMS));
		g_meters[DigitalFrontRightRMS].SetCurrentLevel(ToDB(ol.dwDigitalFrontRightRMS));
		g_meters[DigitalBackLeftRMS].SetCurrentLevel(ToDB(ol.dwDigitalBackLeftRMS));
		g_meters[DigitalBackRightRMS].SetCurrentLevel(ToDB(ol.dwDigitalBackRightRMS));
		g_meters[DigitalLowFrequencyRMS].SetCurrentLevel(ToDB(ol.dwDigitalLowFrequencyRMS));
	}
	else
	{
		g_meters[AnalogLeftTotalPeak].SetCurrentLevel(0.0f);
		g_meters[AnalogRightTotalPeak].SetCurrentLevel(0.0f);
		g_meters[AnalogLeftTotalRMS].SetCurrentLevel(0.0f);
		g_meters[AnalogRightTotalRMS].SetCurrentLevel(0.0f);
		g_meters[DigitalFrontLeftPeak].SetCurrentLevel(0.0f);
		g_meters[DigitalFrontCenterPeak].SetCurrentLevel(0.0f);
		g_meters[DigitalFrontRightPeak].SetCurrentLevel(0.0f);
		g_meters[DigitalBackLeftPeak].SetCurrentLevel(0.0f);
		g_meters[DigitalBackRightPeak].SetCurrentLevel(0.0f);
		g_meters[DigitalLowFrequencyPeak].SetCurrentLevel(0.0f);
		g_meters[DigitalFrontLeftRMS].SetCurrentLevel(0.0f);
		g_meters[DigitalFrontCenterRMS].SetCurrentLevel(0.0f);
		g_meters[DigitalFrontRightRMS].SetCurrentLevel(0.0f);
		g_meters[DigitalBackLeftRMS].SetCurrentLevel(0.0f);
		g_meters[DigitalBackRightRMS].SetCurrentLevel(0.0f);
		g_meters[DigitalLowFrequencyRMS].SetCurrentLevel(0.0f);
	}
}

//------------------------------------------------------------------------------
//	MeterIndexToText
//------------------------------------------------------------------------------
static const char*
MeterIndexToText(
				 IN MeterIndex mi
				 )
/*++

Routine Description:

	Converts a MeterIndex to a string

Arguments:

	IN mi -	Meter index

Return Value:

	A string

--*/
{
	switch(mi)
	{
	case AnalogLeftTotalPeak:
		return "AnalogLeftTotalPeak";
	case AnalogRightTotalPeak:
		return "AnalogRightTotalPeak";
	case AnalogLeftTotalRMS:
		return "AnalogLeftTotalRMS";
	case AnalogRightTotalRMS:
		return "AnalogRightTotalRMS";
	case DigitalFrontLeftPeak:
		return "DigitalFrontLeftPeak";
	case DigitalFrontCenterPeak:
		return "DigitalFrontCenterPeak";
	case DigitalFrontRightPeak:
		return "DigitalFrontRightPeak";
	case DigitalBackLeftPeak:
		return "DigitalBackLeftPeak";
	case DigitalBackRightPeak:
		return "DigitalBackRightPeak";
	case DigitalLowFrequencyPeak:
		return "DigitalLowFrequencyPeak";
	case DigitalFrontLeftRMS:
		return "DigitalFrontLeftRMS";
	case DigitalFrontCenterRMS:
		return "DigitalFrontCenterRMS";
	case DigitalFrontRightRMS:
		return "DigitalFrontRightRMS";
	case DigitalBackLeftRMS:
		return "DigitalBackLeftRMS";
	case DigitalBackRightRMS:
		return "DigitalBackRightRMS";
	case DigitalLowFrequencyRMS:
		return "DigitalLowFrequencyRMS";
	}
	return "Unknown";
}

//------------------------------------------------------------------------------
//	ValidateMeters
//------------------------------------------------------------------------------
static void
ValidateMeters(void)
/*++

Routine Description:

	Checks all the meters and reports errors

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	for(i=0; i<NumMeters; ++i)
	{
		if(g_meters[i].GetError())
		{
			SETLOG(g_hLog, "robheit", "OutputLevels", "ErrorCheck", "ValidateMeters");
		    DbgPrint("Signal detected on inactive mixbin: %s\n", MeterIndexToText((MeterIndex)i));
			xLog(g_hLog, XLL_FAIL, "Signal detected on inactive mixbin: %s\n", MeterIndexToText((MeterIndex)i));
		}
	}
}

//------------------------------------------------------------------------------
//	ToDB
//------------------------------------------------------------------------------
static float
ToDB(
 	 IN DWORD	dwVal
	 )
/*++

Routine Description:

	Converts an audio level 0 - 0x7fffff to a dB 0.0 - 1.0
	0 dB	= 1.0
	-100dB	= 0.0

Arguments:

	IN dwVal -	Value

Return Value:

	dB Value (inverted and scaled 0.0-1.0)

--*/
{
	float fVal;

	fVal = (float)((100.0 - (-20.0 * log10((double)dwVal / pow(2,23)))) / 100.0);
	if(fVal < 0.00001f)
		return 0.0f;
	return fVal;
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest	
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( outputlevels)
#pragma data_seg()

BEGIN_EXPORT_TABLE( outputlevels )
    EXPORT_TABLE_ENTRY( "StartTest", outputlevels_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", outputlevels_EndTest )
END_EXPORT_TABLE( outputlevels )
