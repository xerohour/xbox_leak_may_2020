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
#include "music.h"
#include "input.h"
#include <d3d8perf.h>
#include "medialoader.h"
#include "dsstress.h"


#ifndef NOLOGGING
#include <makeini.h>
#include <stressmon.h>
#endif // NOLOGGING

using namespace WFVO;

extern HANDLE  g_hLog;

namespace WFVO {

#define MEDIA_FILES_INI_PATH	"D:\\MediaFiles.ini"

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
CMusic              g_music;
IDirect3D8*		    g_d3d			= NULL;
IDirect3DDevice8*	g_d3dDevice		= NULL;
IDirectSound *      g_dsound        = NULL;
BOOL                g_notDone		= TRUE;
CScene				g_scene;

//------------------------------------------------------------------------------
//	media_BasicTest
//------------------------------------------------------------------------------
HRESULT
media_BasicTest(void)
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

        // Update Music
		g_music.Update();

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
media_StartTest(
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

	EXECUTE( makeini_WFVOTest() );
	if ( SUCCEEDED( hr ) )
	{
		CreateThread( NULL, 65536, StartStressMon, (LPVOID) LogHandle, 0, NULL );
	}

#endif // NOLOGGING

    EXECUTE( media_BasicTest() );
}

//------------------------------------------------------------------------------
//	media_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
media_EndTest(VOID)
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
    static const struct
    {
        LPCSTR                      pszName;
        DSI3DL2LISTENER             Properties;
    } Environments[] =
    {
        { "Default",         { DSI3DL2_ENVIRONMENT_PRESET_DEFAULT } },
        { "Generic",         { DSI3DL2_ENVIRONMENT_PRESET_GENERIC } },
        { "PaddedCell",      { DSI3DL2_ENVIRONMENT_PRESET_PADDEDCELL } },
        { "Room",            { DSI3DL2_ENVIRONMENT_PRESET_ROOM } },
        { "Bathroom",        { DSI3DL2_ENVIRONMENT_PRESET_BATHROOM } },
        { "LivingRoom",      { DSI3DL2_ENVIRONMENT_PRESET_LIVINGROOM } },
        { "StoneRoom",       { DSI3DL2_ENVIRONMENT_PRESET_STONEROOM } },
        { "Auditorium",      { DSI3DL2_ENVIRONMENT_PRESET_AUDITORIUM } },
        { "ConcertHall",     { DSI3DL2_ENVIRONMENT_PRESET_CONCERTHALL } },
        { "Cave",            { DSI3DL2_ENVIRONMENT_PRESET_CAVE } },
        { "Arena",           { DSI3DL2_ENVIRONMENT_PRESET_ARENA } },
        { "Hangar",          { DSI3DL2_ENVIRONMENT_PRESET_HANGAR } },
        { "CarpetedHallway", { DSI3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY } },
        { "Hallway",         { DSI3DL2_ENVIRONMENT_PRESET_HALLWAY } },
        { "StoneCorridor",   { DSI3DL2_ENVIRONMENT_PRESET_STONECORRIDOR } },
        { "Alley",           { DSI3DL2_ENVIRONMENT_PRESET_ALLEY } },
        { "Forest",          { DSI3DL2_ENVIRONMENT_PRESET_FOREST } },
        { "City",            { DSI3DL2_ENVIRONMENT_PRESET_CITY } },
        { "Mountains",       { DSI3DL2_ENVIRONMENT_PRESET_MOUNTAINS } },
        { "Quarry",          { DSI3DL2_ENVIRONMENT_PRESET_QUARRY } },
        { "Plain",           { DSI3DL2_ENVIRONMENT_PRESET_PLAIN } },
        { "ParkingLot",      { DSI3DL2_ENVIRONMENT_PRESET_PARKINGLOT } },
        { "SewerPipe",       { DSI3DL2_ENVIRONMENT_PRESET_SEWERPIPE } },
        { "UnderWater",      { DSI3DL2_ENVIRONMENT_PRESET_UNDERWATER } },
        { "NoReverb",        { DSI3DL2_ENVIRONMENT_PRESET_NOREVERB } },
    };

