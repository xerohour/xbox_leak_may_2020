/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	regsuite.cpp

Abstract:

	Implementation file for regression suite library (regsuite.lib)

Author:

	Jeff Sullivan (jeffsul) 11-Feb-2002

[Notes:]

	Known Issues

		RSBeginTest() returns XLL_FAIL at this point for error.  It should return XLL_ABORT or XLL_SKIP when those values are defined.

		If g_D3D_PresentFlag is not set to one of the following values, the variable will not be set correctly:
		D3DPRESENTFLAG_WIDESCREEN
		D3DPRESENTFLAG_INTERLACED            
		D3DPRESENTFLAG_PROGRESSIVE
		D3DPRESENTFLAG_FIELD

		If g_D3D_MultiSampleType is not set to one of the following values, the variable will not be set correctly:
		D3DMULTISAMPLE_NONE                                    
		D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR            
		D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX          
		D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR 
		D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR   
		D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR            
		D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN          
		D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR            
		D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN          
		D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN          
		D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN

Revision History:

	13-Feb-2002 jeffsul
		Initial Version

--*/

#include "regsuite.h"
#include <stdio.h>
#include <xdbg.h>
#include <xbeimage.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

//------------------------------------------------------------------------------
//	Globals
//------------------------------------------------------------------------------
LPBYTE		g_pUsedMem		= NULL;
BOOL		g_bHVSLaunched	= FALSE;
RSINIT_FUNC	g_pfTestFunc	= NULL;
LPDIRECTSOUND8 g_pDSound	= NULL;
LPDIRECTSOUNDBUFFER     g_pDSBuffer[256] = {0};


//------------------------------------------------------------------------------
//	Pre-defines
//------------------------------------------------------------------------------
VOID
SetDefaults( 
	RSTESTPARAMS*		pTestParams
);

HRESULT 
ParseTestParams(
	CHAR*				lpszBuffer,
	RSTESTPARAMS*		pTestParams,
	LPVOID				lpUserParam
);

