/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	media.cpp

Abstract:

	Media stress - D3D, DSound, DMusic and others.

Author:

	Robert Heitkamp (robheit) 27-Apr-2001

Environment:

	Xbox only

Revision History:

	27-Apr-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <time.h>
#include "media.h"
#include "helpers.h"
#include "scene.h"
#include "input.h"
#include <d3d8perf.h>

#ifndef NOLOGGING
#include <makeini.h>
#include <stressmon.h>
#endif // NOLOGGING

#include "medialoader.h"
#define MEDIA_FILES_INI_PATH	"D:\\MediaFiles.ini"

using namespace Media4;


extern HANDLE  g_hLog;

//------------------------------------------------------------------------------
//	Static Function Declarations:
//------------------------------------------------------------------------------
static HRESULT Initialize(void);
static HRESULT InitializeD3D(void);
static void Cleanup(void);
static HRESULT InitializeScene(void);

//------------------------------------------------------------------------------
//	Static Global Variables
//------------------------------------------------------------------------------
IDirect3D8*		    g_d3d			= NULL;
IDirect3DDevice8*	g_d3dDevice		= NULL;
BOOL                g_notDone		= TRUE;
CScene				g_scene;

//------------------------------------------------------------------------------
//	media4_BasicTest
//------------------------------------------------------------------------------
HRESULT
media4_BasicTest(void)
/*++

Routine Description:

    Basic Test for media

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT hr	= S_OK;

	// General initialization
	hr = Initialize();
	if(hr != S_OK)
	{
		__asm int 3;
		return hr;
	}

	// Enable profiling?
#ifdef _DEBUG
	if(g_TestParams.bD3DPerf)
		D3DPERF_StartPerfProfile();
#endif 

	// Loop until done
	while(g_notDone)
	{
		// Draw the scene
		g_scene.Render(g_d3dDevice);

		// Next frame
		g_scene.NextFrame();

        // input
        CheckUserInput();
	}

	// Disable profiling?
#ifdef _DEBUG
	if(g_TestParams.bD3DPerf)
		D3DPERF_StopPerfProfile();
#endif

    // Return the Hresult
    return hr;
}

//
// this function should only be linked in / defined if this is compiled as a lib
//

#ifndef NOLOGGING
DWORD WINAPI StartStressMon( LPVOID lpParameter )
{
	StressMONStartTest( (HANDLE) lpParameter );
	return 0;
}
#endif // NOLOGGING

//------------------------------------------------------------------------------
//	
//------------------------------------------------------------------------------
VOID
WINAPI 
media4_StartTest(
				IN HANDLE LogHandle 
				)
/*++

Routine Description:

    The harness entry into the media tests

Arguments:

    IN LogHandle - a handle to a logging object

Return Value:

    None

--*/
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	//
	// decide if this is a lib or an exe, and generate an ini file if it is a lib
	//

#ifndef NOLOGGING

	EXECUTE( makeini_BasicTest() );
	if ( SUCCEEDED( hr ) )
	{
		CreateThread( NULL, 65536, StartStressMon, (LPVOID) LogHandle, 0, NULL );
	}

#endif // NOLOGGING

    EXECUTE( media4_BasicTest() );
}

//------------------------------------------------------------------------------
//	media4_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
media4_EndTest(VOID)
/*++

Routine Description:

    The exit function for the test harness

Arguments:

    None

Return Value:

    None

--*/
{
	Cleanup();
}

//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
#ifdef NOLOGGING
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
    // Initialize USB Devices
    XInitDevices(0, NULL);
    InputDukeInsertions(XGetDevices(XDEVICE_TYPE_GAMEPAD));

    media4_StartTest(NULL);
    media4_EndTest();
}
#endif // NOLOGGING

//------------------------------------------------------------------------------
//	InitializeD3D
//------------------------------------------------------------------------------
static HRESULT
InitializeD3D(void)
/*++

Routine Description:

	Initializes the d3d device

Arguments:

	None

Return Value:

	S_OK on success, any other value on failure

--*/
{
	HRESULT					hr;
	D3DPRESENT_PARAMETERS	d3dpp;
	
	// Create an instance of a Direct3D8 object 
	g_d3d = Direct3DCreate8(D3D_SDK_VERSION);
	if(g_d3d == NULL)
		return E_FAIL;
	
	// Setup the present parameters
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth					= 640;
	d3dpp.BackBufferHeight					= 480;
	d3dpp.BackBufferFormat					= D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount					= g_TestParams.dwBackBufferCount;
	d3dpp.Flags								= 0;
//	d3dpp.MultiSampleType					= D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN;
	d3dpp.MultiSampleType					= g_TestParams.dwMultiSampleType;
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
						     &g_d3dDevice);
	if(hr != D3D_OK)
		return hr;
	
	// Enable antialiasing
