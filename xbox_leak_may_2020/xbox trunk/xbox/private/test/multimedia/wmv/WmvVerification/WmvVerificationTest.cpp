/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvVerificationTest.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 26-Sep-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	26-Sep-2001 jeffsul
		Initial Version

--*/


#include "WmvVerificationTest.h"

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

//BOOL g_bOverlay = TRUE;

/*++

Routine Description:

	Test constructor

Arguments:

	None

Return Value:

	None

--*/
CWmvVerificationTest::CWmvVerificationTest()
{
	m_pD3DDevice = NULL;
	m_pDSound = NULL;
	m_pWMVDecoder = NULL;
	m_bReset = FALSE;
}


/*++

Routine Description:

	Test destructor

Arguments:

	None

Return Value:

	None

--*/
CWmvVerificationTest::~CWmvVerificationTest()
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
HRESULT CWmvVerificationTest::ReadIni()
{
	HRESULT hr = S_OK;

	CHAR	szSearchPath[MAX_PATH];

	// grab media files from server
	MLSetErrorLevel( 3 );
	MEDIAFILEDESC MediaFiles[] =
	{
		{"wmv/dolphin(\\w*).wmv",	"T:\\WMV\\",			COPY_IF_NEWER},
		{NULL,									NULL,								0}
	};
	EXECUTE( LoadMedia( MediaFiles ) );

	// look for files on the xbox matching certain parameters
	GetIniString( WMVTEST_APP_NAME, "WMVFileDirectory", "T:\\WMV\\", m_szPath, MAX_PATH, WMVTEST_INI_PATH );

	sprintf( szSearchPath, "%sdolphin*.wmv", m_szPath );
	
	m_hFiles = FindFirstFile( szSearchPath, &m_FindData );
	if ( INVALID_HANDLE_VALUE == m_hFiles )
	{
		return E_FAIL;
	}

	// create a wide character file name (for output later to screen)
	for( UINT i = 0; i < strlen( m_FindData.cFileName ); i++ )
	{
		m_szWideFileName[i] = (WCHAR)m_FindData.cFileName[i];
	}
	m_szWideFileName[i] = 0;

	return hr;
}