/*++

Routine Description:

	Parses launch data for test initialization.  
	If the test is launched from HVS Launcher, the data comes from the launch data.  Otherwise (test is stand alone), 
		the data comes from <XBENAME>.ini file in the same directory as the .xbe.
	The routine generates a .repro file (in the same directory as the .xbe) that will only be removed if RSEndTest() 
		is called with 0 passed as uiFail.
	The routine also allocates memory and voices as specified by launch parameters.  To override this, use the flags parameter.
	The routine will set medialoader debug level based on input.

Arguments:

	IN DWORD			dwFlags			-	0 for default behaviour, use RS_DONTALLOCXXX to avoid unwanted allocations
	OUT RSTESTPARAMS*	pTestParams		-	pointer to a struct to be filled in with launch data
	IN RSINIT_FUNC		LocalTestInit	-	pointer to function to parse remaining test-specific init values
	IN LPVOID			lpUserParam		-	pointer to a user defined variable that is passed to LocalTestInit when called

Return Value:

	XLL_PASS	-	if success
	XLL_FAIL	-	never returned
	XLL_INFO	-	?
	XLL_ABORT	-	if function fails
	XLL_SKIP	-	if current configuration is not supported

--*/
DWORD _stdcall
RSBeginTest( 
	IN DWORD			dwFlags,
	OUT RSTESTPARAMS*	pTestParams,
	IN RSINIT_FUNC		LocalTestInit,
	IN LPVOID			lpUserParam
)
{
	DWORD		dwStatus		= XLL_PASS;//XLL_SKIP;
	HANDLE		hFile			= INVALID_HANDLE_VALUE;
	CHAR*		pIniBuffer		= NULL;

	if ( NULL == pTestParams )
	{
		DbgPrint( "Null passed for pTestParams\n" );
		goto error;
	}

	g_pfTestFunc = LocalTestInit;

	//------------------------------------------------------------------------------
	//	Determine how the test was launched
	//------------------------------------------------------------------------------

	DWORD		dwSuccess;
	DWORD		dwLaunchType;
	LAUNCH_DATA	LaunchData;
	ToTestData* pToTestData;

	dwSuccess = XGetLaunchInfo( &dwLaunchType, &LaunchData );
	if ( ERROR_SUCCESS == dwSuccess )
	{
		if ( LDT_TITLE == dwLaunchType )
		{
			pToTestData = (ToTestData*)(LaunchData.Data);
			if ( HVSLAUNCHERID == pToTestData->titleID )
			{
				g_bHVSLaunched = TRUE;
			}
		}
	}

	//------------------------------------------------------------------------------
	//	Read in initialization values
	//------------------------------------------------------------------------------
	if ( TRUE == g_bHVSLaunched )
	{
		// get info from launch data
		ParseTestParams( pToTestData->configSettings, pTestParams, lpUserParam );
	}
	else
	{
		// get info from ini file
		DWORD	dwFileSize		= 0;
		DWORD	dwNumBytesRead	= 0;
		BOOL	bSuccess		= FALSE;

		CHAR	szFileName[MAX_PATH];
		CHAR	szPath[MAX_PATH] = "D:\\";
		
		wcstombs( szFileName, XeImageHeader()->Certificate->TitleName, wcslen( XeImageHeader()->Certificate->TitleName ) + 1 );
		strcat( szPath, szFileName );
		strcat( szPath, ".ini" );

		hFile = CreateFile( szPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE == hFile )
		{
			DbgPrint( "Unable to open .ini file '%s'\n", szPath );
			goto error;
		}

		dwFileSize = GetFileSize( hFile, NULL );
		if ( -1 == dwFileSize )
		{
			DbgPrint( "Unable to get .ini file size for '%s'\n", szPath );
			goto error;
		}
		
		pIniBuffer = new CHAR [ dwFileSize + 1 ];
		if ( NULL == pIniBuffer )
		{
			DbgPrint( "Ran out of memory creating ini buffer\n" );
			goto error;
		}
		
		bSuccess = ReadFile( hFile, pIniBuffer, dwFileSize, &dwNumBytesRead, NULL );
		if ( FALSE == bSuccess || dwNumBytesRead != dwFileSize )
		{
			DbgPrint( "Unable to read file '%s' (error %x)\n", szPath, GetLastError() );
			goto error;
		}
		
		pIniBuffer[ dwFileSize ] = '\0';
		CloseHandle( hFile );

		ParseTestParams( pIniBuffer, pTestParams, lpUserParam );

		SAFEDELETEARRAY( pIniBuffer );
	}

	//------------------------------------------------------------------------------
	//	Do work
	//------------------------------------------------------------------------------
	// set medialoader error level
	if ( !(RS_DONTSETMLDEBUG & dwFlags) )
	{
		//MLSetErrorLevel( pTestParams->RS_dwMLErrorLevel );
	}

	// allocate a chunk of memory
	if ( !(RS_DONTALLOCMEMORY & dwFlags) && 0 < pTestParams->RS_dwDecreaseMemory )
	{
		g_pUsedMem = new BYTE [ pTestParams->RS_dwDecreaseMemory ];
		if ( NULL == g_pUsedMem )
		{
			DbgPrint( "Ran out of memory creating pre-allocation\n" );
			goto error;
		}
	}

	// allocate voices
	if ( !(RS_DONTALLOCVOICES & dwFlags) && (0 != pTestParams->RS_dwDSUsedVoices2D || 0 != pTestParams->RS_dwDSUsedVoices3D) )
	{
		HRESULT                 hr;
		DSBUFFERDESC            dsbd;
		WAVEFORMATEX            wfx;
		
		// Create DSound Object
		hr = DirectSoundCreate(NULL, &g_pDSound, NULL);
		if( FAILED(hr) )
		{
			DbgPrint( "Failed to create DSound (error %x)\n", hr );
            goto error;
		}
		
		// Check free 2D/3D voices
		DSCAPS deviceCaps;
		g_pDSound->GetCaps(&deviceCaps);
		
		if ( pTestParams->RS_dwDSUsedVoices2D > deviceCaps.dwFree2DBuffers || pTestParams->RS_dwDSUsedVoices2D > deviceCaps.dwFree3DBuffers )
		{
			DbgPrint( "Too many voices: 2D = %d, 3D = %d\n", pTestParams->RS_dwDSUsedVoices2D, pTestParams->RS_dwDSUsedVoices3D );
			goto error;
		}

		// verify dsp
		//ASSERT(XAudioDownloadEffectsImage("dsstdfx", &effectLoc, XAUDIO_DOWNLOADFX_XBESECTION, &pDesc) == S_OK);

		// set 3D algorithm initialization function
		if ( 0 == strcmp( pTestParams->DS_szHRTF, "FULL" ) ) DirectSoundUseFullHRTF();
		else if ( 0 == strcmp( pTestParams->DS_szHRTF, "LIGHT" ) ) DirectSoundUseLightHRTF();
		else if ( 0 == strcmp( pTestParams->DS_szHRTF, "PAN3D" ) ) DirectSoundUsePan3D();
		else { DbgPrint( "Un-supported DS 3D algorithm '%s'\n", pTestParams->DS_szHRTF ); goto error; }
		
		// Data structures for buffer
		memset(&wfx, 0, sizeof(WAVEFORMATEX));
		wfx.wFormatTag          = WAVE_FORMAT_PCM;
		wfx.nChannels           = 1;
		wfx.nSamplesPerSec      = 11500;
		wfx.nBlockAlign         = 1;
		wfx.nAvgBytesPerSec     = wfx.nSamplesPerSec * wfx.nBlockAlign;
		wfx.wBitsPerSample      = 8;
		
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize             = sizeof(DSBUFFERDESC);
		dsbd.dwFlags            = 0;
		dsbd.dwBufferBytes      = 4;
		dsbd.lpwfxFormat  = (LPWAVEFORMATEX)&wfx;
		
		// Create the DSound 2D buffers
		for ( UINT i=0; i < pTestParams->RS_dwDSUsedVoices2D; i++ )
		{
			hr = g_pDSound->CreateSoundBuffer(&dsbd, &g_pDSBuffer[i], NULL);
			if( FAILED(hr) )
			{
				DbgPrint( "Can not create DirectSoundBuffer (error %x)\n", hr );
				goto error;
			}
		}
		// create the DSound 3D buffers
		dsbd.dwFlags            = DSBCAPS_CTRL3D;
		for ( ; i < pTestParams->RS_dwDSUsedVoices2D + pTestParams->RS_dwDSUsedVoices3D; i++ )
		{
			hr = g_pDSound->CreateSoundBuffer(&dsbd, &g_pDSBuffer[i], NULL);
			if( FAILED(hr) )
			{
				DbgPrint( "Can not create DirectSoundBuffer (error %x)\n", hr );
				goto error;
			}
		}
	}

	return dwStatus;

error:
	SAFEDELETEARRAY( g_pUsedMem );
	SAFEDELETEARRAY( pIniBuffer );
	for ( UINT i=0; i < 256; i++ )
	{
		RELEASE( g_pDSBuffer[i] );
	}
	RELEASE( g_pDSound );
	CloseHandle( hFile );
	dwStatus = XLL_FAIL;//XLL_ABORT;
	return dwStatus;
}


