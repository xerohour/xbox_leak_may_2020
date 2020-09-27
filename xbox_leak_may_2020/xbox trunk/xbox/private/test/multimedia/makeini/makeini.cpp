/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       makeini.cpp
 *  Content:    makeini tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/ 3/01    danrose Created to test Xbox makeini
 *  01/30/02    danhaff Added makeini_WFVOTest
 *
 ****************************************************************************/

#include "makeini.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

static const float cPi = 3.14159265359f;




/****************************************************************************
 *
 *  sfprintf
 *
 *  Description:
 *      Writes to both the file and the screen.
 *
 *  Arguments:
 *      same as fprintf
 *
 *  Returns:  
 *      same as fprintf
 *
 ****************************************************************************/
HRESULT sfprintf
(
    FILE *pFile, 
    LPSTR                   szFormat,
    ...
)
{
    va_list va;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    vfprintf(pFile, szFormat, va);
    va_end(va);    
    DbgPrint("%s", szBuffer);
    return S_OK;
};


/****************************************************************************
 *
 *  makeini_BasicTest
 *
 *  Description:
 *      Basic Test for makeini
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT makeini_BasicTest( void )
{
    HRESULT             hr              = S_OK;
	FILE*				pFile			= NULL;


    LPSTR szDMAudioPaths[4] = 
    {
    "DMUS_APATH_SHARED_STEREOPLUSREVERB",
    "DMUS_APATH_SHARED_STEREO",
    "DMUS_APATH_DYNAMIC_MONO",
    "DMUS_APATH_DYNAMIC_3D"
    };


    LPSTR szDMStressDirs[1] = 
    {
    "t:\\media\\music\\midi"
//    "t:\\media\\music\\ADPCM",
//"t:\\media\\music\\wav\\normal"
    };

    LPSTR szDMHeaps[4] = 
    {
    "FixedUser",
    "FixedDefault",
    "Tracking",
    "Default"
    };

    D3DMULTISAMPLE_TYPE d3dmst[] =
    {
    D3DMULTISAMPLE_NONE,
    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,
    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,
    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR,
    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,
    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,
    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,
    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,
    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,
    };

	//
	// The variables that are outputted to the file
	//

	LARGE_INTEGER		liSeed;
	DWORD				dwMinSpeedWhole;
	DWORD				dwMinSpeedFrac;
	DWORD				dwMaxSpeedWhole;
	DWORD				dwMaxSpeedFrac;
	DWORD				dwSparksPerCollision;
	DWORD				dwMaxSparks;
	DWORD				dwTemp;
	float				radius;
	float				minRadius;

    BOOL bDMDoWorkLocally;
    DWORD dwDMDoWorkFrequency;

    CHAR  szDMHeap[MAX_PATH];
    DWORD dwDMFixedHeapSizeNorm;
    DWORD dwDMFixedHeapSizePhys;

	DWORD				K[] = { 8, 11, 16, 22, 32, 44, 48 };
	DWORD				NUM_K = 7;

	//
	// get the random seed
	//

	QueryPerformanceCounter( &liSeed );
	srand( liSeed.LowPart );

	//
	// setup the ball speeds
	//

	dwTemp = 0;
	dwMinSpeedWhole = rand() % 21;
	dwMaxSpeedWhole = rand() % 21;
	dwMinSpeedFrac = rand() % 10000;
	dwMaxSpeedFrac = rand() % 10000;

	// normalize the fractions 

	while( dwMinSpeedFrac < 1000 )
		dwMinSpeedFrac *= 10;

	while( dwMaxSpeedFrac < 1000 )
		dwMaxSpeedFrac *= 10;

	//
	// decide the max and min speeds
	//

	dwTemp = min( dwMinSpeedWhole, dwMaxSpeedWhole );
	dwMaxSpeedWhole = max( dwMinSpeedWhole, dwMaxSpeedWhole );
	dwMinSpeedWhole = dwTemp;

	dwTemp = min( dwMinSpeedFrac, dwMaxSpeedFrac );
	dwMaxSpeedFrac = max( dwMinSpeedFrac, dwMaxSpeedFrac );
	dwMinSpeedFrac = dwTemp;


	//
	// decide on the sparks
	//

	dwSparksPerCollision = rand() % 11;
	DWORD maxSparks = (DWORD) rand() % 251;
	dwMaxSparks = (DWORD) max(dwSparksPerCollision, maxSparks);

    //
    //Calculate how many voices DMusic may use.
    //We have 256 voices.  1 is used by DSound as a dummy, 1 is used by the audiopath (unless it's STEREOPLUSREVERB or STEREO), and 1 is used for each ball and light.
    //

    //These must total 63 (1 3D voice may be used by the 3D Audiopath)
    DWORD dwMaxBalls = 49;
    DWORD dwMaxLights = 14;        

    BOOL bSubmixBallSound = rand() % 2;
    BOOL b3DSound         = rand() % 2;
    BOOL bLightSound      = FALSE;          //ASSUME there is no light sound.
    BOOL bDMusic          = TRUE;

    LONG Voices2DPerObject =
                          (  (!b3DSound || bSubmixBallSound) ? 1 : 0)  //Collision Buffer, if we're not 3D or we're submixing.
                        + (  (bLightSound && (!b3DSound || bSubmixBallSound)) ? 1 : 0)  //Ambient Buffer, "" ""
                        + (  (bSubmixBallSound && !b3DSound) ? 1 : 0); //Submix Buffer, if we're not 3D.


    LONG Voices3DPerObject =     (dwMaxBalls + dwMaxLights)                    //For each ball/light, there will be 1-3 3D buffers.
                               * (
                                 (  ( b3DSound    && !bSubmixBallSound) ? 1 : 0)           //Collision Buffer - if we're 3D and not submixing.
                               + (  ( bLightSound && ( b3DSound    && !bSubmixBallSound)) ? 1 : 0)           //Ambient Buffer, ""  ""
                               + (  ( b3DSound    &&  bSubmixBallSound) ? 1 : 0)           //Submix Buffer, if we are 3D.
                                 );

    LONG Voices2D =   (dwMaxBalls + dwMaxLights) * Voices2DPerObject;
    LONG Voices3D =   (dwMaxBalls + dwMaxLights) * Voices3DPerObject;

    //For some reason, XBPerfmon always shows 188 free voices when nothing is playing, hence we shall use that # here.
    LONG dwMaxDMVoiceCount = 188 - Voices2D;
    if (dwMaxDMVoiceCount < 0)
    {
        dwMaxDMVoiceCount = 0;
        bDMusic = 0;
    }

    printf("");
    printf("2D Voices Per Object     = %d", Voices2DPerObject);
    printf("3D Voices Per Object     = %d", Voices3DPerObject);
    printf("2D Voices                = %d", Voices2D);
    printf("3D Voices                = %d", Voices3D);
    printf("");


	// Min Radius
	// The volume of the sphere must be at least 3 times the volume of all the 
	// balls and lights within it, therefore:
	// 4/3*PI*r^3 >= 3 * (maxBalls * ballVolume + maxLights * lightVolume), or
	// r^3 >= 9/4 * (maxBalls * ballVolume + maxLights * lightVolume)
	//   ballRadius is defined in media\scene.cpp as 0.5
	//   lightRadius is defined in media\scene.cpp as 0.125
	minRadius = 9.0f / 4.0f * 
				(((float)dwMaxBalls * (4.0f / 3.0f * cPi * 0.5f * 0.5f * 0.5f)) +
				 ((float)dwMaxLights * (4.0f / 3.0f * cPi * 0.125f * 0.125f * 0.125f)));
	minRadius = (float)pow(minRadius, 1.0f / 3.0f);

	radius = (float)(rand() % 18999 + 3000) / 1000.0f;
	radius = max(radius, minRadius);

    //Decide whether or not to do work locally or not.
    bDMDoWorkLocally = rand() % 2;    
    if (bDMDoWorkLocally)
        dwDMDoWorkFrequency = rand() % 60 + 1;
    else
        dwDMDoWorkFrequency = 0;

    //Always a fixed heap now.
//	sprintf(szDMHeap, rand() % NUMELEMS(szDMHeaps));
	sprintf(szDMHeap, "FixedUser");
    

    if (_strcmpi(szDMHeap, "FixedUser") == 0)
    {    
        dwDMFixedHeapSizeNorm = 8 * 1024 * 1024;     //8 MB
        dwDMFixedHeapSizePhys =     1024 * 1024;     //1 MB
    }
    else
    {    
        dwDMFixedHeapSizeNorm = 0;
        dwDMFixedHeapSizePhys = 0;
    }

    // Choose a multisample type
    DWORD dwMultiSampleType = (DWORD)d3dmst[rand() % 9];

    // Choose a back buffer count
    DWORD dwBackBufferCount = rand() % 3;

	// remove any read only flags on the file
	//

	if ( ! SetFileAttributes( "t:\\testini.ini", FILE_ATTRIBUTE_NORMAL ) )
		hr = E_FAIL;

	// 
	// open the testini.ini
	//

	pFile = fopen( "t:\\testini.ini", "wt" );

	//
	// make sure allocation succeeded
	//

	if ( NULL == pFile )
		hr = E_OUTOFMEMORY;

	//
	// write out the test params to the testini.ini
	//

	if ( SUCCEEDED( hr ) )
	{
		sfprintf( pFile, ";******************************************************************************************\n" );
		sfprintf( pFile, ";TESTINI.INI:  This file was generated by the makeini.lib for media, and fills in \n" );
		sfprintf( pFile, ";                all the variables used by media.\n" );
		sfprintf( pFile, ";******************************************************************************************/\n" );
		sfprintf( pFile, "\n" );
		sfprintf( pFile, "[testlist]\n" );
		sfprintf( pFile, "media\n" );
		sfprintf( pFile, "\n" );
		sfprintf( pFile, "[media]\n" );
		sfprintf( pFile, "ThreadStackSize = 65536\n" );
		sfprintf( pFile, "dwSeed = %u\n", liSeed.LowPart );
		sfprintf( pFile, "dwBallDetail = %u\n", rand() % 17 + 4 );
		sfprintf( pFile, "dwWallDetail =  %u\n", rand() % 59 + 6 );
		sfprintf( pFile, "dwLightDetail = %u\n", rand() % 59 + 6 );
		sfprintf( pFile, "dwNumLayers =  %u\n", rand() % 51 );
		sfprintf( pFile, "dwNumBalls = %u\n", rand() % dwMaxBalls + 1 );
		sfprintf( pFile, "dwMaxBalls = %u\n", dwMaxBalls );
		sfprintf( pFile, "dwNumLights = %u\n", rand() % dwMaxLights + 1 );
		sfprintf( pFile, "dwMaxLights = %u\n", dwMaxLights );
		sfprintf( pFile, "dwSparkMaxAge = %u\n", rand() % 501 );
		sfprintf( pFile, "dwMaxSparks = %u\n", dwMaxSparks );
		sfprintf( pFile, "dwSparksPerCollision = %u\n", dwSparksPerCollision );
		sfprintf( pFile, "bSparksAreLights = %u\n", rand() % 2 );
		sfprintf( pFile, "bDrawWireframe = %u\n", rand() % 2 );
		sfprintf( pFile, "bTextureBalls = %u\n", rand() % 2 );
		sfprintf( pFile, "fRadius = %f\n", radius);
		sfprintf( pFile, "fMaxSpeed = %u.%u\n", dwMaxSpeedWhole, dwMaxSpeedFrac );
		sfprintf( pFile, "fMinSpeed = %u.%u\n", dwMinSpeedWhole, dwMinSpeedFrac );
		sfprintf( pFile, "bDMusic = %d\n", bDMusic );
		sfprintf( pFile, "bDMDoWorkLocally = %u\n", bDMDoWorkLocally);
		sfprintf( pFile, "dwDMDoWorkFrequency = %u\n", dwDMDoWorkFrequency);
		sfprintf( pFile, "bDMUseGM = 1\n" );
		sfprintf( pFile, "bDMLoaderThread = %u\n", rand() % 2 );
		sfprintf( pFile, "dwDMVoiceCount = %u\n", rand() % dwMaxDMVoiceCount + 1 );
		sfprintf( pFile, "lDMMasterVolume = -%u\n", rand() % 2001 );
		
        //sfprintf( pFile, "fDMMasterTempo = %u.%u\n", rand() % 100, rand() % 9999 );  //BUGBUG: Put this back in when 7447 is fixed!!!
        sfprintf( pFile, "fDMMasterTempo = %u.%u\n", rand() % 2, rand() % 9999 );


		sfprintf( pFile, "lDMMasterTranspose = %i\n", rand() % 256 - 128 );
		sfprintf( pFile, "dwFileIOSize = %u\n", rand() % 1048576 + 1 );
        sfprintf( pFile, "szDMHeap = \"%s\"\n", szDMHeap );
        sfprintf( pFile, "dwDMFixedHeapSizeNorm = %u\n", dwDMFixedHeapSizeNorm);
        sfprintf( pFile, "dwDMFixedHeapSizePhys = %u\n", dwDMFixedHeapSizePhys);
		sfprintf( pFile, "lDSVolume = -%u\n", rand() % 2001 );
		sfprintf( pFile, "dwDSFreq = %u\n", rand() % 75990 + 10010 );
        sfprintf( pFile, "bSubMixBallSound = %u\n", bSubmixBallSound);

        sfprintf( pFile, "b3DSound = %u\n", b3DSound );
        sfprintf( pFile, "szDMAudioPath = %s\n", szDMAudioPaths[rand() % NUMELEMS(szDMAudioPaths)]);
        sfprintf( pFile, "szDMStressDir = %s\n", szDMStressDirs[rand() % NUMELEMS(szDMStressDirs)]);
        
		switch( rand() % 2 )
		{
		case 0:
			sfprintf( pFile, "szBounceSound = \"t:\\media\\audio\\pcm\\ball.wav\"\n" );
			break;

		case 1:
			sfprintf( pFile, "szBounceSound = \"t:\\media\\audio\\pcm\\%uK%uB%c.wav\"\n", K[rand() % NUM_K], ( rand() % 2 + 1 ) * 8, ( b3DSound ? 0 : rand() % 2) ? 'S' : 'M' ); 
			break;
		}

        sfprintf( pFile, "dwMultiSampleType = %u\n", dwMultiSampleType);
        sfprintf( pFile, "dwBackBufferCount = %u\n", dwBackBufferCount);

		sfprintf( pFile, "\n" );
		sfprintf( pFile, "[XNetRef]\n" );
		sfprintf( pFile, "cfgFlags=0x01\n" );
	}

	if ( pFile )
	{
		fclose( pFile );
	}

    //
    // Return the Hresult
    //

    return hr;
}


