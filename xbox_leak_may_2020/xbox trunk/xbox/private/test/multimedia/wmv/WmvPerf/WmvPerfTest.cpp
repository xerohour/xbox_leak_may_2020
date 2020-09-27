/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvPerfTest.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 15-Oct-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	15-Oct-2001 jeffsul
		Initial Version

--*/

#include "WmvPerfTest.h"

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

DWORD g_dwVBlankCount = 0;
//-----------------------------------------------------------------------------
// Name: VBlankCallback
// Desc: Called every vblank, so we can mantain a count of vblanks
//-----------------------------------------------------------------------------
VOID _cdecl VBlankCallback( D3DVBLANKDATA *pData/*DWORD dwContext*/ )
{
    g_dwVBlankCount++;
}

/*++

Routine Description:

	Test constructor

Arguments:

	None

Return Value:

	None

--*/
CWmvPerfTest::CWmvPerfTest()
{
	m_pD3DDevice = NULL;
	m_pDSound = NULL;

#ifndef USE_WMVPLAYER
	m_pWMVDecoder = NULL;
	for( UINT i = 0; i < WMV_PERF_TEST_NUM_FRAMES; i++ )
    {
		m_aVideoFrames[i].pTexture = NULL;
    }
	m_pbSampleData = NULL;
	m_pvbQuad = NULL;
	m_pStream = NULL;
#endif

	m_nNumFilesPlayed = 0;
	m_bReset = FALSE;

	m_bGotData = FALSE;
	m_bDrawText = TRUE;
	m_bFullScreen = TRUE;
	ZeroMemory( &m_PrevInputState, sizeof(XINPUT_STATE) );

	g_dwVBlankCount = 0;
}