/*++

Routine Description:

	Called at the end of a test app to give control back to the HVS launcher or reboot the box.
	If uiFail is 0, this routine deletes the .repro file created by RSInitializeTest().
	Also cleans up any memory/voices allocated by RSBeginTest().

Arguments:

	UINT64		uiPass		-	the number of tests passed
	UINT64		uiFail		-	the number of tests failed
	LPCSTR		lpszNotes	-	optional string of notes to pass back to the launcher

Return Value:

	None

--*/
VOID _stdcall
RSEndTest( 
	IN UINT64			uiPass,
	IN UINT64			uiFail,
	IN LPCSTR			lpszNotes
)
{
	LAUNCH_DATA		LaunchData;
	ToLauncherData* pLauncherData = (ToLauncherData*)&LaunchData;
	pLauncherData->titleID = XeImageHeader()->Certificate->TitleID;
	pLauncherData->numPass = uiPass;
	pLauncherData->numFail = uiFail;
	strcpy( pLauncherData->notes, lpszNotes );

	//------------------------------------------------------------------------------
	//	Clean up
	//------------------------------------------------------------------------------

	// free used memory
	if ( NULL != g_pUsedMem )
	{
		SAFEDELETEARRAY( g_pUsedMem );
	}

	//	delete .repro file
	if ( 0 == uiFail )
	{
		CHAR	szFileName[MAX_PATH];
		CHAR	szPath[MAX_PATH] = "D:\\";
		
		if ( TRUE == g_bHVSLaunched )
		{
			strcat( szPath, "tests\\" );
		}
		
		wcstombs( szFileName, XeImageHeader()->Certificate->TitleName, wcslen( XeImageHeader()->Certificate->TitleName ) + 1 );
		strcat( szPath, szFileName );
		strcat( szPath, ".repro" );
		
		HANDLE hFile = CreateFile( szPath, DELETE, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL );
		CloseHandle( hFile );
	}

	// free voices
	for ( UINT i=0; i < 256; i++ )
	{
		RELEASE( g_pDSBuffer[i] );
	}
	RELEASE( g_pDSound );

	//------------------------------------------------------------------------------
	//	Return to Launcher or reboot system
	//------------------------------------------------------------------------------
	if ( TRUE == g_bHVSLaunched)
    {
        LaunchImage( "D:\\default.xbe", &LaunchData );
    }
    else
	{
        XLaunchNewImage( NULL, NULL );
    }
}