/*++

Routine Description:

	initialize graphics rendering device

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::InitGraphics()
{
	HRESULT hr = S_OK;

	D3DPRESENT_PARAMETERS	d3dpp;

	// Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = NULL;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	EXECUTE( Direct3D_CreateDevice(	D3DADAPTER_DEFAULT, 
									D3DDEVTYPE_HAL,
									NULL, 
									D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									&d3dpp, 
									&m_pD3DDevice) );
    if ( FAILED ( hr ) )
	{
        return hr;
    }

	// Create a buffer for the text
    hr = m_pD3DDevice->CreateTexture(320, 240, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &m_pd3dtText);
    if (FAILED(hr)) {
        return hr;
    }

    // Clear the texture
	D3DLOCKED_RECT d3dlr;
    m_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
    for ( UINT i = 0; i < 240; i++) {
        memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
    }
    m_pd3dtText->UnlockRect(0);

    m_prText[0].x = 0.0f;
    m_prText[0].y = 480.0f;
    m_prText[0].u = 0.0f;
    m_prText[0].v = 240.0f;

    m_prText[1].x = 0.0f;
    m_prText[1].y = 0.0f;
    m_prText[1].u = 0.0f;
    m_prText[1].v = 0.0f;

    m_prText[2].x = 640.0f;
    m_prText[2].y = 0.0f;
    m_prText[2].u = 320.0f;
    m_prText[2].v = 0.0f;

    m_prText[3].x = 640.0f;
    m_prText[3].y = 480.0f;
    m_prText[3].u = 320.0f;
    m_prText[3].v = 240.0f;

    for (i = 0; i < 4; i++) {
        m_prText[i].z = 0.0f; //0.000001f;
        m_prText[i].rhw = 1.0f; ///1000000.0f;
    }

	return hr;
}


/*++

Routine Description:

	cleanup graphics rendering device

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::DestroyGraphics()
{
	HRESULT hr = S_OK;

	RELEASE( m_pD3DDevice );
	RELEASE( m_pd3dtText );

	return hr;
}


/*++

Routine Description:

	initialize audio device

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::InitAudio()
{
	HRESULT hr = S_OK;

	hr = DirectSoundCreate( NULL, &m_pDSound, NULL );

	return hr;
}


/*++

Routine Description:

	cleanup audio device

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::DestroyAudio()
{
	HRESULT hr = S_OK;

	RELEASE( m_pDSound );

	return hr;
}


/*++

Routine Description:

	initialize wmv resources

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::InitResources()
{
	HRESULT hr = S_OK;
	
	m_dwLastFPS = m_dwCurrentTime-1000;

	m_dwVideoFrames = 0;
	m_dwAudioFrames = 0;

	m_dwMaxDiffAudio = 
	m_dwMaxDiffAudioFrame =
	m_dwMaxStdDevAudioFrame =
	m_dwMaxDiffVideo =
	m_dwMaxDiffVideoFrame =
	m_dwMaxStdDevVideoFrame = 0;

	m_fMaxStdDevAudio = 
	m_fMaxStdDevVideo = 0.0;

	m_audStat = 
	m_audSize =
	m_vidStat =
	m_vidSize = 0;
	m_rtAudio = 
	m_rtVideo = 0;

	m_hRawFile =
	m_hPcmFile = NULL;


	ZeroMemory( &m_xmpAudio, sizeof( XMEDIAPACKET ) );
	ZeroMemory( &m_xmpVideo, sizeof( XMEDIAPACKET ) );

	//------------------------------------------------------------------------------
	//	Load the section containg the WMV decoder
	//------------------------------------------------------------------------------
    XLoadSection("WMVDEC");

	//------------------------------------------------------------------------------
	//	Set up file io
	//------------------------------------------------------------------------------

	CHAR	szFileName[MAX_PATH];
	sprintf( szFileName, "%s%s", m_szPath, m_FindData.cFileName );
	strcpy( m_szRawPath, szFileName );
	strcpy( m_szPcmPath, szFileName );
	LPSTR	lpszTemp;
	lpszTemp = strrchr( m_szRawPath, '.' );
	CHECKPTR( lpszTemp );
	*lpszTemp = '\0';
	lpszTemp = strrchr( m_szPcmPath, '.' );
	CHECKPTR( lpszTemp );
	strcpy( lpszTemp, ".pcm" );

	m_hPcmFile = CreateFile( m_szPcmPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == m_hPcmFile )
	{
		DbgPrint( "Unable to open %s for writing (error %d)\n", m_szPcmPath, GetLastError() );
		return E_FAIL;
	}

	//------------------------------------------------------------------------------
	//	Create the video decoder.
	//------------------------------------------------------------------------------

    hr = WmvCreateDecoder( szFileName,
							NULL,
							/*g_bOverlay ? */WMVVIDEOFORMAT_YUY2/* : WMVVIDEOFORMAT_RGB32*/,
							NULL,
							&m_pWMVDecoder );
	if ( FAILED( hr ) )
	{
		SAFECLOSEHANDLE( m_hPcmFile );
		DbgPrint( "Unable to create decoder for '%s' (error: %d)\n", szFileName, hr );
		return E_FAIL;
	}

	//------------------------------------------------------------------------------
	//	set up for decoding
	//------------------------------------------------------------------------------

	WMVVIDEOINFO			wmvVideoInfo;             // Video info
	WAVEFORMATEX			wmvAudioInfo;             // Audio format

	CHECKEXECUTE( m_pWMVDecoder->GetVideoInfo( &wmvVideoInfo ) );
	CHECKEXECUTE( m_pWMVDecoder->GetAudioInfo( &wmvAudioInfo ) );
	
	// Set up an audio packet
	m_xmpAudio.dwMaxSize = WMV_VER_TEST_PACKET_SIZE;
	m_xmpAudio.pdwStatus = &m_audStat;
	m_xmpAudio.pdwCompletedSize = &m_audSize;
	m_xmpAudio.prtTimestamp = &m_rtAudio;
	m_dwBitsPerSample = wmvAudioInfo.wBitsPerSample;

	m_xmpAudio.pvBuffer  = new BYTE[ WMV_VER_TEST_PACKET_SIZE ];
	if ( NULL == m_xmpAudio.pvBuffer )
	{
		DbgPrint("");
		return E_FAIL;
	}
	ZeroMemory( m_xmpAudio.pvBuffer, m_xmpAudio.dwMaxSize );
	
	// Set up a video packet
	m_xmpVideo.dwMaxSize = wmvVideoInfo.dwWidth * wmvVideoInfo.dwHeight * wmvVideoInfo.dwOutputBitsPerPixel / 8;
	m_xmpVideo.pdwStatus = &m_vidStat;
	m_xmpVideo.pdwCompletedSize = &m_vidSize;
	m_xmpVideo.prtTimestamp = &m_rtVideo;
	
	m_xmpVideo.pvBuffer  = new BYTE [m_xmpVideo.dwMaxSize];
	if ( NULL == m_xmpVideo.pvBuffer )
	{
		DbgPrint("");
		return E_FAIL;
	}
	ZeroMemory( m_xmpVideo.pvBuffer, m_xmpVideo.dwMaxSize );

	return hr;
}