/*++

Routine Description:

	Test destructor

Arguments:

	None

Return Value:

	None

--*/
CWmvPerfTest::~CWmvPerfTest()
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
HRESULT CWmvPerfTest::ReadIni()
{
	HRESULT hr = S_OK;

	CHAR	szSearchPath[MAX_PATH];

	MLSetErrorLevel( 3 );
	// grab media files from server
#define CREATE_FLAG	MLCOPY_IFNEWER
	MEDIAFILEDESC MediaFiles[] =
	{
		{"wmv/ms_wmv_v8/%3",	"T:\\WMV\\Perf\\",			CREATE_FLAG},
		{NULL,									NULL,								0}
	};
	EXECUTE( LoadMedia( MediaFiles ) );

	// look for files on the xbox matching certain parameters
	//if ( CopyFile( "A:\\mad_audiovideo_filmvbr_quality97.wmv", "D:\\mad_audiovideo_filmvbr_quality97.wmv", FALSE ) )
	//{
	//	return E_FAIL;
	//}
	//if ( FALSE == CopyFile( "A:\\Office Space.wmv", "D:\\Office Space.wmv", FALSE ) )
	//{
	//	return E_FAIL;
	//}
	strcpy( m_szPath, "T:\\WMV\\Perf\\" );
	sprintf( szSearchPath, "%s*.wmv", m_szPath );
	
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
HRESULT CWmvPerfTest::InitGraphics()
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

	m_pD3DDevice->SetVerticalBlankCallback( VBlankCallback );

	//m_pD3DDevice->EnableOverlay( TRUE );

	// Create a buffer for the text
    hr = m_pD3DDevice->CreateTexture(640, 480, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &m_pd3dtText);
    if (FAILED(hr)) {
        return hr;
    }

    // Clear the texture
	D3DLOCKED_RECT d3dlr;
    m_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
    for ( UINT i = 0; i < 480; i++) {
        memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 640 * 4);
    }
    m_pd3dtText->UnlockRect(0);

    m_prText[0].x = 0.0f;
    m_prText[0].y = 480.0f;
    m_prText[0].u = 0.0f;
    m_prText[0].v = 480.0f;

    m_prText[1].x = 0.0f;
    m_prText[1].y = 0.0f;
    m_prText[1].u = 0.0f;
    m_prText[1].v = 0.0f;

    m_prText[2].x = 640.0f;
    m_prText[2].y = 0.0f;
    m_prText[2].u = 640.0f;
    m_prText[2].v = 0.0f;

    m_prText[3].x = 640.0f;
    m_prText[3].y = 480.0f;
    m_prText[3].u = 640.0f;
    m_prText[3].v = 480.0f;

    for (i = 0; i < 4; i++) {
        m_prText[i].z = 0.0f; //0.000001f;
        m_prText[i].rhw = 1.0f; ///1000000.0f;
    }

	D3DDISPLAYMODE mode;
    hr = m_pD3DDevice->GetDisplayMode(&mode);
    if (SUCCEEDED(hr) && mode.RefreshRate) {
        m_dwRefreshInterval = 1000*10000 / mode.RefreshRate;
    } 
	else 
	{
        // Default to 16msecs
        m_dwRefreshInterval = 16*10000;
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
HRESULT CWmvPerfTest::DestroyGraphics()
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
HRESULT CWmvPerfTest::InitAudio()
{
	HRESULT hr = S_OK;

	hr = DirectSoundCreate( NULL, &m_pDSound, NULL );
	if ( FAILED( hr ) )
	{
		DbgPrint( "Unable to create Direct Sound (error: %x)\n", hr );
		return hr;
	}
	m_pDSound->GetTime( &m_rtStartTime );

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

	cleanup audio device

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvPerfTest::DestroyAudio()
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
HRESULT CWmvPerfTest::InitResources()
{
	HRESULT hr = S_OK;
	
	m_rtStartTime = 0;
    m_rtSeekAdjustment = 0;
    m_pCurrentFrame = NULL;
	
	m_dwCurrentFrame =
	m_dwVideoFrames =
	m_dwRenderedFrames =
	m_dwVideoFPS	=
	m_dwRenderedFPS	=
	m_dwDroppedFrames =
	m_dwLastDroppedFrame = 0;

	m_pD3DDevice->EnableOverlay( TRUE );

	// Create the video decoder.  We only load
    // the section containing the decoder on demand
    XLoadSection("WMVDEC");
	CHAR	szFileName[MAX_PATH];
	sprintf( szFileName, "%s%s", m_szPath, m_FindData.cFileName );

	hr = XFONT_OpenDefaultFont( &m_pFont );
	if ( FAILED( hr ) )
	{
		DbgPrint( "Unable to create font (error: %x)\n", hr );
		return E_FAIL;
	}
	m_pFont->SetTextHeight( 4 );
	m_pFont->SetIntercharacterSpacing( -2 );


#ifdef USE_WMVPLAYER
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

#else

    hr = WmvCreateDecoder( szFileName,
							NULL,
							WMVVIDEOFORMAT_YUY2,
							NULL,
							&m_pWMVDecoder );
	if ( FAILED( hr ) )
	{
		DbgPrint( "Unable to create decoder for '%s' (error: %x)\n", szFileName, hr );
		return E_FAIL;
	}

	// If that succeeded, set up for decoding:
	CHECKEXECUTE( m_pWMVDecoder->GetPlayDuration( &m_rtDuration, &m_rtPreroll ) );
	CHECKEXECUTE( m_pWMVDecoder->GetAudioInfo( &m_wfx ) );
	if ( SUCCEEDED( hr ) )
	{
		// Set up a stream for audio playback
		DSSTREAMDESC dssd = {0};
		dssd.dwFlags =  0;
		dssd.dwMaxAttachedPackets = WMV_PERF_TEST_NUM_PACKETS;
		dssd.lpwfxFormat = &m_wfx;
		
		CHECKEXECUTE( DirectSoundCreateStream( &dssd, &m_pStream ) );
		
		// Allocate sample data
		if ( SUCCEEDED( hr ) )
		{
			m_pbSampleData = new BYTE[ WMV_PERF_TEST_NUM_PACKETS * WMV_PERF_TEST_PACKET_SIZE ];
			CHECKALLOC( m_pbSampleData );
			
			// Initialize audio packet status
			for( UINT i = 0; i < WMV_PERF_TEST_NUM_PACKETS; i++ )
			{
				m_adwStatus[ i ] = XMEDIAPACKET_STATUS_SUCCESS;
			}
		}
	}
	
	EXECUTE( m_pWMVDecoder->GetVideoInfo( &m_wmvVideoInfo ) );
	if ( SUCCEEDED( hr ) )
	{
		// Set up video frame buffers
		for( UINT i = 0; i < WMV_PERF_TEST_NUM_FRAMES; i++ )
		{
			// Create the texture
			hr = m_pD3DDevice->CreateTexture(
				m_wmvVideoInfo.dwWidth,
				m_wmvVideoInfo.dwHeight,
				0,
				0,
				D3DFMT_YUY2,
				NULL,
				&m_aVideoFrames[i].pTexture );
			if ( FAILED( hr ) )
			{
				DbgPrint( "Failed to create texture %d (error: %d)\n", i, hr );
				return E_FAIL;
			}
			
			// Get a pointer to the texture data
			D3DLOCKED_RECT lr;
			m_aVideoFrames[i].pTexture->LockRect( 0, &lr, NULL, 0 );
			m_aVideoFrames[i].pBits = (BYTE *)lr.pBits;
			m_aVideoFrames[i].pTexture->UnlockRect( 0 );
			
			m_aVideoFrames[i].dwSetAtVBlank = 0;
			m_fqFree.Enqueue( &m_aVideoFrames[i] );
		}
		
		// Create a vertex buffer for the quad
		FLOAT fLeft;
		FLOAT fRight;
		FLOAT fTop; 
		FLOAT fBottom;
		
		if ( FALSE == m_bFullScreen )
		{
			fLeft = 320.0f - m_wmvVideoInfo.dwWidth / 2;
			fRight = 320.0f + m_wmvVideoInfo.dwWidth / 2;
			fTop = 240.0f - m_wmvVideoInfo.dwHeight / 2;
			fBottom = 240.0f + m_wmvVideoInfo.dwHeight / 2;
		}
		else
		{
			fLeft = 0.0f;
			fRight = 640.0f;
			fTop = 240.0f - (m_wmvVideoInfo.dwHeight*320.0f/m_wmvVideoInfo.dwWidth);
			fBottom = 240.0f + (m_wmvVideoInfo.dwHeight*320.0f/m_wmvVideoInfo.dwWidth);
		}
		
		CHECKEXECUTE( m_pD3DDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ), 0, 0, 0, &m_pvbQuad ) );
		if ( SUCCEEDED( hr ) )
		{
			CUSTOMVERTEX * pVertices;
			m_pvbQuad->Lock( 0, 0, (BYTE **)&pVertices, 0 );
			pVertices[ 0 ].p = D3DXVECTOR4( fLeft, fTop, 1.0f, 1.0f ); pVertices[0].t = D3DXVECTOR2( 0.0f, 0.0f );
			pVertices[ 1 ].p = D3DXVECTOR4( fRight, fTop, 1.0f, 1.0f ); pVertices[1].t = D3DXVECTOR2( (FLOAT)m_wmvVideoInfo.dwWidth, 0.0f );
			pVertices[ 2 ].p = D3DXVECTOR4( fRight, fBottom, 1.0f, 1.0f ); pVertices[2].t = D3DXVECTOR2( (FLOAT)m_wmvVideoInfo.dwWidth, (FLOAT)m_wmvVideoInfo.dwHeight );
			pVertices[ 3 ].p = D3DXVECTOR4( fLeft, fBottom, 1.0f, 1.0f ); pVertices[3].t = D3DXVECTOR2( 0.0f, (FLOAT)m_wmvVideoInfo.dwHeight );
			m_pvbQuad->Unlock();
		}
	}
#endif
	
	m_dwLastFPS = m_dwCurrentTime-1000;

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
HRESULT CWmvPerfTest::DestroyResources()
{
	HRESULT hr = S_OK;

	m_pD3DDevice->EnableOverlay( FALSE );
	
#ifdef USE_WMVPLAYER
	for( UINT i = 0; i < 2; i++ )
    {
        RELEASE( m_pSurface[i] );
        RELEASE( m_pOverlay[i] );
    }
	m_WMVPlayer.CloseFile();

#else
	m_fqFree.m_pHead = m_fqFree.m_pTail = NULL;
	m_fqBusy.m_pHead = m_fqBusy.m_pTail = NULL;
	m_fqReady.m_pHead = m_fqReady.m_pTail = NULL;

	for( UINT i = 0; i < WMV_PERF_TEST_NUM_FRAMES; i++ )
    {
		RELEASE( m_aVideoFrames[i].pTexture );
    }
	RELEASE( m_pWMVDecoder );
	RELEASE( m_pvbQuad );
	RELEASE( m_pStream );
	if ( NULL != m_pbSampleData )
	{
		delete [] m_pbSampleData;
		m_pbSampleData = NULL;
	}
#endif

	XFreeSection("WMVDEC");

	m_pD3DDevice->EnableOverlay( FALSE );

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
	else
	{
		for( UINT i = 0; i < strlen( m_FindData.cFileName ); i++ )
		{
			m_szWideFileName[i] = (WCHAR)m_FindData.cFileName[i];
		}
		m_szWideFileName[i] = 0;
	}

	return hr;
}


/*++

Routine Description:

	get the input state from the control pad

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWmvPerfTest::ProcessInput()
{
	HRESULT hr = S_OK;

	DWORD dwDeviceMap;
	DWORD dwSuccess;
	BOOL bDeviceChanges;
	DWORD dwInsertions;
	DWORD dwRemovals;

	XINPUT_STATE xinpstate;

	bDeviceChanges = XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );
	if ( bDeviceChanges )
	{
		if ( dwInsertions & XDEVICE_PORT0_MASK )
		{
			m_hInpDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL );
			if ( m_hInpDevice == NULL )
			{
				DbgPrint( "Error Opening Input\n" );
				return E_FAIL;
			}
		}
		
		if ( dwRemovals & XDEVICE_PORT0_MASK )
		{
			DbgPrint( "Gamepad removed from Port 0\n" );
			return E_FAIL;
		}
	}

	dwDeviceMap = XGetDevices( XDEVICE_TYPE_GAMEPAD );
	if ( !(dwDeviceMap & XDEVICE_PORT0_MASK) ) 
	{
		//DbgPrint( "No Device in Port 0\n" );
		return S_OK; // live with it!
	}
	else if ( NULL == m_hInpDevice )
	{
		m_hInpDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL );
		if ( m_hInpDevice == NULL )
		{
			DWORD dwError;
			dwError = GetLastError();
			DbgPrint( "Error Opening Input. Error#: %d\n", dwError);
			return E_FAIL;
		}
	}

	dwSuccess = XInputGetState( m_hInpDevice, &xinpstate );
	if ( dwSuccess != ERROR_SUCCESS )
	{
		DbgPrint( "Error Getting Input State\n" );
		return dwSuccess;
	}

	// toggle the text display on/off
	if ( xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 15  && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] <= 15 )
	{
		m_bDrawText = !m_bDrawText;
		if ( TRUE == m_bDrawText )
		{
			m_dwLastFPS = -1000;
		}
	}

	// toggle full screen display on/off
	if ( xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 15  && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] <= 15 )
	{
		m_bFullScreen = !m_bFullScreen;
	}

	// go to the next file
	if ( xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] <= 15)
	{
		//m_rtSeekAdjustment += (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]-15)*320000/240;
		DumpPerfData();
		m_bReset = TRUE;
	}

	// quit the test
	if ( xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] &&
		 xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] && 
		 xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] )
	{
		m_bQuit = TRUE;
	}

	// copy the current state to the previous state structure for use next time around
	memcpy( &m_PrevInputState, &xinpstate, sizeof(XINPUT_STATE) );

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
HRESULT CWmvPerfTest::Update()
{
	HRESULT hr = S_OK;

	if ( m_bReset )
	{
#ifndef USE_WMVPLAYER
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
#endif
		DestroyResources();
		if ( TRUE == m_bQuit )
		{
			return S_OK;
		}
		hr = InitResources();
		m_bReset = FALSE;
		m_bGotData = FALSE;
		g_dwVBlankCount = 0;

		if ( FAILED( hr ) )
		{
			m_bReset = TRUE;
			return hr;
		}
	}

#ifdef USE_WMVPLAYER
	// Set our time for this frame:
	// Video Time = Current Time - Start Time + Seek Adjustment
	if ( 0 == m_rtStartTime )
	{
		m_pDSound->GetTime( &m_rtStartTime );
	}
	m_pDSound->GetTime( &m_rtCurrentTime );
	m_rtCurrentTime = m_rtCurrentTime - m_rtStartTime + m_rtSeekAdjustment;

	if ( m_dwDroppedFrames != m_WMVPlayer.m_dwDroppedFrames )
	{
		m_dwVideoFrames += m_WMVPlayer.m_dwDroppedFrames-m_dwDroppedFrames;
		m_dwLastDroppedFrame = m_WMVPlayer.m_dwDroppedFrames;
		m_dwDroppedFrames = m_WMVPlayer.m_dwDroppedFrames;
	}

#else
	VIDEOFRAME *pFrame;
	DWORD dwIndex = 0;
	// See how we're doing on audio packets
	DWORD dwPending = 0;
	DWORD dwSucceeded = 0;
	for( int i = 0; i < WMV_PERF_TEST_NUM_PACKETS; i++ )
	{
		switch( m_adwStatus[ i ] )
		{
		case XMEDIAPACKET_STATUS_PENDING:
			dwPending++;
			break;
		case XMEDIAPACKET_STATUS_SUCCESS:
			dwSucceeded++;
			break;
		}
	}
	
	// Now check video frames
	DWORD dwBusy = 0;
	dwPending = dwSucceeded = 0;
	for( pFrame = m_fqFree.m_pHead; pFrame; pFrame = pFrame->pNext )
		dwSucceeded++;
	for( pFrame = m_fqReady.m_pHead; pFrame; pFrame = pFrame->pNext )
		dwPending++;
	for( pFrame = m_fqBusy.m_pHead; pFrame; pFrame = pFrame->pNext )
		dwBusy++;

	// Set our time for this frame:
    // Video Time = Current Time - Start Time + Seek Adjustment
    m_pDSound->GetTime( &m_rtCurrentTime );
    m_rtCurrentTime = m_rtCurrentTime - m_rtStartTime + m_rtSeekAdjustment;
    
    // Process our 3 video queues
    ProcessVideoQueues();

    // If we are actively decoding...
    if( m_bDecode )
    {
        // Find an audio packet and a video frame
        while( FindFreePacket( &dwIndex ) && FindFreeFrame( &pFrame ) )
        {
            //
            // Set up an audio packet:
            //
            DWORD audStat, audSize;
            XMEDIAPACKET xmpAudio = {0};
            REFERENCE_TIME rtAudio;

            xmpAudio.dwMaxSize = WMV_PERF_TEST_PACKET_SIZE;
            xmpAudio.pvBuffer  = m_pbSampleData + dwIndex * WMV_PERF_TEST_PACKET_SIZE;
            xmpAudio.pdwStatus = &audStat;
            xmpAudio.pdwCompletedSize = &audSize;
            xmpAudio.prtTimestamp = &rtAudio;

            //
            // Set up a video packet
            //
            DWORD vidStat, vidSize;
            XMEDIAPACKET xmpVideo = {0};
            REFERENCE_TIME rtVideo;

            xmpVideo.dwMaxSize = m_wmvVideoInfo.dwWidth * 
                                 m_wmvVideoInfo.dwHeight * 
                                 m_wmvVideoInfo.dwOutputBitsPerPixel / 8;
            xmpVideo.pdwStatus = &vidStat;
            xmpVideo.pdwCompletedSize = &vidSize;
            xmpVideo.prtTimestamp = &rtVideo;

            // We shouldn't have put the frame in the free queue
            // until we were sure the next one was being used
            if( pFrame->pTexture->IsBusy() )
                DbgPrint( "Writing to a busy frame.\n" );

            xmpVideo.pvBuffer  = pFrame->pBits;
			ZeroMemory( xmpVideo.pvBuffer, xmpVideo.dwMaxSize );
        
            // Process the decoder
            hr = m_pWMVDecoder->ProcessMultiple( &xmpVideo, &xmpAudio );

            // S_FALSE means that we hit the end of the video
            if( S_FALSE == hr )
            {
                //DbgPrint( "Done playing video\n" );
				DumpPerfData();
				m_bReset = TRUE;
                break;
            }

            // Get the system start time as of first packet, so 
            // that we can calculate the reference time 
            if( !m_bGotData && ( audSize > 0 || vidSize > 0 ) )
            {
                m_bGotData = TRUE;
                m_pDSound->GetTime( &m_rtStartTime );
                m_rtCurrentTime = m_rtSeekAdjustment;
				m_dwLastFPS = m_dwCurrentTime-1000;
				m_dwVideoFPS = 
				m_dwVideoFrames = 0;
            }
            
            // If we got a video frame, append it to the queue
            if( vidSize > 0 )
            {
                // TODO: Understand why decoder gives us these packets.
                if( rtVideo > m_rtDuration )
                {
                    DbgPrint( "Got RT: %I64d, but duration is only %I64d.  Ditching frame.\n", rtVideo, m_rtDuration );
                }
                else
                {
                    // Update the pending video frame queue
                    pFrame->rtTimeStamp = rtVideo;
                    pFrame->dwFrameNumber = m_dwVideoFrames++;//m_dwVidFrame++;
                    m_fqReady.Enqueue( m_fqFree.Dequeue() );
					m_dwVideoFPS++;
                }
            }

            // If we got an audio packet, send it to the stream
            if( audSize > 0 )
            {
                // Process the audio packet
                xmpAudio.dwMaxSize = audSize;
                xmpAudio.pdwCompletedSize = NULL;
                xmpAudio.pdwStatus = &( m_adwStatus[ dwIndex ] );
                m_pStream->Process( &xmpAudio, NULL );
            }
        }
    }
#endif

	m_dwRenderedFPS++;
	m_dwRenderedFrames++;

	// a second has gone by, lets update our onscreen text
	if ( 1000 < m_dwCurrentTime - m_dwLastFPS )
	{
		// Clear the texture
		D3DLOCKED_RECT d3dlr;
		m_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
		for ( UINT i = 0; i < 480; i++) {
			memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 640 * 4);
		}
		m_pd3dtText->UnlockRect(0);
		
		LPDIRECT3DSURFACE8	pSurface = NULL;
		hr = m_pd3dtText->GetSurfaceLevel(0, &pSurface);
		if ( SUCCEEDED( hr ) && m_bDrawText )
		{

#define DRAWTEXT( x, y ) m_pFont->TextOut( pSurface, buffer, -1, x, y );
//#define DRAWTEXT( x, y ) m_BitFont.DrawText( pSurface, buffer, x, y, 0, D3DCOLOR_XRGB( 255, 255, 255 ), D3DCOLOR_XRGB( 0, 0, 0 ) );

			//m_BitFont.DrawText( pSurface, m_szWideFileName, 20, 15, 0, D3DCOLOR_XRGB( 255, 255, 255 ), D3DCOLOR_XRGB( 0, 0, 0 ) );
			m_pFont->SetTextAlignment( XFONT_TOP | XFONT_LEFT );
			m_pFont->TextOut( pSurface, m_szWideFileName, -1, 30, 20 );

			WCHAR buffer[MAX_PATH];
			UINT nCurrentTime = (UINT)(m_rtCurrentTime/10000000);
			UINT nTotalTime = (UINT)(m_rtDuration/10000000);

			swprintf( buffer, L"Time: %u / %u", nCurrentTime, nTotalTime );
			DRAWTEXT( 30, 40 );

			m_pFont->SetTextAlignment( XFONT_TOP | XFONT_RIGHT );
			swprintf( buffer, L"Decoding: %u fps", m_dwVideoFPS );
			DRAWTEXT( 640-30, 20 );
			swprintf( buffer, L"Rendering: %u fps", m_dwRenderedFPS );
			DRAWTEXT( 640-30, 40 );

			m_pFont->SetTextAlignment( XFONT_BOTTOM | XFONT_LEFT );
			swprintf( buffer, L"Dropped Frames: %d / %d (%d%%)", m_dwDroppedFrames, m_dwVideoFrames, (m_dwVideoFrames) ? m_dwDroppedFrames*100/m_dwVideoFrames : 0 );
			DRAWTEXT( 30, 480-40 );
			swprintf( buffer, L"Last Dropped: %d", m_dwLastDroppedFrame );
			DRAWTEXT( 30, 480-20 );
		}
		RELEASE( pSurface );

		m_dwRenderedFPS = 
		m_dwVideoFPS = 0;
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
#define CLAMP( x, a, b ) \
if ( x < a ) x = a; \
else if ( x > b ) x = b;

HRESULT CWmvPerfTest::Render()
{
	HRESULT hr = S_OK;

	if ( TRUE == m_bQuit )
	{
		return S_OK;
	}

	RECT rcSrc = { 0, 0, m_wmvVideoInfo.dwWidth, m_wmvVideoInfo.dwHeight };
	RECT rcDest;
	if ( m_bFullScreen )
	{
		rcDest.left		= 0;
		rcDest.top		= 240- (m_wmvVideoInfo.dwHeight*320/m_wmvVideoInfo.dwWidth);
		CLAMP( rcDest.top, 0, 240 );
		rcDest.right	= 640;
		rcDest.bottom	= 240+ (m_wmvVideoInfo.dwHeight*320/m_wmvVideoInfo.dwWidth);
		CLAMP( rcDest.bottom, 240, 480 );
	}
	else 
	{
		rcDest.left		= 320-m_wmvVideoInfo.dwWidth / 2;
		CLAMP( rcDest.left, 0, 320 );
		rcDest.top		= 240-m_wmvVideoInfo.dwHeight / 2;
		CLAMP( rcDest.top, 0, 240 );
		rcDest.right	= 320+m_wmvVideoInfo.dwWidth / 2;
		CLAMP( rcDest.right, 320, 640 );
		rcDest.bottom	= 240+m_wmvVideoInfo.dwHeight / 2;
		CLAMP( rcDest.bottom, 240, 480 );
		//rcDest = { 320-m_wmvVideoInfo.dwWidth / 2, 240-m_wmvVideoInfo.dwHeight / 2, 320+m_wmvVideoInfo.dwWidth / 2, 240+m_wmvVideoInfo.dwHeight / 2 };
	}

	// clear the rendering surface
	EXECUTE( m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL|D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0L ) );

	// begin the scene
	EXECUTE( m_pD3DDevice->BeginScene() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

#ifdef USE_WMVPLAYER
	// Get the next frame into our texture
	hr = m_WMVPlayer.GetTexture( m_pOverlay[m_dwCurrent] );
	if( FAILED( hr ) )
	{
		__asm int 3;
	}

	if ( m_WMVPlayer.IsReady() /*&& m_pD3DDevice->GetOverlayUpdateStatus()*/ )
    {
		m_pD3DDevice->BlockUntilVerticalBlank();
		m_pD3DDevice->UpdateOverlay( m_pSurface[m_dwCurrent], &rcSrc, &rcDest, FALSE, 0 );
		m_dwVideoFrames++;
		m_dwVideoFPS++;
		
		// Decode the next frame
		if( S_FALSE == m_WMVPlayer.DecodeNext() )
		{
			DumpPerfData();
			m_bReset = TRUE;
			return hr;
		}
		
		// We'll use the other texture next time
		m_dwCurrent ^= 1;
	}
	