HRESULT 
ParseTestParams(
	CHAR*				lpszBuffer,
	RSTESTPARAMS*		pTestParams,
	LPVOID				lpUserParam
)
{
	HRESULT hr = S_OK;

	if ( NULL == lpszBuffer || NULL == pTestParams )
	{
		DbgPrint( "Bad argument to ParseTestParams\n" );
		goto error;
	}

	SetDefaults( pTestParams );

	//------------------------------------------------------------------------------
	//	Output buffer to .repro file
	//------------------------------------------------------------------------------
	CHAR	szFileName[MAX_PATH];
	CHAR	szPath[MAX_PATH] = "D:\\";

	if ( TRUE == g_bHVSLaunched )
	{
		strcat( szPath, "tests\\" );
	}

	wcstombs( szFileName, XeImageHeader()->Certificate->TitleName, wcslen( XeImageHeader()->Certificate->TitleName ) + 1 );
	strcat( szPath, szFileName );
	strcat( szPath, ".repro" );

	HANDLE hFile = CreateFile( szPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		DbgPrint( "Unable to create .repro file '%s' (error %x)\n", szPath, GetLastError() );
		goto error;
	}

	DWORD	dwSize, dwNumBytesWritten = 0;
	BOOL	bSuccess = FALSE;
	dwSize = strlen( lpszBuffer );
	bSuccess = WriteFile( hFile, lpszBuffer, dwSize, &dwNumBytesWritten, NULL );
	CloseHandle( hFile );
	if ( FALSE == bSuccess || dwSize != dwNumBytesWritten )
	{
		DbgPrint( "Unable to write .repro file '%s' (error %x)\n", szPath, GetLastError() );
		goto error;
	}

	//------------------------------------------------------------------------------
	//	Parse variables into struct and pass off test specific ones
	//------------------------------------------------------------------------------

	CHAR	szValue[256];
    CHAR	szVariable[64];
    CHAR*	lpszEnd;
	CHAR*	c;

    do
    {
        lpszEnd = strchr( lpszBuffer, '\n' );
        if ( NULL != lpszEnd )
		{
			*lpszEnd = '\0';
		}
		
        c = strchr( lpszBuffer, ';' );
        if ( NULL != c ) 
		{
			*c = '\0';
		}

        c = strchr( lpszBuffer, '[' );
        if ( NULL != c ) 
		{
			*c = '\0';
		}

        RightTrim( lpszBuffer );

		DWORD	dwFlag = 0;
		
        if( sscanf( lpszBuffer, "%63[^=]= %255[^\0]", szVariable, szValue ) == 2 )
		{
            // global settings
			//d3d
            if( strstr( szVariable, "g_D3D_BackBufferWidth") != NULL)							pTestParams->D3D_uiBackBufferWidth = GetNumber(szValue);
			else if( strstr( szVariable, "g_D3D_BackBufferHeight") != NULL)					pTestParams->D3D_uiBackBufferHeight = GetNumber(szValue);
			else if( strstr( szVariable, "g_D3D_BackBufferFormat") != NULL)						GetConst( szValue, 256, (PDWORD)&(pTestParams->D3D_BackBufferFormat) );
			else if( strstr( szVariable, "g_D3D_BackBufferCount") != NULL)					pTestParams->D3D_uiBackBufferCount = GetNumber(szValue);
			else if( strstr( szVariable, "g_D3D_SwapEffect") != NULL)							GetConst( szValue, 256, (PDWORD)&(pTestParams->D3D_BackBufferFormat) );
			else if( strstr( szVariable, "g_D3D_EnableAutoDepthStencil") != NULL)				pTestParams->D3D_bEnableAutoDepthStencil = GetNumber(szValue);
			else if( strstr( szVariable, "g_D3D_AutoDepthStencilFormat") != NULL)				GetConst( szValue, 256, (PDWORD)&(pTestParams->D3D_AutoDepthStencilFormat) );
			else if( strstr( szVariable, "g_D3D_RefreshRateInHz") != NULL)			pTestParams->D3D_uiFullScreen_RefreshRateInHz = GetNumber(szValue);
			else if( strstr( szVariable, "g_D3D_PresentationInterval") != NULL)	GetConst( szValue, 256, (PDWORD)&(pTestParams->D3D_bSingleStepPusher) );
			else if( strstr( szVariable, "g_D3D_SingleStepPusher") != NULL)					pTestParams->D3D_bSingleStepPusher = GetNumber(szValue);
			else if( strstr( szVariable, "g_D3D_Parser") != NULL)							pTestParams->D3D_bD3dParser = GetNumber(szValue);
			else if( strstr( szVariable, "g_D3D_NullHardware") != NULL)						pTestParams->D3D_bNullHardware = GetNumber(szValue);
			//dwflags
			else if( strstr( szVariable, "g_D3D_PresentFlag") != NULL){							GetConst( szValue, 256, &dwFlag ); pTestParams->D3D_dwFlags |= dwFlag; }
			else if( strstr( szVariable, "g_D3D_WidescreenMode") != NULL){						GetConst( szValue, 256, &dwFlag ); pTestParams->D3D_dwFlags |= dwFlag; }
			else if( strstr( szVariable, "g_D3D_10x11PixelAspectRatio") != NULL){				GetConst( szValue, 256, &dwFlag ); pTestParams->D3D_dwFlags |= dwFlag; }
			//multisampletype         
			else if( strstr( szVariable, "g_D3D_MultiSampleType") != NULL){						GetConst( szValue, 256, &dwFlag ); pTestParams->D3D_MultiSampleType |= dwFlag; }
			else if( strstr( szVariable, "g_D3D_PrefilterBufferFormat") != NULL){				GetConst( szValue, 256, &dwFlag ); pTestParams->D3D_MultiSampleType |= dwFlag; }

			//dmusic
			else if( strstr( szVariable, "g_DM_bDoWorkLocally") != NULL)		pTestParams->DM_bDoWorkLocally = GetNumber(szValue);
			else if( strstr( szVariable, "g_DM_dwDoWorkFrequency") != NULL)		pTestParams->DM_dwDoWorkFrequency = GetNumber(szValue);
			else if( strstr( szVariable, "g_DM_dwDebugLevel") != NULL)			pTestParams->DM_dwDebugLevel = GetNumber(szValue);
			else if( strstr( szVariable, "g_DM_dwRIPLevel") != NULL)			pTestParams->DM_dwRIPLevel = GetNumber(szValue);
			else if( strstr( szVariable, "g_DM_dwTestDebugLevel") != NULL)		pTestParams->DM_dwTestDebugLevel = GetNumber(szValue);
			else if( strstr( szVariable, "g_DM_dwVoiceCount") != NULL)			pTestParams->DM_dwVoiceCount = GetNumber(szValue);
			else if( strstr( szVariable, "g_DM_szDMHeap") != NULL)				strcpy( pTestParams->DM_szDMHeap, szValue );
			else if( strstr( szVariable, "g_DM_dwHeapSizeNorm") != NULL)		pTestParams->DM_dwHeapSizeNorm = GetNumber(szValue);
			else if( strstr( szVariable, "g_DM_dwHeapSizePhys") != NULL)		pTestParams->DM_dwHeapSizePhys = GetNumber(szValue);

			//dsound
			else if( strstr( szVariable, "g_DS_bEnableHeadphones") != NULL)		pTestParams->DS_bEnableHeadphones = GetNumber(szValue);
			else if( strstr( szVariable, "g_DS_szFXImage") != NULL)				strcpy( pTestParams->DS_szFXImage, szValue );
			else if( strstr( szVariable, "g_DS_szHRTF") != NULL)				strcpy( pTestParams->DS_szHRTF, szValue );

			//other
			else if( strstr( szVariable, "g_RS_szTestRun") != NULL)				strcpy( pTestParams->RS_szTestRun, szValue );
			else if( strstr( szVariable, "g_RS_szHttpServer") != NULL)			strcpy( pTestParams->RS_szHttpServer, szValue );
			else if( strstr( szVariable, "g_RS_szConfigPage") != NULL)			strcpy( pTestParams->RS_szConfigPage, szValue );
			else if( strstr( szVariable, "g_RS_szStatePage") != NULL)			strcpy( pTestParams->RS_szStatePage, szValue );
			else if( strstr( szVariable, "g_RS_szLogPage") != NULL)				strcpy( pTestParams->RS_szLogPage, szValue );
			else if( strstr( szVariable, "g_RS_szPostFileName") != NULL)		strcpy( pTestParams->RS_szPostFileName, szValue );
			else if( strstr( szVariable, "g_RS_bLogResults") != NULL)			pTestParams->RS_bLogResults = GetNumber(szValue);
			else if( strstr( szVariable, "g_RS_dwDecreaseMemory") != NULL)		pTestParams->RS_dwDecreaseMemory = GetNumber(szValue);
			else if( strstr( szVariable, "g_RS_dwDSUsedVoices2D") != NULL)		pTestParams->RS_dwDSUsedVoices2D = GetNumber(szValue);
			else if( strstr( szVariable, "g_RS_dwDSUsedVoices3D") != NULL)		pTestParams->RS_dwDSUsedVoices3D = GetNumber(szValue);
			else if( strstr( szVariable, "g_RS_dwMLErrorLevel") != NULL)		pTestParams->RS_dwMLErrorLevel = GetNumber(szValue);
			//core config settings
			//else if( strstr( szVariable, "g_RS_szResultsServer") != NULL)		strcpy( pTestParams->RS_szResultsServer, szValue );
			//else if( strstr( szVariable, "g_RS_dwStopAfter") != NULL)			pTestParams->RS_dwStopAfter = GetNumber(szValue);
			//else if( strstr( szVariable, "g_RS_szSaveName") != NULL)			strcpy( pTestParams->RS_szSaveName, szValue );
			
            // test specific settings
            else if ( NULL != g_pfTestFunc) (g_pfTestFunc)( szVariable, szValue, lpUserParam );
		}
		
        if ( lpszEnd ) 
		{ 
			lpszBuffer = lpszEnd+1;
		}

	} while( lpszEnd );

	return hr;

error:
	return E_FAIL;
}