/*++

Routine Description:

	cleanup wmv resources

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::DestroyResources()
{
	HRESULT hr = S_OK;
	
	RELEASE( m_pWMVDecoder );
	XFreeSection("WMVDEC");

	SAFECLOSEHANDLE( m_hPcmFile );

	if ( !FindNextFile( m_hFiles, &m_FindData ) )
	{
		m_bQuit = TRUE;
	}
	else
	{
		for( UINT i = 0; i < strlen( m_FindData.cFileName ); i++ )
		{
			m_szWideFileName[i] = (WCHAR)m_FindData.cFileName[i];
		}
		m_szWideFileName[i] = 0;
	}

	SAFEDELETE( m_xmpAudio.pvBuffer );
	SAFEDELETE( m_xmpVideo.pvBuffer );

	return hr;
}


/*++

Routine Description:

	update the scene each frame

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::Update()
{
	HRESULT hr = S_OK;

	if ( m_bReset )
	{
		HRESULT hrTemp = S_OK;
		// just checking discontuinuity to make sure it does what the doc says (just return DS_OK)
		hrTemp = m_pWMVDecoder->Discontinuity();
		if ( DS_OK != hrTemp )
		{
			DbgPrint( "IWMVDecoder::Discontinuity() returned %d, should always be DS_OK\n", hrTemp );
		}
		hrTemp = m_pWMVDecoder->Flush();
		if ( FAILED( hrTemp ) )
		{
			DbgPrint( "IWMVDecoder::Flush() failed (hr = %d)\n", hrTemp );
		}

		CHAR	szBuf[MAX_PATH];
		sprintf( szBuf, "Max Diff Pixel: %d (Frame: %d)\tMax Std Dev: %f (Frame: %d)\nMax Diff Sample: %d (Frame: %d)\tMax Std Dev: %f (Frame %d)",
				m_dwMaxDiffVideo, m_dwMaxDiffVideoFrame,
				m_fMaxStdDevVideo, m_dwMaxStdDevVideoFrame,
				m_dwMaxDiffAudio, m_dwMaxDiffAudioFrame,
				m_fMaxStdDevAudio, m_dwMaxStdDevAudioFrame );

		DbgPrint( szBuf );

		DestroyResources();
		if ( TRUE == m_bQuit )
		{
			return hr;
		}
		InitResources();
		m_bReset = FALSE;
	}
	
	// Process the decoder
	hr = m_pWMVDecoder->ProcessMultiple( &m_xmpVideo, &m_xmpAudio );
	if( S_FALSE == hr )
	{
		m_bReset = TRUE;
		//m_State = WMVTS_COMPARE;
	}
	
	// If we got a video frame dump it to a file
	if( m_vidSize > 0 )
	{
		CompareFrames();
		//OutputRawData( &m_xmpVideo );
		m_dwVideoFrames++;
		m_vidSize = 0;
	}
	
	// If we got an audio packet dump it to the file
	if( m_audSize > 0 )
	{
		CompareAudio();
		m_dwAudioFrames++;
		m_xmpAudio.dwMaxSize = m_audSize;
		m_audSize = 0;
		OutputPcmData( &m_xmpAudio );
	}
	
	// a second has gone by, lets update our onscreen text
	if ( 1000 < m_dwCurrentTime - m_dwLastFPS )
	{
		// Clear the texture
		D3DLOCKED_RECT d3dlr;
		m_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
		for ( UINT i = 0; i < 240; i++) {
			memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
		}
		m_pd3dtText->UnlockRect(0);
		
		LPDIRECT3DSURFACE8	pSurface = NULL;
		hr = m_pd3dtText->GetSurfaceLevel(0, &pSurface);
		if ( SUCCEEDED( hr ) /*&& m_bDrawText*/ )
		{
			WCHAR buffer[MAX_PATH];
			swprintf( buffer, L"Frame: %d", m_dwVideoFrames );
			
			m_BitFont.DrawText( pSurface, m_szWideFileName, 20, 15, 0, D3DCOLOR_XRGB( 255, 255, 255 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
			m_BitFont.DrawText( pSurface, buffer, 20, 25, 0, D3DCOLOR_XRGB( 255, 255, 255 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
		}
		RELEASE( pSurface );
		m_dwLastFPS = m_dwCurrentTime;
	}

	return hr;
}


/*++

Routine Description:

	render the scene to the screen

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvVerificationTest::Render()
{
	HRESULT hr = S_OK;
			
	// clear the rendering surface
	EXECUTE( m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL|D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0L ) );

	// begin the scene
	EXECUTE( m_pD3DDevice->BeginScene() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	// draw the geometry with the text on it
	m_pD3DDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pD3DDevice->SetTexture(0, m_pd3dtText);
	
	m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prText, sizeof(TVERTEX));
	
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


/*++

Routine Description:

	description-of-function

Arguments:

	LPCXMEDIAPACKET pVideoPacket

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT 
CWmvVerificationTest::OutputRawData( 
	IN LPCXMEDIAPACKET pVideoPacket
)
{
	HRESULT hr = S_OK;
	BOOL	bSuccess = FALSE;
	DWORD	dwNumBytesWritten = 0;

	CHAR	szRawFrameFile[MAX_PATH];
	sprintf( szRawFrameFile, "%s_%s_%d.xrw", m_szRawPath, /*g_bOverlay ? */"YUY2"/* : "RGB32"*/, m_dwVideoFrames );

	m_hRawFile = CreateFile( szRawFrameFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == m_hRawFile )
	{
		DbgPrint( "Unable to open %s for writing (error %d)\n", m_szRawPath, GetLastError() );
		return E_FAIL;
	}

	bSuccess = WriteFile( m_hRawFile, pVideoPacket->pvBuffer, pVideoPacket->dwMaxSize, &dwNumBytesWritten, NULL );
	if ( FALSE == bSuccess || pVideoPacket->dwMaxSize != dwNumBytesWritten )
	{
		DbgPrint( "Unable to ouput .xrw data correctly\n" );
		hr = E_FAIL;
	}

	SAFECLOSEHANDLE( m_hRawFile );

	return hr;
}


