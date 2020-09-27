/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvMemTest.cpp

Abstract:

	General MEM tests including parameter tests.

Author:

	Jeff Sullivan (jeffsul) 13-Sep-2001

Revision History:

	13-Sep-2001 jeffsul
		Initial Version

--*/

#include "WmvMemoryTest.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xgmath.h>
#include <medialoader.h>

#include <stdio.h>
#include <tchar.h>

#define WMVTEST_APP_NAME	"WMVTEST"
#define	WMVTEST_INI_PATH	"D:\\WMVTest.ini"

/*++

Routine Description:

	Test constructor

Arguments:

	None

Return Value:

	None

--*/
CWmvMemTest::CWmvMemTest()
{
}


/*++

Routine Description:

	Test destructor

Arguments:

	None

Return Value:

	None

--*/
CWmvMemTest::~CWmvMemTest()
{
}


/*++

Routine Description:

	read initialization variables from an ini file

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvMemTest::ReadIni()
{
	HRESULT hr = S_OK;

	
	MLSetErrorLevel( 1 );
#define CREATE_FLAG	MLCOPY_IFNEWER
	MEDIAFILEDESC MediaFiles[] =
	{
		{"wmv/ms_wmv_v8/(\\w*).(\\w*)%1",	"T:\\WMV\\Mem\\",			CREATE_FLAG},
		{NULL,									NULL,								0}
	};
	EXECUTE( LoadMedia( MediaFiles ) );

	// get initial memory status before tests
	DWORD				dwAvailPhys			= 0;
	MEMORYSTATUS		MemStatus			= {0};

	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	dwAvailPhys = MemStatus.dwAvailPhys;

	// check memory under poor conditions
	LPVOID	pvBuffer = NULL;
	DWORD	dwMemSize = WMV_MEM_TEST_LARGE_MEMORY_SIZE;
	while ( NULL == pvBuffer )
	{
		pvBuffer = malloc( dwMemSize );
		dwMemSize = dwMemSize >> 1;
	}

	ExecuteMemoryChecks();
	free( pvBuffer );

	// check memory under optimal conditions
	ExecuteMemoryChecks();

	// get memory status after all tests have been done
	Sleep( 5000 );
	GlobalMemoryStatus( &MemStatus );
	DbgPrint( "[WmvMemoryTest] info: available memory\n\tinitial = %d\n\tending  = %d\n\tdifference = %d\n", MemStatus.dwAvailPhys, dwAvailPhys, dwAvailPhys-MemStatus.dwAvailPhys );

	m_bQuit = TRUE;

	return hr;
}

VOID
CWmvMemTest::ExecuteMemoryChecks()
{
	HRESULT				hr					= S_OK;
	HRESULT				hrExpected			= S_OK;
	LPWMVDECODER		pWmvDecoder			= NULL;
	LPWMVVIDEOINFO		pWmvVideoInfo		= NULL;
	MEMORYSTATUS		MemStatus			= {0};
	CHAR				szBadFileName[]		= "test.file";
	CHAR				szFileName[MAX_PATH];//= "T:\\WMV\\test.asf";
	INT					nTest				= 64;
	DWORD				dwInitAvailPhys		= 0;
	DWORD				dwAvailPhys 		= 0;
	UINT				i					= 0;


	WIN32_FIND_DATA	FindData;
	HANDLE hFiles = FindFirstFile( "T:\\WMV\\Mem\\*.*", &FindData );
	if ( INVALID_HANDLE_VALUE == hFiles )
	{
		DbgPrint( "[WmvMemoryTest] error: no files to open.\n" );
		return;
	}
	sprintf( szFileName, "T:\\WMV\\Mem\\%s", FindData.cFileName );

	//------------------------------------------------------------------------------
	//	see how WmvCreateDecoder()/Release() affect memory
	//------------------------------------------------------------------------------

	// get current memory status
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	dwInitAvailPhys = dwAvailPhys  = MemStatus.dwAvailPhys;
	DbgPrint( "[WmvMemoryTest] info: initial available memory = %d\n", MemStatus.dwAvailPhys );

	// run WmvCreateDecoder()/Release()
	DbgPrint( "[WmvMemoryTest] info: testing WmvCreateDecoder()/Release()...\n" );
	for ( i=0; i < 10*WMV_MEM_TEST_NUM_TESTS; i++ )
	{
		hr = WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder );
		if ( FAILED( hr ) )
		{
			DbgPrint( "[WmvMemoryTest] error: WmvCreateDecoder failed (error %d)\n", hr );
		}
		RELEASE( pWmvDecoder );
	}

	// compare memory
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[WmvMemoryTest] warning: Possible leak in WmvCreateDecoder.  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, i );
	}

	//------------------------------------------------------------------------------
	//	create a decoder to test other functions
	//------------------------------------------------------------------------------

	hr = WmvCreateDecoder( szFileName, NULL, WMVVIDEOFORMAT_YUY2, NULL, &pWmvDecoder );
	if ( FAILED( hr ) )
	{
		DbgPrint( "[WmvMemoryTest] error: WmvCreateDecoder failed (error %d)\n", hr );
		DbgPrint( "[WmvMemoryTest] error: Unable to test Get... functions\n" );
		return;
	}

	XMEDIAINFO		Info;
	WAVEFORMATEX	AudioInfo;
	WMVVIDEOINFO	VideoInfo;
	REFERENCE_TIME	rtPlayDuration, rtPreroll;
	DWORD			dwStatus;

	//------------------------------------------------------------------------------
	//	see how Get...() functions affect memory
	//------------------------------------------------------------------------------

	// get current memory status
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	dwAvailPhys  = MemStatus.dwAvailPhys;

	// run Get...() functions
	DbgPrint( "[WmvMemoryTest] info: testing Get...()...\n" );
	for ( i=0; i < WMV_MEM_TEST_NUM_TESTS; i++ )
	{
		pWmvDecoder->GetAudioInfo( &AudioInfo );
		pWmvDecoder->GetVideoInfo( &VideoInfo );
		pWmvDecoder->GetPlayDuration( &rtPlayDuration, &rtPreroll );
		pWmvDecoder->GetInfo( &Info );
		pWmvDecoder->GetStatus( &dwStatus );
	}

	// compare memory
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[WmvMemoryTest] warning: Possible leak in IWMVDecoder::Get... functions.  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, WMV_MEM_TEST_NUM_TESTS );
	}

	//------------------------------------------------------------------------------
	//	create buffers for decoding tests
	//------------------------------------------------------------------------------

	XMEDIAPACKET	VideoOutputPacket, AudioOutputPacket;
	DWORD			dwVideoStatus, dwAudioStatus;
	DWORD			dwVideoCompletedSize, dwAudioCompletedSize;
	LPVOID			pvVideoBuffer, pvAudioBuffer;
	REFERENCE_TIME	rtVideo, rtAudio;
	DWORD			dwMaxVideoSize = VideoInfo.dwWidth * VideoInfo.dwHeight * VideoInfo.dwOutputBitsPerPixel / 8;
	
	pvVideoBuffer = malloc( dwMaxVideoSize );
	if ( NULL == pvVideoBuffer )
	{
		DbgPrint( "[WmvMemoryTest] error: Unable to allocate video buffer\n" );
		RELEASE( pWmvDecoder );
		return;
	}
	pvAudioBuffer = malloc( WMV_MEM_TEST_PACKET_SIZE );
	if ( NULL == pvAudioBuffer )
	{
		DbgPrint( "[WmvMemoryTest] error: Unable to allocate audio buffer\n" );
		free( pvVideoBuffer );
		RELEASE( pWmvDecoder );
		return;
	}


	VideoOutputPacket.pdwStatus = &dwVideoStatus;
	AudioOutputPacket.pdwStatus = &dwAudioStatus;
	VideoOutputPacket.pdwCompletedSize = &dwVideoCompletedSize;
	AudioOutputPacket.pdwCompletedSize = &dwAudioCompletedSize;
	VideoOutputPacket.pvBuffer = pvVideoBuffer;
	AudioOutputPacket.pvBuffer = pvAudioBuffer;
	AudioOutputPacket.dwMaxSize = WMV_MEM_TEST_PACKET_SIZE;
	VideoOutputPacket.dwMaxSize = dwMaxVideoSize;
	VideoOutputPacket.prtTimestamp = &rtVideo;
	AudioOutputPacket.prtTimestamp = &rtAudio;

	//------------------------------------------------------------------------------
	//	see how ProcessMultiple()/Flush() affect memory
	//------------------------------------------------------------------------------

	CheckProcessMultiple( pWmvDecoder, &VideoOutputPacket, &AudioOutputPacket, 250, FALSE );
	CheckProcessMultiple( pWmvDecoder, &VideoOutputPacket, &AudioOutputPacket, 100, TRUE );

	free( pvVideoBuffer );
	free( pvAudioBuffer );

	//------------------------------------------------------------------------------
	//	see how AddRef()/Release affects memory/refcount
	//------------------------------------------------------------------------------
	
	DWORD		dwRefCount;

	// get current memory status
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	dwAvailPhys  = MemStatus.dwAvailPhys;

	// run AddRef()/Release()
	pWmvDecoder->AddRef();
	dwRefCount = pWmvDecoder->Release();
	if ( 1 != dwRefCount )
	{
		DbgPrint( "[WmvMemoryTest] warning: Unexpected RefCount: %d\n", dwRefCount );
	}
	
	for ( i=0; i < WMV_MEM_TEST_NUM_TESTS; i++ )
	{
		pWmvDecoder->AddRef();
	}
	for ( i=0; i < WMV_MEM_TEST_NUM_TESTS; i++ )
	{
		pWmvDecoder->Release();
	}

	// compare memory
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[WmvMemoryTest] warning: Possible leak in IWMVDecoder::AddRef()/Release() functions.  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, WMV_MEM_TEST_NUM_TESTS );
	}

	//------------------------------------------------------------------------------
	//	Compare overall memory usage during this run
	//------------------------------------------------------------------------------

	dwRefCount = pWmvDecoder->Release();
	if ( 0 != dwRefCount )
	{
		DbgPrint( "[WmvMemoryTest] warning: Unexpected RefCount: %d\n", dwRefCount );
	}

	// check overall memory usage
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	DbgPrint( "[WmvMemoryTest] info: available memory\n\tinitial = %d\n\tending  = %d\n\tdifference = %d\n", MemStatus.dwAvailPhys, dwInitAvailPhys, dwInitAvailPhys-MemStatus.dwAvailPhys );
}


VOID 
CWmvMemTest::CheckProcessMultiple( 
	LPWMVDECODER	pWmvDecoder,
	LPCXMEDIAPACKET pVideoOutputPacket,
    LPCXMEDIAPACKET pAudioOutputPacket,
	UINT			nNumRuns, 
	BOOL			bDecodeEntire 
)
{
	HRESULT				hr					= DS_OK;
	MEMORYSTATUS		MemStatus			= {0};
	DWORD				dwAvailPhys 		= 0;

	// get current memory status
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	dwAvailPhys  = MemStatus.dwAvailPhys;

	// run ProcessMultiple()/Flush()
	for ( UINT i=0; i < nNumRuns; i++ )
	{
		DbgPrint( "[WmvMemoryTest] info: testing ProcessMultiple()... (run %d/%d)\n", i, nNumRuns );
		hr = DS_OK;
		while ( DS_OK == hr )
		{
			hr = pWmvDecoder->ProcessMultiple( pVideoOutputPacket, pAudioOutputPacket );
			if ( FALSE == bDecodeEntire && rand()%100 > 97 )
			{
				DbgPrint( "[WmvMemoryTest] info: ending stream early (run %d/%d)\n", i, nNumRuns );
				hr = S_FALSE;
			}
		}

		if ( S_FALSE != hr )
		{
			DbgPrint( "[WmvMemoryTest] warning: ProcessMultiple() failed (error %d) after %d tests\n", hr, i );
		}

		hr = pWmvDecoder->Flush();
		if ( FAILED( hr ) )
		{
			DbgPrint( "[WmvMemoryTest] warning: Flush() failed (error %x) after %d tests\n", hr, i );
			break;
		}
	}

	// compare memory
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[WmvMemoryTest] warning: Possible leak in IWMVDecoder::ProcessMultiple()/Flush().  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, i );
	}
}