//	g_d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE); 

	return S_OK;
}


//------------------------------------------------------------------------------
//	InitializeDSound
//------------------------------------------------------------------------------
static HRESULT
InitializeDSound(void)
/*++
Routine Description:
	Downloads the scratch image to the DSP
Return Value:
	S_OK on success, any other value on failure
*/
{
    return S_OK;
}


//------------------------------------------------------------------------------
//	InitializeMedia
//------------------------------------------------------------------------------
HRESULT InitializeMedia(void)
{
    HRESULT hr = S_OK;

    //If we're instructed not to copy from the network (if we're running from a DVD, for example)
    //  then we'll skip this part.
    if (!g_TestParams.bCopyMediaFromNetwork)
        return S_OK;
	
    #define CREATE_FLAG COPY_IF_NEWER
    SETLOG( g_hLog, "robheit", "MCPX", "media", "Basic" );
	MLSetErrorLevel( 3 );
	MEDIAFILEDESC MediaFiles[] =
	{
		{"mediastress/graphics/", "T:\\media\\graphics\\", CREATE_FLAG},
		{"DSP/", "T:\\media\\DSPCode", CREATE_FLAG},
		{NULL, NULL, 0}
	};
	//EXECUTE( LoadMediaFromIni( MEDIA_FILES_INI_PATH ) );
	EXECUTE( LoadMedia( MediaFiles ) );
	if ( FAILED( hr ) )
	{
		//Log("LoadMedia failed!!!\n");
        //__asm int 3;
        hr = E_FAIL;
	}

    return hr;

}


//------------------------------------------------------------------------------
//	Initialize
//------------------------------------------------------------------------------
static HRESULT
Initialize(void)
/*++

Routine Description:

	General Intialization

Arguments:

	None

Return Value:

	S_OK on success, any other value on failure

--*/
{
	HRESULT			hr;
	LONGLONG		timer;
	unsigned int	seed;

	// Set the seed to randomize the runs
	// A variable is used so that the seed can be overrided at run time
	QueryPerformanceCounter((LARGE_INTEGER*)&timer);
	seed = (unsigned int)timer;
	srand(seed);

    // Load user-defined parameters.    
    hr = LoadTestParams();
    if (FAILED(hr))
        return hr;

    // Check the parameters are in order..    
    hr = SanityCheckTestParams();
    if (FAILED(hr))
        return hr;

    // Copy down the media, if required.
    hr = InitializeMedia();
    //if (FAILED(hr))
    //    return hr;

    // Check that the files are present and accessible.
    hr = SanityCheckFiles();
    if (FAILED(hr))
        return hr;

	// Initialize D3D
	hr = InitializeD3D();
	if(hr != S_OK)
		return hr;

	// Create the graphic data
	hr = InitializeScene();
	if(hr != S_OK)
		return hr;

	return S_OK;
}

//------------------------------------------------------------------------------
//	Cleanup
//------------------------------------------------------------------------------
static void
Cleanup(void)
/*++

Routine Description:

	Free resources

Arguments:

	None

Return Value:

	None

--*/
{
	
    // IDirect3DDevice8
	if(g_d3dDevice)
	{
		g_d3dDevice->Release();
		g_d3dDevice = NULL;
	}

	// IDirect3D8
	if(g_d3d)
	{
		g_d3d->Release();
		g_d3d = NULL;
	}

    FreeTestParams();
}

//------------------------------------------------------------------------------
//	InitializeScene
//------------------------------------------------------------------------------
static HRESULT
InitializeScene(void)
/*++

Routine Description:

	Initializes all data for the scene

Arguments:

	None

Return Value:

	S_OK on success, any other value on failure

--*/
{
	HRESULT	hr;
	
	hr = g_scene.Initialize(g_d3dDevice);
	return hr;
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( media4 )
#pragma data_seg()

BEGIN_EXPORT_TABLE( media4 )
    EXPORT_TABLE_ENTRY( "StartTest", media4_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", media4_EndTest )
END_EXPORT_TABLE( media4 )