VOID
SetDefaults( 
	RSTESTPARAMS*		pTestParams
)
{
	if ( NULL == pTestParams )
	{
		DbgPrint( "Bad argument to SetDefaults\n" );
		return;
	}

	ZeroMemory( pTestParams, sizeof( RSTESTPARAMS ) );
	/*pTestParams->D3D_uiBackBufferWidth	= 640;
	pTestParams->D3D_uiBackBufferHeight = 480;
	pTestParams->D3D_BackBufferFormat	= D3DFMT_LIN_A8R8G8B8;
	pTestParams->D3D_uiBackBufferCount	= 1;
	
	pTestParams->D3D_SwapEffect			= D3DSWAPEFFECT_DISCARD;
	
	pTestParams->D3D_AutoDepthStencilFormat = D3DFMT_LIN_D24S8;
						
	pTestParams->D3D_uiFullScreen_RefreshRateInHz = 60; 
	pTestParams->D3D_uiFullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			
	pTestParams->DM_dwDoWorkFrequency	= 60;				
	pTestParams->DM_dwDebugLevel		= 1;					
							
	pTestParams->DM_dwTestDebugLevel	= 2;				
	pTestParams->DM_dwVoiceCount		= 180;					
			
	pTestParams->DM_dwHeapSizeNorm		= 16777216;					
	pTestParams->DM_dwHeapSizePhys		= 16777216;	
	
	pTestParams->RS_dwMLErrorLevel		= 3;*/
	
	//pTestParams->D3D_MultiSampleType	= D3DMULTISAMPLE_NONE;
	//pTestParams->D3D_bEnableAutoDepthStencil;
	//pTestParams->D3D_dwFlags			= D3DPRESENTFLAG_INTERLACED;	
	//pTestParams->D3D_bSingleStepPusher;
	//pTestParams->D3D_bD3dParser;
	//pTestParams->D3D_bNullHardware;
	//pTestParams->DM_bDoWorkLocally;
	//pTestParams->DM_dwRIPLevel;
	//pTestParams->DM_szDMHeap[MAX_PATH];
	//pTestParams->DS_bEnableHeadphones;				
	//pTestParams->DS_szFXImage[MAX_PATH];				
	//pTestParams->DS_szHRTF[MAX_PATH];				
	//pTestParams->RS_szTestRun[MAX_PATH];				
	//pTestParams->RS_szHttpServer[MAX_PATH];			
	//pTestParams->RS_szConfigPage[MAX_PATH];			
	//pTestParams->RS_szStatePage[MAX_PATH];			
	//pTestParams->RS_szLogPage[MAX_PATH];				
	//pTestParams->RS_szPostFileName[MAX_PATH];		
	//pTestParams->RS_bLogResults;						
	//pTestParams->RS_dwDecreaseMemory;				
	//pTestParams->RS_dwDSUsedVoices2D;				
	//pTestParams->RS_dwDSUsedVoices3D;				
}			