#else

	DirectSoundDoWork();

	// draw the quad with the video texture
    if( m_pCurrentFrame )
    {
		LPDIRECT3DSURFACE8 pSurface;
		//D3DLOCKED_RECT LockedRect = {0};
		hr = m_pCurrentFrame->pTexture->GetSurfaceLevel( 0, &pSurface );
		if ( FAILED( hr ) )
		{
			return hr;
		}
		m_pD3DDevice->UpdateOverlay( pSurface, &rcSrc, &rcDest, FALSE, 0 );
		RELEASE( pSurface );

		// Remember when we attempted to render this frame.
        // The time it actually gets rendered depends on the method
        // For overlay: We can be sure it's been rendered after next vblank
        m_pCurrentFrame->dwSetAtVBlank = g_dwVBlankCount;
    }
#endif

	// draw the geometry with the text on it
	if ( m_bDrawText )
	{
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
	}

	// render the scene to the surface
	EXECUTE( m_pD3DDevice->EndScene() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	// flip the surface to the screen
	EXECUTE( m_pD3DDevice->Present( NULL, NULL, NULL, NULL ) );

	if ( TRUE == m_bReset )
	{
		m_pD3DDevice->EnableOverlay( FALSE );
	}

	return hr;
}

#ifndef USE_WMVPLAYER
//-----------------------------------------------------------------------------
// Name: ProcessVideoQueues()
// Desc: Processes our video queues:
//       1) Free up any busy frames that we know are no longer in use.
//          For overlay, we know a frame is no longer in use if we're at
//          least one VBlank after updating with the next frame.
//          For textures, we could have up to 3 frames in the pushbuffer,
//          so we know a frame is no longer in use if we're at least 3 VBlanks
//          after rendering the next frame
//       2) Discard any frames from the ready queue that are obsolete.  A
//          frame is obsolete if the frame after it is displayable.
//       3) Figure out if we need to render a new frame - that is, is there
//          a frame in the ready queue with a timestamp less than current time
//-----------------------------------------------------------------------------
HRESULT
CWmvPerfTest::ProcessVideoQueues()
{
    // Step 1: Move packets from busy->free if they're not being used.
    // The only time the busy queue should be empty is starting up
    while( !m_fqBusy.IsEmpty() )
    {
        // If frame Y was submitted after frame X, and we're sure
        // frame Y is the one being displayed, free frame X
        if( m_fqBusy.m_pHead->pNext &&
            m_fqBusy.m_pHead->pNext->dwSetAtVBlank < g_dwVBlankCount )
        {
            m_fqFree.Enqueue( m_fqBusy.Dequeue() );
        }
        else
            break;
    }

    // Step 2: Discard dropped frames.  We drop frame X if there's 
    // frame Y after it, and frame Y is displayable
    while( !m_fqReady.IsEmpty() && 
            m_fqReady.m_pHead->pNext &&
            m_fqReady.m_pHead->pNext->rtTimeStamp <= m_rtCurrentTime
			/*(m_rtCurrentTime - (m_fqReady.m_pHead->rtTimeStamp+m_rtStartTime)) > m_dwRefreshInterval*/ )
    {
		m_dwDroppedFrames++;
		m_dwLastDroppedFrame = m_fqReady.m_pHead->dwFrameNumber;
        m_fqFree.Enqueue( m_fqReady.Dequeue() );
    }

    // Step 3: Now we have at most 1 frame starting before current time
    // See if he should be submitted to render
    if( !m_fqReady.IsEmpty() &&
        m_fqReady.m_pHead->rtTimeStamp <= m_rtCurrentTime )
    {
        // Render the packet
        m_pCurrentFrame = m_fqReady.Dequeue();
		m_dwCurrentFrame = m_pCurrentFrame->dwFrameNumber;
        m_fqBusy.Enqueue( m_pCurrentFrame );
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FindFreePacket
// Desc: Attempts to find a free audio packet (one that isn't currently 
//       submitted to the stream).  Returns TRUE if it found one
//-----------------------------------------------------------------------------
BOOL
CWmvPerfTest::FindFreePacket( DWORD * pdwIndex )
{
    // Check the status of each packet
    for( int i = 0; i < WMV_PERF_TEST_NUM_PACKETS; i++ )
    {
        // If we find a non-pending packet, return it
        if( m_adwStatus[ i ] != XMEDIAPACKET_STATUS_PENDING )
        {
            *pdwIndex = i;
            return TRUE;
        }
    }

    return FALSE;
}



//-----------------------------------------------------------------------------
// Name: FindFreeFrame
// Desc: Attempts to find an available video frame (one that is not in the
//          pending video frame queue).  Returns TRUE if one was available
//-----------------------------------------------------------------------------
BOOL
CWmvPerfTest::FindFreeFrame( VIDEOFRAME ** ppFrame )
{
    // Do we have any frames in the free queue?
    if( !m_fqFree.IsEmpty() )
    {
        *ppFrame = m_fqFree.m_pHead;
        return TRUE;
    }

    return FALSE;
}
#endif

VOID
CWmvPerfTest::DumpPerfData()
{
	DbgPrint( "\n[WmvPerf] performance for %s:\n\
				Video Dimesions: %d x %d\n\
				Video Bitrate: %d bps\n\
				Audio Bits per sample: %d\n\
				Audio Samples per second: %d\n\
				Format: %s\n\
				Rendered FPS: %d\n\
				Decoded FPS: %d\n\
				Dropped Frames: %d / %d (%d%%)\n\
				Last Dropped: %d\n\n",
				m_FindData.cFileName,
				m_wmvVideoInfo.dwWidth, m_wmvVideoInfo.dwHeight,
				m_wmvVideoInfo.dwBitsPerSecond,
				m_wfx.wBitsPerSample, 
				m_wfx.nSamplesPerSec,
				"YUY2",
				(m_rtCurrentTime > 10000000) ? (INT)(m_dwRenderedFrames / (m_rtCurrentTime/10000000)) : 0,
				(m_rtCurrentTime > 10000000) ? (INT)(m_dwVideoFrames / (m_rtCurrentTime/10000000)) : 0,
				m_dwDroppedFrames, m_dwVideoFrames, (m_dwVideoFrames) ? m_dwDroppedFrames*100/m_dwVideoFrames : 0,
				m_dwLastDroppedFrame );
}
