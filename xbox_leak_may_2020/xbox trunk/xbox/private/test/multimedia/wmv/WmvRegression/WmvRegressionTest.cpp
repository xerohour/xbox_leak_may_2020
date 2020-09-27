/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvRegressionTest.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 28-Dec-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	28-Dec-2001 jeffsul
		Initial Version

--*/


#include "WmvRegressionTest.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xgmath.h>
#include <medialoader.h>

#include <stdio.h>
#include <tchar.h>

//#define	WMV_REG_GENERATE_CRC

CWmvRegTest::CWmvRegTest()
{
	m_bReset = FALSE;
	m_hOutputFile = INVALID_HANDLE_VALUE;
	m_hFiles = INVALID_HANDLE_VALUE;

	// Initialize the CRC table
	INT	i, j;
	DWORD dwCRC;
    for (i = 0; i < 256; i++) 
	{
        dwCRC = i;
        for (j = 0; j < 8; j++) 
		{
            dwCRC = (dwCRC & 1) ? ((dwCRC >> 1) ^ 0xEDB88320L) : (dwCRC >> 1);
        }
        m_dwCRCTable[i] = dwCRC;
    }
}


CWmvRegTest::~CWmvRegTest()
{
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::ReadIni()
{
	HRESULT		hr			= S_OK;

	CHAR	szSearchPath[MAX_PATH];
	
	//Initialize our network.
	// initialize the Xbox Secure Network Library (SNL).
	if ( 0 == XNetAddRef() )
	{
		DbgPrint( "[WMVREG] Error: Unable to initialize Xbox SNL (error %d)\n", nSuccess );
		return E_FAIL;
	}

	// grab media files from server
	MLSetErrorLevel( 3 );
#define CREATE_FLAG	COPY_IF_NEWER
	MEDIAFILEDESC MediaFiles[] =
	{
		{"wmv/ms_wmv_v8/%3",	"T:\\WMV\\Reg\\",			CREATE_FLAG},
		{NULL,									NULL,								0}
	};
	EXECUTE( LoadMedia( MediaFiles ) );

	// look for files on the xbox matching certain parameters
	strcpy( m_szPath, "T:\\WMV\\Reg\\" );
	sprintf( szSearchPath, "%s*.wmv", m_szPath );
	
	m_hFiles = FindFirstFile( szSearchPath, &m_FindData );
	if ( INVALID_HANDLE_VALUE == m_hFiles )
	{
		return E_FAIL;
	}

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::InitGraphics()
{
	HRESULT		hr			= S_OK;

	hr = InitD3DDevice( &m_pD3DDevice );

	m_pD3DDevice->EnableOverlay( TRUE );

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::DestroyGraphics()
{
	HRESULT		hr			= S_OK;

	RELEASE( m_pD3DDevice );

	// call xnetcleanup here so it only happens one time
	XNetRelease();

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::InitAudio()
{
	HRESULT		hr			= S_OK;

	hr = DirectSoundCreate( NULL, &m_pDSound, NULL );
	if ( FAILED( hr ) )
	{
		DbgPrint( "Unable to create Direct Sound (error: %x)\n", hr );
		return hr;
	}

	hr = m_WMVPlayer.Initialize( m_pD3DDevice, m_pDSound );
	if ( FAILED( hr ) )
	{
		DbgPrint( "Unable to initialize WMVPlayer (error: %x)\n", hr );
		return hr;
	}

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::DestroyAudio()
{
	HRESULT		hr			= S_OK;

	RELEASE( m_pDSound );

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::InitResources()
{
	HRESULT		hr			= S_OK;

	m_rtStartTime = 0;
	m_nChecksumIndex = 0;

	// Create the video decoder.  We only load
    // the section containing the decoder on demand
    XLoadSection("WMVDEC");
	CHAR	szFileName[MAX_PATH];
	sprintf( szFileName, "%s%s", m_szPath, m_FindData.cFileName );

	hr = m_WMVPlayer.OpenFile( szFileName );
	if ( FAILED( hr ) )
	{
		DbgPrint( "Unable to create WMVPlayer for '%s' (error: %x)\n", szFileName, hr );
		return E_FAIL;
	}

    m_WMVPlayer.GetVideoInfo( &m_wmvVideoInfo );

	// Create two textures based off the video format.  We need to
    // use two textures because the overlay surface reads directly
    // from the texture.  Updating a texture while it's being used
    // by the overlay will cause tearing
    for( int i = 0; i < 2; i++ )
    {
        m_pD3DDevice->CreateTexture( m_wmvVideoInfo.dwWidth,
                                     m_wmvVideoInfo.dwHeight,
                                     1,
                                     0,
                                     D3DFMT_YUY2,
                                     0,
                                     &m_pOverlay[i] );
        m_pOverlay[i]->GetSurfaceLevel( 0, &m_pSurface[i] );
    }
    m_dwCurrent = 0;

	CHAR	szCrcFileName[MAX_PATH];
	LPSTR	lpszExtension;

	strcpy( szCrcFileName, szFileName );
	lpszExtension = strchr( szCrcFileName, '.' );
	strcpy( lpszExtension, ".crc" );

#ifdef WMV_REG_GENERATE_CRC
	m_hOutputFile = CreateFile( szCrcFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == m_hOutputFile )
	{
		return E_FAIL;
	}
#else

	// grab media files from server
	CHAR	szRemotePath[MAX_PATH];
	MLSetErrorLevel( 3 );
	MEDIAFILEDESC MediaFiles[] =
	{
		{szRemotePath,	"T:\\WMV\\Reg\\",			CREATE_FLAG},
		{NULL,									NULL,								0}
	};
	sprintf( szRemotePath, "wmv/crc/%s", m_FindData.cFileName );
	lpszExtension = strchr( szRemotePath, '.' );
	strcpy( lpszExtension, ".crc" );
	LoadMedia( MediaFiles );

	m_hInputFile = CreateFile( szCrcFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == m_hInputFile )
	{
		DbgPrint( "[WMVREG]: Unable to open input file %s (error: %x)\n", szCrcFileName, GetLastError() );
		return E_FAIL;
	}
#endif

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::DestroyResources()
{
	HRESULT		hr			= S_OK;

	for( UINT i = 0; i < 2; i++ )
    {
        RELEASE( m_pSurface[i] );
        RELEASE( m_pOverlay[i] );
    }
	m_WMVPlayer.CloseFile();

	XFreeSection("WMVDEC");

	if ( !FindNextFile( m_hFiles, &m_FindData ) )
	{
		CHAR	szSearchPath[MAX_PATH];

		sprintf( szSearchPath, "%s*.wmv", m_szPath );
		
		m_hFiles = FindFirstFile( szSearchPath, &m_FindData );
	}

	if ( INVALID_HANDLE_VALUE == m_hFiles )
	{
			m_bQuit = TRUE;
	}

	CloseHandle( m_hOutputFile );

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::ProcessInput()
{
	HRESULT		hr			= S_OK;

	XINPUT_STATE		XInputState;
	ZeroMemory(	&XInputState, sizeof(XINPUT_STATE) );

	hr = GetNextInputState( &m_hInputDevice, &XInputState );

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT CWmvRegTest::Update()
{
	HRESULT		hr			= S_OK;

	if ( m_bReset )
	{
		DestroyResources();
		if ( TRUE == m_bQuit )
		{
			return S_OK;
		}
		hr = InitResources();
		m_bReset = FALSE;
		//m_bGotData = FALSE;
		//g_dwVBlankCount = 0;

		if ( FAILED( hr ) )
		{
			m_bReset = TRUE;
			return hr;
		}
	}

	// Set our time for this frame:
	// Video Time = Current Time - Start Time + Seek Adjustment
	if ( 0 == m_rtStartTime )
	{
		m_pDSound->GetTime( &m_rtStartTime );
	}
	m_pDSound->GetTime( &m_rtCurrentTime );
	m_rtCurrentTime = m_rtCurrentTime - m_rtStartTime;

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
#define CLAMP( x, a, b ) \
if ( x < a ) x = a; \
else if ( x > b ) x = b;
HRESULT CWmvRegTest::Render()
{
	HRESULT		hr			= S_OK;

	RECT rcSrc = { 0, 0, m_wmvVideoInfo.dwWidth, m_wmvVideoInfo.dwHeight };
	RECT rcDest;
	rcDest.left		= 320-m_wmvVideoInfo.dwWidth / 2;
	CLAMP( rcDest.left, 0, 320 );
	rcDest.top		= 240-m_wmvVideoInfo.dwHeight / 2;
	CLAMP( rcDest.top, 0, 240 );
	rcDest.right	= 320+m_wmvVideoInfo.dwWidth / 2;
	CLAMP( rcDest.right, 320, 640 );
	rcDest.bottom	= 240+m_wmvVideoInfo.dwHeight / 2;
	CLAMP( rcDest.bottom, 240, 480 );

	// clear the rendering surface
	EXECUTE( m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL|D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0L ) );

	// begin the scene
	EXECUTE( m_pD3DDevice->BeginScene() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	// Get the next frame into our texture
	hr = m_WMVPlayer.GetTexture( m_pOverlay[m_dwCurrent] );
	if( FAILED( hr ) )
	{
		__asm int 3;
	}

	if ( m_WMVPlayer.IsReady() )
    {
		m_pD3DDevice->BlockUntilVerticalBlank();
		m_pD3DDevice->UpdateOverlay( m_pSurface[m_dwCurrent], &rcSrc, &rcDest, FALSE, 0 );

		// generate checksum for frame
		DWORD	dwChecksum;
		D3DLOCKED_RECT	LockedRect;
		D3DSURFACE_DESC SurfaceDesc;

		hr = m_pSurface[m_dwCurrent]->LockRect( &LockedRect, NULL, D3DLOCK_READONLY );
		if ( FAILED( hr ) )
		{
			__asm int 3;
		}

		hr = m_pSurface[m_dwCurrent]->GetDesc( &SurfaceDesc );
		if ( FAILED( hr ) )
		{
			__asm int 3;
		}
		
		dwChecksum = ComputeCRC32( LockedRect.pBits, SurfaceDesc.Size );

#ifdef WMV_REG_GENERATE_CRC
		m_dwChecksums[m_nChecksumIndex] = dwChecksum;

		m_nChecksumIndex++;
		if ( 512 <= m_nChecksumIndex )
		{
			DWORD	dwNumBytesWritten;
			WriteFile( m_hOutputFile, m_dwChecksums, sizeof( DWORD )*512, &dwNumBytesWritten, NULL );
			if ( 512*sizeof( DWORD ) != dwNumBytesWritten )
			{
				__asm int 3;
			}
			m_nChecksumIndex = 0;
		}
#else
		DWORD	dwNumBytesRead, dwCorrectChecksum;
		ReadFile( m_hInputFile, &dwCorrectChecksum, sizeof( DWORD ), &dwNumBytesRead, NULL );
		if ( sizeof( DWORD ) != dwNumBytesRead )
		{
			__asm int 3;
		}

		if ( dwChecksum != dwCorrectChecksum )
		{
			DbgPrint( "[WMVREG]: Checksum failed.  Expected %x, got %x.\n", dwCorrectChecksum, dwChecksum );
		}
#endif
		
		// Decode the next frame
		if( S_FALSE == m_WMVPlayer.DecodeNext() )
		{
			m_bReset = TRUE;
#ifdef WMV_REG_GENERATE_CRC
			DWORD	dwNumBytesWritten;
			WriteFile( m_hOutputFile, m_dwChecksums, sizeof( DWORD )*m_nChecksumIndex, &dwNumBytesWritten, NULL );
			if ( m_nChecksumIndex*sizeof( DWORD ) != dwNumBytesWritten )
			{
				__asm int 3;
			}
			m_nChecksumIndex = 0;
#endif
			return hr;
		}
		
		// We'll use the other texture next time
		m_dwCurrent ^= 1;
	}

	// render the scene to the surface
	EXECUTE( m_pD3DDevice->EndScene() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	// flip the surface to the screen
	EXECUTE( m_pD3DDevice->Present( NULL, NULL, NULL, NULL ) );

	return hr;
}


DWORD CWmvRegTest::ComputeCRC32( LPVOID pvData, UINT uNumBytes ) 
{
    LPBYTE pData = (LPBYTE)pvData;
    DWORD  dwCRC = 0;
    UINT   i;

    for (i = 0; i < uNumBytes; i++) {
        dwCRC = m_dwCRCTable[(dwCRC ^ pData[i]) & 0xFF] ^ (dwCRC >> 8);
    }

    return dwCRC;
}


BOOL CWmvRegTest::CompareCRC()
{
	BOOL		bSuccess		= FALSE;
	HANDLE		hFile			= INVALID_HANDLE_VALUE;

	return bSuccess;
}