BOOL _stdcall
GetConst(
	IN CONST CHAR*	szStr, 
	IN INT		cchStr, 
	OUT PDWORD	pval
)
{
    #undef XTAG
    #define XTAG(_tag) { _tag, #_tag }
    static const struct
    {
        INT Val;
        CONST CHAR *szStr;
    } rgszConsts[] =
    {
        // Pixel Shaders
        XTAG(PS_TEXTUREMODES_NONE), XTAG(PS_TEXTUREMODES_PROJECT2D), XTAG(PS_TEXTUREMODES_PROJECT3D),
        XTAG(PS_TEXTUREMODES_CUBEMAP), XTAG(PS_TEXTUREMODES_PASSTHRU), XTAG(PS_TEXTUREMODES_CLIPPLANE),
        XTAG(PS_TEXTUREMODES_BUMPENVMAP), XTAG(PS_TEXTUREMODES_BUMPENVMAP_LUM), XTAG(PS_TEXTUREMODES_BRDF),
        XTAG(PS_TEXTUREMODES_DOT_ST), XTAG(PS_TEXTUREMODES_DOT_ZW), XTAG(PS_TEXTUREMODES_DOT_RFLCT_DIFF),
        XTAG(PS_TEXTUREMODES_DOT_RFLCT_SPEC), XTAG(PS_TEXTUREMODES_DOT_STR_3D), XTAG(PS_TEXTUREMODES_DOT_STR_CUBE),
        XTAG(PS_TEXTUREMODES_DPNDNT_AR), XTAG(PS_TEXTUREMODES_DPNDNT_GB), XTAG(PS_TEXTUREMODES_DOTPRODUCT),
        XTAG(PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST), XTAG(PS_COMPAREMODE_S_LT), XTAG(PS_COMPAREMODE_S_GE),
        XTAG(PS_COMPAREMODE_T_LT), XTAG(PS_COMPAREMODE_T_GE), XTAG(PS_COMPAREMODE_R_LT),
        XTAG(PS_COMPAREMODE_R_GE), XTAG(PS_COMPAREMODE_Q_LT), XTAG(PS_COMPAREMODE_Q_GE),
        XTAG(PS_COMBINERCOUNT_MUX_LSB), XTAG(PS_COMBINERCOUNT_MUX_MSB), XTAG(PS_COMBINERCOUNT_SAME_C0),
        XTAG(PS_COMBINERCOUNT_UNIQUE_C0), XTAG(PS_COMBINERCOUNT_SAME_C1), XTAG(PS_COMBINERCOUNT_UNIQUE_C1),
        XTAG(PS_INPUTMAPPING_UNSIGNED_IDENTITY), XTAG(PS_INPUTMAPPING_UNSIGNED_INVERT), XTAG(PS_INPUTMAPPING_EXPAND_NORMAL),
        XTAG(PS_INPUTMAPPING_EXPAND_NEGATE), XTAG(PS_INPUTMAPPING_HALFBIAS_NORMAL), XTAG(PS_INPUTMAPPING_HALFBIAS_NEGATE),
        XTAG(PS_INPUTMAPPING_SIGNED_IDENTITY), XTAG(PS_INPUTMAPPING_SIGNED_NEGATE), XTAG(PS_REGISTER_ZERO),
        XTAG(PS_REGISTER_DISCARD), XTAG(PS_REGISTER_C0), XTAG(PS_REGISTER_C1),
        XTAG(PS_REGISTER_FOG), XTAG(PS_REGISTER_V0), XTAG(PS_REGISTER_V1),
        XTAG(PS_REGISTER_T0), XTAG(PS_REGISTER_T1), XTAG(PS_REGISTER_T2),
        XTAG(PS_REGISTER_T3), XTAG(PS_REGISTER_R0), XTAG(PS_REGISTER_R1),
        XTAG(PS_REGISTER_V1R0_SUM), XTAG(PS_REGISTER_EF_PROD), XTAG(PS_REGISTER_ONE),
        XTAG(PS_REGISTER_NEGATIVE_ONE), XTAG(PS_REGISTER_ONE_HALF), XTAG(PS_REGISTER_NEGATIVE_ONE_HALF),
        XTAG(PS_CHANNEL_RGB), XTAG(PS_CHANNEL_BLUE), XTAG(PS_CHANNEL_ALPHA),
        XTAG(PS_FINALCOMBINERSETTING_CLAMP_SUM), XTAG(PS_FINALCOMBINERSETTING_COMPLEMENT_V1), XTAG(PS_FINALCOMBINERSETTING_COMPLEMENT_R0),
        XTAG(PS_COMBINEROUTPUT_IDENTITY), XTAG(PS_COMBINEROUTPUT_BIAS), XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_1),
        XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS), XTAG(PS_COMBINEROUTPUT_SHIFTLEFT_2), XTAG(PS_COMBINEROUTPUT_SHIFTRIGHT_1),
        XTAG(PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA), XTAG(PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA), XTAG(PS_COMBINEROUTPUT_AB_MULTIPLY),
        XTAG(PS_COMBINEROUTPUT_AB_DOT_PRODUCT), XTAG(PS_COMBINEROUTPUT_CD_MULTIPLY), XTAG(PS_COMBINEROUTPUT_CD_DOT_PRODUCT),
        XTAG(PS_COMBINEROUTPUT_AB_CD_SUM), XTAG(PS_COMBINEROUTPUT_AB_CD_MUX),
        XTAG(PS_DOTMAPPING_ZERO_TO_ONE), XTAG(PS_DOTMAPPING_MINUS1_TO_1_D3D), XTAG(PS_DOTMAPPING_MINUS1_TO_1_GL),
        XTAG(PS_DOTMAPPING_MINUS1_TO_1), XTAG(PS_DOTMAPPING_HILO_1), XTAG(PS_DOTMAPPING_HILO_HEMISPHERE_D3D), XTAG(PS_DOTMAPPING_HILO_HEMISPHERE_GL),
        XTAG(PS_DOTMAPPING_HILO_HEMISPHERE),

        // D3DFORMATs
        XTAG(D3DFMT_A8R8G8B8), XTAG(D3DFMT_X8R8G8B8), XTAG(D3DFMT_R5G6B5), XTAG(D3DFMT_R6G5B5),
        XTAG(D3DFMT_X1R5G5B5), XTAG(D3DFMT_A1R5G5B5), XTAG(D3DFMT_A4R4G4B4), XTAG(D3DFMT_A8),
        XTAG(D3DFMT_A8B8G8R8), XTAG(D3DFMT_B8G8R8A8), XTAG(D3DFMT_R4G4B4A4), XTAG(D3DFMT_R5G5B5A1),
        XTAG(D3DFMT_R8G8B8A8), XTAG(D3DFMT_R8B8), XTAG(D3DFMT_G8B8), XTAG(D3DFMT_P8),
        XTAG(D3DFMT_L8), XTAG(D3DFMT_A8L8), XTAG(D3DFMT_AL8), XTAG(D3DFMT_L16),
        XTAG(D3DFMT_V8U8), XTAG(D3DFMT_L6V5U5), XTAG(D3DFMT_X8L8V8U8), XTAG(D3DFMT_Q8W8V8U8),
        XTAG(D3DFMT_V16U16), XTAG(D3DFMT_D16_LOCKABLE), XTAG(D3DFMT_D16), XTAG(D3DFMT_D24S8),
        XTAG(D3DFMT_F16), XTAG(D3DFMT_F24S8), XTAG(D3DFMT_UYVY), XTAG(D3DFMT_YUY2),
        XTAG(D3DFMT_DXT1), XTAG(D3DFMT_DXT2), XTAG(D3DFMT_DXT3), XTAG(D3DFMT_DXT4),
        XTAG(D3DFMT_DXT5), XTAG(D3DFMT_LIN_A1R5G5B5), XTAG(D3DFMT_LIN_A4R4G4B4), XTAG(D3DFMT_LIN_A8),
        XTAG(D3DFMT_LIN_A8B8G8R8), XTAG(D3DFMT_LIN_A8R8G8B8), XTAG(D3DFMT_LIN_B8G8R8A8), XTAG(D3DFMT_LIN_G8B8),
        XTAG(D3DFMT_LIN_R4G4B4A4), XTAG(D3DFMT_LIN_R5G5B5A1), XTAG(D3DFMT_LIN_R5G6B5), XTAG(D3DFMT_LIN_R6G5B5),
        XTAG(D3DFMT_LIN_R8B8), XTAG(D3DFMT_LIN_R8G8B8A8), XTAG(D3DFMT_LIN_X1R5G5B5), XTAG(D3DFMT_LIN_X8R8G8B8),
        XTAG(D3DFMT_LIN_A8L8), XTAG(D3DFMT_LIN_AL8), XTAG(D3DFMT_LIN_L16), XTAG(D3DFMT_LIN_L8),
        XTAG(D3DFMT_LIN_D24S8), XTAG(D3DFMT_LIN_F24S8), XTAG(D3DFMT_LIN_D16), XTAG(D3DFMT_LIN_F16),

		// D3DSWAPEFFECT
		XTAG(D3DSWAPEFFECT_DISCARD), XTAG(D3DSWAPEFFECT_FLIP), XTAG( D3DSWAPEFFECT_COPY),

		// D3DPRESENTFLAGs
		XTAG(D3DPRESENTFLAG_LOCKABLE_BACKBUFFER), XTAG(D3DPRESENTFLAG_WIDESCREEN),
		XTAG(D3DPRESENTFLAG_INTERLACED), XTAG(D3DPRESENTFLAG_PROGRESSIVE),
		XTAG(D3DPRESENTFLAG_FIELD), XTAG(D3DPRESENTFLAG_10X11PIXELASPECTRATIO),

		// D3DMULTISAMPLE_TYPEs
        XTAG(D3DMULTISAMPLE_NONE), XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8),

        XTAG(D3DCLEAR_TARGET), XTAG(D3DCLEAR_ZBUFFER), XTAG(D3DCLEAR_STENCIL),

        // FVFs
        XTAG(D3DFVF_XYZ), XTAG(D3DFVF_XYZRHW), XTAG(D3DFVF_NORMAL), XTAG(D3DFVF_DIFFUSE),
        XTAG(D3DFVF_SPECULAR), XTAG(D3DFVF_TEX0), XTAG(D3DFVF_TEX1), XTAG(D3DFVF_TEX2),
        XTAG(D3DFVF_TEX3), XTAG(D3DFVF_TEX4),

        // textcoord sizes
        /*XTAG(T0_SIZE1), XTAG(T0_SIZE2), XTAG(T0_SIZE3), XTAG(T0_SIZE4),
        XTAG(T1_SIZE1), XTAG(T1_SIZE2), XTAG(T1_SIZE3), XTAG(T1_SIZE4),
        XTAG(T2_SIZE1), XTAG(T2_SIZE2), XTAG(T2_SIZE3), XTAG(T2_SIZE4),
        XTAG(T3_SIZE1), XTAG(T3_SIZE2), XTAG(T3_SIZE3), XTAG(T3_SIZE4),*/

        // D3DCMPs
        XTAG(D3DCMP_NEVER), XTAG(D3DCMP_LESS), XTAG(D3DCMP_EQUAL), XTAG(D3DCMP_LESSEQUAL),
        XTAG(D3DCMP_GREATER), XTAG(D3DCMP_NOTEQUAL), XTAG(D3DCMP_GREATEREQUAL), XTAG(D3DCMP_ALWAYS),

		// STENCILOPs
		XTAG(D3DSTENCILOP_KEEP),
		XTAG(D3DSTENCILOP_ZERO),
		XTAG(D3DSTENCILOP_REPLACE),
		XTAG(D3DSTENCILOP_INCRSAT),
		XTAG(D3DSTENCILOP_DECRSAT),
		XTAG(D3DSTENCILOP_INVERT),
		XTAG(D3DSTENCILOP_INCR),
		XTAG(D3DSTENCILOP_DECR),

		// D3DZBUFFERTYPE
		XTAG(D3DZB_TRUE), XTAG(D3DZB_USEW), XTAG(D3DZB_FALSE),

        XTAG(D3DTEXF_NONE), XTAG(D3DTEXF_POINT), XTAG(D3DTEXF_LINEAR), XTAG(D3DTEXF_ANISOTROPIC),
        XTAG(D3DTEXF_QUINCUNX), XTAG(D3DTEXF_GAUSSIANCUBIC),