    HRESULT hr = S_OK;
    UINT i;

    CHECKRUN(DirectSoundCreate( NULL, &g_dsound, NULL ));
    if (_strcmpi("none", g_TestParams.szDSScratchImage))
        CHECKRUN(DownloadScratch(g_dsound, g_TestParams.szDSScratchImage));

    if (_strcmpi("full", g_TestParams.szDSHRTF) == 0)
    {
        CHECK(DirectSoundUseFullHRTF());
    }
    else if (_strcmpi("light", g_TestParams.szDSHRTF) == 0)
    {
        CHECK(DirectSoundUseLightHRTF());
    }
    else
        ASSERT(FALSE);

    for (i = 0; i < sizeof(Environments) / sizeof(Environments[0]); i++)
    {
        if (_strcmpi(Environments[i].pszName, g_TestParams.szDSEnvironment) == 0)
        {
            CHECK(g_dsound->SetI3DL2Listener(&Environments[i].Properties, DS3D_IMMEDIATE));
            break;
        }
    }

    ASSERT(i < sizeof(Environments) / sizeof(Environments[0]));

    return hr;
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

	// figure out the remote path for each file
#define MAX_MEDIA_FILES 34
	UINT	nNumFiles = 0;
	MEDIAFILEDESC MediaFiles[MAX_MEDIA_FILES];
	for ( UINT i=0; i < MAX_MEDIA_FILES-1; i++ )
	{
		MediaFiles[i].lpszFromPath = new CHAR [ MAX_PATH ];
		MediaFiles[i].lpszToPath = new CHAR [ MAX_PATH ];
		MediaFiles[i].dwFlags = COPY_IF_NEWER;
		if ( NULL == MediaFiles[i].lpszFromPath || NULL == MediaFiles[i].lpszToPath )
		{
			return E_OUTOFMEMORY;
		}
	}
	MediaFiles[i].lpszFromPath = MediaFiles[i].lpszToPath = NULL;