/****************************************************************************
 *
 *  makeini_WFVOTest
 *
 *  Description:
 *      Writes a test profile for the WFVO suite.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT makeini_WFVOTest( void )
{
    HRESULT             hr              = S_OK;
	FILE*				pFile			= NULL;
	if ( ! SetFileAttributes( "t:\\testini.ini", FILE_ATTRIBUTE_NORMAL ) )
		hr = E_FAIL;
	pFile = fopen( "t:\\testini.ini", "wt" );
	if ( NULL == pFile )
		hr = E_OUTOFMEMORY;
	if ( SUCCEEDED( hr ) )
	{
	    sfprintf( pFile, ";******************************************************************************************\n" );
	    sfprintf( pFile, ";TESTINI.INI:  This file was generated by the makeini.lib for media, and fills in \n" );
	    sfprintf( pFile, ";                all the variables used by media.\n" );
	    sfprintf( pFile, ";******************************************************************************************/\n" );
	    sfprintf( pFile, "\n" );
	    sfprintf( pFile, "[testlist]\n" );
	    sfprintf( pFile, "media\n" );
	    sfprintf( pFile, "\n" );
	    sfprintf( pFile, "[media]\n" );
        sfprintf( pFile, "dwBallDetail = 8\n");
        sfprintf( pFile, "dwWallDetail = 8\n");
        sfprintf( pFile, "dwNumLayers = 1\n");
        sfprintf( pFile, "dwNumBalls = 2\n");
        sfprintf( pFile, "dwMaxBalls = 2\n");
        sfprintf( pFile, "dwNumLights = 2\n");
        sfprintf( pFile, "dwMaxLights = 2\n");
        sfprintf( pFile, "fRadius = 20\n");
        sfprintf( pFile, "fMaxSpeed =10\n");
        sfprintf( pFile, "fMinSpeed = 1\n");
        sfprintf( pFile, "fDMMasterTempo = 1\n");
        sfprintf( pFile, "szBounceSound = ""t:\\media\\audio\\pcm\\ball.wav""\n");
        sfprintf( pFile, "bDMusic = 0\n");
        sfprintf( pFile, "dwDSDebugLevel = 1\n");
        sfprintf( pFile, "bCopyMediaFromNetwork = 1\n");
        sfprintf( pFile, "dwDSSThreads          = 1\n");
        sfprintf( pFile, "dwDSSTotalBuffers2D   = 187\n");
        sfprintf( pFile, "dwDSSTotalBuffers3D   = 63\n");
        sfprintf( pFile, "dwDSSTotalStreams2D   = 0\n");
        sfprintf( pFile, "dwDSSTotalStreams3D   = 0\n");
        sfprintf( pFile, "dwDSSMixinBuffers2D   = 0\n");
        sfprintf( pFile, "dwDSSMixinBuffers3D   = 0\n");
        sfprintf( pFile, "dwDSSSends2D          = 0\n");
        sfprintf( pFile, "dwDSSSends3D          = 0\n");
        sfprintf( pFile, "dwDSSWait             = 5000\n");

		sfprintf( pFile, "\n" );
		sfprintf( pFile, "[XNetRef]\n" );
		sfprintf( pFile, "cfgFlags=0x01\n" );

    }

	if ( pFile )
	{
		fclose( pFile );
	}

    return hr;
};


/****************************************************************************
 *
 *  makeini_StartTest
 *
 *  Description:
 *      The Harness Entry into the makeini tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI makeini_StartTest( HANDLE LogHandle )
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
    // Test makeini in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "danrose", "MCPX", "makeini", "Basic" );
    EXECUTE( makeini_BasicTest() );
}

/****************************************************************************
 *
 *  makeini_EndTest
 *
 *  Description:
 *      The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI makeini_EndTest( VOID )
{
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      the exe entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef NOLOGGING
void __cdecl main( void )
{
    makeini_StartTest( NULL );
    makeini_EndTest();

	XLaunchNewImage( NULL, NULL );
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( makeini )
#pragma data_seg()

BEGIN_EXPORT_TABLE( makeini )
    EXPORT_TABLE_ENTRY( "StartTest", makeini_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", makeini_EndTest )
END_EXPORT_TABLE( makeini )