//        XTAG(TEX_None), XTAG(TEX_2d), XTAG(TEX_Cubemap), XTAG(TEX_Volume),

 //       XTAG(TIME_Present), XTAG(TIME_Render),

        // swaths
        XTAG(D3DSWATH_8), XTAG(D3DSWATH_16), XTAG(D3DSWATH_32),
        XTAG(D3DSWATH_64), XTAG(D3DSWATH_128), XTAG(D3DSWATH_OFF),

        XTAG(D3DCOLORWRITEENABLE_RED), XTAG(D3DCOLORWRITEENABLE_GREEN), XTAG(D3DCOLORWRITEENABLE_BLUE),
        XTAG(D3DCOLORWRITEENABLE_ALPHA), XTAG(D3DCOLORWRITEENABLE_ALL),

		// File Creation Flags
		XTAG(CREATE_NEW), XTAG(CREATE_ALWAYS), XTAG(OPEN_EXISTING), XTAG(OPEN_ALWAYS), XTAG(TRUNCATE_EXISTING),

		// File Copy Flags
//		XTAG(COPY_IF_NOT_EXIST), XTAG(COPY_IF_NEWER), XTAG(COPY_ALWAYS),

        // misc
        XTAG(FALSE), XTAG(TRUE)
    };

    for(int ifmt = 0; ifmt < ARRAYSIZE(rgszConsts); ifmt++)
    {
        if(!_strnicmp(rgszConsts[ifmt].szStr, szStr, cchStr))
        {
            // set val
            *pval = (DWORD)rgszConsts[ifmt].Val;
            return true;
        }
    }

    return false;
}	