/*++

Routine Description:

	description-of-function

Arguments:

	LPCXMEDIAPACKET pAudioPacket

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT 
CWmvVerificationTest::OutputPcmData( 
	IN LPCXMEDIAPACKET pAudioPacket
)
{
	HRESULT hr = S_OK;
	BOOL	bSuccess = FALSE;
	DWORD	dwNumBytesWritten = 0;

	ASSERT( NULL != m_hPcmFile );
	ASSERT( INVALID_HANDLE_VALUE != m_hPcmFile );

	bSuccess = WriteFile( m_hPcmFile, pAudioPacket->pvBuffer, pAudioPacket->dwMaxSize, &dwNumBytesWritten, NULL );
	if ( FALSE == bSuccess || pAudioPacket->dwMaxSize != dwNumBytesWritten )
	{
		DbgPrint( "Unable to ouput .pcm data correctly\n" );
		hr = E_FAIL;
	}

	return hr;
}

#define SAFEDELETEARRAY( p )                                              \
{                                                                    \
	delete [] p;                                                        \
	p = NULL;                                                        \
}
/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
VOID
CWmvVerificationTest::CompareFrames()
{
	CHAR	szReferenceFrame[MAX_PATH];
	HANDLE	hReferenceFile;

	// not sure about this yet:
	sprintf( szReferenceFrame, "%s_%s_1_%d.raw", m_szRawPath, /*g_bOverlay ? */"YUY2"/* : "RGB32"*/, /*m_dwCurrentFrame*/m_dwVideoFrames );
	hReferenceFile = CreateFile( szReferenceFrame, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == hReferenceFile )
	{
		DbgPrint( "Ubable to open file %s (error %x)\n", szReferenceFrame, GetLastError() );
		return;
	}

	/*if ( g_bOverlay )
	{*/
		typedef struct {
			BYTE y0, u, y1, v;
		} YUY2MACROPIXEL;

		typedef struct {
			DOUBLE y0, u, y1, v;
		} YUY2STDDEV;

		BOOL	bSuccess;
		DWORD	dwBytesRead;
		DWORD	dwSize = m_xmpVideo.dwMaxSize;
		DWORD	dwNumPixels = dwSize / sizeof(YUY2MACROPIXEL);
		//DWORD	dwSize = dwNumPixels*sizeof(YUY2MACROPIXEL);

		YUY2MACROPIXEL*	decPixels;
		YUY2MACROPIXEL*	refPixels;
		YUY2MACROPIXEL	maxDiff = {0};
		YUY2STDDEV		stdDev = {0};

		refPixels = new YUY2MACROPIXEL [dwNumPixels];
		if ( NULL == refPixels )
		{
			SAFECLOSEHANDLE( hReferenceFile );
			return;
		}

		//ZeroMemory( decPixels, dwSize );
		ZeroMemory( refPixels, dwSize );
		decPixels = (YUY2MACROPIXEL*)m_xmpVideo.pvBuffer;

		bSuccess = ReadFile( hReferenceFile, refPixels, dwSize, &dwBytesRead, NULL );
		if ( FALSE == bSuccess || dwSize != dwBytesRead )
		{
			DbgPrint( "Unable to properly read %s. Expected %d bytes, got %d.\n", szReferenceFrame, dwSize, dwBytesRead );
			SAFECLOSEHANDLE( hReferenceFile );
			SAFEDELETEARRAY( refPixels );
			return;
		}

		SAFECLOSEHANDLE( hReferenceFile );

		for ( UINT i=0; i<dwNumPixels; i++ )
		{
			UINT nDiff;
			UINT nTotalDiff = 0;

			// get stats on y0
			nTotalDiff += nDiff = abs( decPixels[i].y0-refPixels[i].y0 );
			stdDev.y0 += pow( nDiff, 2 );
			if ( nDiff > maxDiff.y0 )
			{
				maxDiff.y0 = (BYTE)nDiff;
			}

			// get stats on u
			nTotalDiff += nDiff = abs( decPixels[i].u-refPixels[i].u );
			stdDev.u += pow( nDiff, 2 );
			if ( nDiff > maxDiff.u )
			{
				maxDiff.u = (BYTE)nDiff;
			}

			// get stats on y1
			nTotalDiff += nDiff = abs( decPixels[i].y1-refPixels[i].y1 );
			stdDev.y1 += pow( nDiff, 2 );
			if ( nDiff > maxDiff.y1 )
			{
				maxDiff.y1 = (BYTE)nDiff;
			}

			// get stats on v
			nTotalDiff += nDiff = abs( decPixels[i].v-refPixels[i].v );
			stdDev.v += pow( nDiff, 2 );
			if ( nDiff > maxDiff.v )
			{
				maxDiff.v = (BYTE)nDiff;
			}

			if ( nTotalDiff > m_dwMaxDiffVideo )
			{
				m_dwMaxDiffVideo = nTotalDiff;
				m_dwMaxDiffVideoFrame = m_dwVideoFrames;
			}
		}

		SAFEDELETEARRAY( refPixels );

		stdDev.y0 = sqrt( stdDev.y0 / dwNumPixels );
		stdDev.u  = sqrt( stdDev.u  / dwNumPixels );
		stdDev.y1 = sqrt( stdDev.y1 / dwNumPixels );
		stdDev.v  = sqrt( stdDev.v  / dwNumPixels );

		DOUBLE	fTotalStdDev = stdDev.y0 + stdDev.u + stdDev.y1 + stdDev.v;
		if ( fTotalStdDev > m_fMaxStdDevVideo )
		{
			m_fMaxStdDevVideo = fTotalStdDev;
			m_dwMaxStdDevVideoFrame = m_dwVideoFrames;
		}

		CHAR	szBuf[MAX_PATH];
		sprintf( szBuf, "Frame: %d\tMax Diff:\tStd Dev:\n\ty0:\t\t %d\t\t\t %f\n\tu:\t\t %d\t\t\t %f\n\ty1:\t\t %d\t\t\t %f\n\tv:\t\t %d\t\t\t %f\n\n",
				m_dwVideoFrames,
				maxDiff.y0, stdDev.y0,
				maxDiff.u, stdDev.u,
				maxDiff.y1, stdDev.y1,
				maxDiff.v, stdDev.v );

		DbgPrint( szBuf );

	/*}
	else
	{

	}*/

	return;
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
VOID
CWmvVerificationTest::CompareAudio()
{
	CHAR	szReferencePcm[MAX_PATH];
	HANDLE	hReferenceFile;

	// not sure about this yet:
	sprintf( szReferencePcm, "%s_%s_0.pcm", m_szRawPath, /*g_bOverlay ? */"YUY2"/* : "RGB32"*/ );
	hReferenceFile = CreateFile( szReferencePcm, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == hReferenceFile )
	{
		DbgPrint( "Ubable to open file %s (error %x)\n", szReferencePcm, GetLastError() );
		return;
	}

	if ( 16 == m_dwBitsPerSample )
	{
		SHORT*	decAudio;
		SHORT*	refAudio;

		DOUBLE	stdDev = 0;
		USHORT	maxDiff = 0;

		BOOL	bSuccess;
		DWORD	dwBytesRead;
		DWORD	dwSize = m_xmpAudio.dwMaxSize;
		DWORD	dwNumSamples = dwSize / sizeof(SHORT);

		refAudio = new SHORT [dwNumSamples];
		if ( NULL == refAudio )
		{
			SAFECLOSEHANDLE( hReferenceFile );
			return;
		}

		ZeroMemory( refAudio, dwSize );
		decAudio = (SHORT*)m_xmpAudio.pvBuffer;

		bSuccess = ReadFile( hReferenceFile, refAudio, dwSize, &dwBytesRead, NULL );
		if ( FALSE == bSuccess || dwSize != dwBytesRead )
		{
			DbgPrint( "Unable to properly read %s. Expected %d bytes, got %d.\n", szReferencePcm, dwSize, dwBytesRead );
			SAFECLOSEHANDLE( hReferenceFile );
			SAFEDELETEARRAY( refAudio );
			return;
		}

		SAFECLOSEHANDLE( hReferenceFile );

		for ( UINT i=0; i<dwNumSamples; i++ )
		{
			UINT nDiff;

			// get stats on sample
			nDiff = abs( decAudio[i]-refAudio[i] );
			stdDev += pow( nDiff, 2 );
			if ( nDiff > maxDiff )
			{
				maxDiff = (USHORT)nDiff;
			}
		}

		SAFEDELETEARRAY( refAudio );

		stdDev = sqrt( stdDev / dwNumSamples );
		
		// update global stat tracking variables
		if ( stdDev > m_fMaxStdDevAudio )
		{
			m_fMaxStdDevAudio = stdDev;
			m_dwMaxStdDevAudioFrame = m_dwAudioFrames;
		}
		if ( maxDiff > m_dwMaxDiffAudio )
		{
			m_dwMaxDiffAudio = maxDiff;
			m_dwMaxDiffAudioFrame = m_dwAudioFrames;
		}

		CHAR	szBuf[MAX_PATH];
		sprintf( szBuf, "Audio Frame: %d\tMax Diff: %d\tStd Dev: %f\n\n",
				m_dwAudioFrames, maxDiff, stdDev );

		DbgPrint( szBuf );

		return;
	}
	else
	{

	}

	return;
}