	hr = TranslatePath( g_TestParams.szBallBaseTex, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
	if ( SUCCEEDED( hr ) ) {
		nNumFiles++;
		hr = TranslatePath( g_TestParams.szBallBumpTex, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath ); 
	}
	if ( SUCCEEDED( hr ) ) {
		nNumFiles++;
		hr = TranslatePath( g_TestParams.szSphereBaseTex, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
	}
	if ( SUCCEEDED( hr ) ) {
		nNumFiles++;
		hr = TranslatePath( g_TestParams.szLightVSH, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
	}
	if ( SUCCEEDED( hr ) ) {
		nNumFiles++;
		hr = TranslatePath( g_TestParams.szSparkVSH, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
	}
	if ( SUCCEEDED( hr ) && g_TestParams.szLightSound[0] ) {
		nNumFiles++;
		hr = TranslatePath( g_TestParams.szLightSound, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
	}
	if ( SUCCEEDED( hr ) ) {
		nNumFiles++;
		hr = TranslatePath( g_TestParams.szBounceSound, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
	}
	
	if ( SUCCEEDED( hr ) ) {
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "DSP/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\DSPCode\\" );
	}
	
	if ( g_TestParams.bDMusic && SUCCEEDED( hr ) )
	{
		if ( SUCCEEDED( hr ) ) {
			nNumFiles++;
			hr = TranslatePath( g_TestParams.szDMStressDir, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
		}
		if ( SUCCEEDED( hr ) ) {
			nNumFiles++;
			hr = TranslatePath( g_TestParams.szDMSupportDir, MediaFiles[nNumFiles].lpszFromPath, MediaFiles[nNumFiles].lpszToPath );
		}
		/*nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/ADPCM/DLS/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\ADPCM\\DLS\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/ADPCM/WaveSeg/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\ADPCM\\WaveSeg\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Default/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Default\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/MAnyNotes/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\ManyNotes\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/MIDI/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\MIDI\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/MIDI/1/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\MIDI\\1\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Other/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Other\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Other/Beat/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Other\\Beat\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Other/Bug6712/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Other\\Bug6712\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Other/MusicReverb/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Other\\MusicReverb\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Other/Sine/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Other\\Sine\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Scott/BBall/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Scott\\BBall\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/support/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\support\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/Variety/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\Variety\\" );
		nNumFiles++;*/
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/ADPCM/Looping64/Original/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\ADPCM\\Looping64\\Original\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/ADPCM/Looping64/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\ADPCM\\Looping64\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/ADPCM/OneShot/Original/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\ADPCM\\OneShot\\Original\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/ADPCM/OneShot/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\ADPCM\\OneShot\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/ADPCM/Streaming/Original/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\ADPCM\\Streaming\\Original\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/ADPCM/Streaming/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\ADPCM\\Streaming\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/Normal/Looping/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\Normal\\Looping\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/Normal/OneShot/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\Normal\\OneShot\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/Normal/Streaming/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\Normal\\Streaming\\" );
		nNumFiles++;
		strcpy( MediaFiles[nNumFiles].lpszFromPath, "mediastress/music/wav/Other/" ); strcpy( MediaFiles[nNumFiles].lpszToPath, "T:\\media\\music\\wav\\Other\\" );
		nNumFiles++;
	}

	//fill in NULLs for unused slots
	for ( i=nNumFiles; i < MAX_MEDIA_FILES; i++ )
	{
		if ( NULL != MediaFiles[i].lpszFromPath )
		{
			delete [] MediaFiles[i].lpszFromPath;
			MediaFiles[i].lpszFromPath = NULL;
		}
		if ( NULL != MediaFiles[i].lpszToPath )
		{
			delete [] MediaFiles[i].lpszToPath;
			MediaFiles[i].lpszToPath = NULL;
		}
	}
	
    SETLOG( g_hLog, "robheit", "MCPX", "media", "Basic" );

	MLSetErrorLevel( 3 );
	EXECUTE( LoadMedia( MediaFiles ) );
	for ( i=0; i < 32; i++ )
	{
		if ( NULL != MediaFiles[i].lpszFromPath )
		{
			delete [] MediaFiles[i].lpszFromPath;
			MediaFiles[i].lpszFromPath = NULL;
		}
		if ( NULL != MediaFiles[i].lpszToPath )
		{
			delete [] MediaFiles[i].lpszToPath;
			MediaFiles[i].lpszToPath = NULL;
		}
	}
	if ( FAILED( hr ) )
	{
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

	// Initialize DSound
	hr = InitializeDSound();
	if(hr != S_OK)
		return hr;

    hr = StartDSStress(g_TestParams.dwDSSThreads,                                               //Threads
                       g_TestParams.dwDSSTotalBuffers2D,    g_TestParams.dwDSSTotalBuffers3D,   //Total 2D/3D Buffers                       
                       g_TestParams.dwDSSTotalStreams2D,    g_TestParams.dwDSSTotalStreams3D,   //Total 2D/3D Streams
                       g_TestParams.dwDSSMixinBuffers2D,    g_TestParams.dwDSSMixinBuffers3D,   //Of the2D/3D Buffers, how many will be mixin buffers (these are given highest prio and are always allocated)
                       g_TestParams.dwDSSSends2D,           g_TestParams.dwDSSSends3D,          //How many of the regular 2D buffers will send to a 2D/3D buffer.
                       g_TestParams.dwDSSWait);

	if(hr != S_OK)
		return hr;
    
    // Initialize DMusic
    hr = g_music.Initialize();
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
	
    if (g_dsound)
    {
        g_dsound->Release();
        g_dsound = NULL;
    }
    
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

    StopDSStress();

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


}; //end namespace WFVO


//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WFVO )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WFVO )
    EXPORT_TABLE_ENTRY( "StartTest", WFVO::media_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WFVO::media_EndTest )
END_EXPORT_TABLE( WFVO )


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
    WFVO::media_StartTest(NULL);
    WFVO::media_EndTest();
}
#endif // NOLOGGING


