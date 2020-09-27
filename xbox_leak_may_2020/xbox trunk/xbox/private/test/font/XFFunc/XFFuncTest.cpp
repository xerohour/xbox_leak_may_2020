/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	XFFuncTest.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 31-Oct-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	31-Oct-2001 jeffsul
		Initial Version

--*/

#include "XFFuncTest.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xgmath.h>
#include <medialoader.h>

#include <stdio.h>
#include <tchar.h>



BOOL CXFFuncTest::AutoRand()
{
	if ( rand()%100 > 95 && m_bAuto)
	{
		return TRUE;
	}
	return FALSE;
}

CXFFuncTest::CXFFuncTest()
{
	m_pD3DDevice		= NULL;
	m_pXFont			= NULL;
	m_pStatusFont		= NULL;
	m_hFiles			= INVALID_HANDLE_VALUE;
//	m_FindData			= {0};
	
	m_uAlignment		= XFONT_BASELINE|XFONT_CENTER;
	m_uHeight			= 16;
	m_uRLEWidth			= 2;
	m_uSpacing			= 0;
	m_uColor			= 0;
	m_uStyle			= XFONT_NORMAL;
	m_uAALevel			= 0;
	m_nBackgroundMode	= XFONT_TRANSPARENT;

	m_nPosX				= 320;
	m_nPosY				= 240;

	m_bAuto				= TRUE;
	srand( timeGetTime() );
}


CXFFuncTest::~CXFFuncTest()
{
}

HRESULT	
CXFFuncTest::ReadIni()
{
	HRESULT		hr		= S_OK;

	MEDIAFILEDESC MediaFiles [] =
	{
		{ "TTF/%5", "T:\\Fonts\\",		MLCOPY_IFNEWER},
		{ NULL,		NULL,				0 }
	};

	MLSetErrorLevel( 3 );
	hr = LoadMedia( MediaFiles );

	return hr;
}


HRESULT
CXFFuncTest::InitGraphics()
{
	HRESULT		hr		= S_OK;

	D3DPRESENT_PARAMETERS  d3dpp;

	ZeroMemory( &d3dpp, sizeof(D3DPRESENT_PARAMETERS) );

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

	hr = Direct3D_CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, 
								&d3dpp,
								&m_pD3DDevice );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	return hr;
}


HRESULT
CXFFuncTest::DestroyGraphics()
{
	HRESULT		hr		= S_OK;

	RELEASE( m_pD3DDevice );
	
	return hr;
}


HRESULT
CXFFuncTest::InitResources()
{
	HRESULT		hr		= S_OK;

	TCHAR	wszPath[MAX_PATH];
	TCHAR	wszFileName[MAX_PATH];

	m_hFiles = FindFirstFile( XFFUNC_PATH, &m_FindData );
	if ( INVALID_HANDLE_VALUE == m_hFiles )
	{
		return E_FAIL;
	}

	lstrcpy( wszPath, XFFUNC_PATHW );
	mbstowcs( wszFileName, m_FindData.cFileName, strlen( m_FindData.cFileName ) + 1 );
	lstrcat( wszPath, wszFileName );

	hr = XFONT_OpenTrueTypeFont( wszPath, XFFUNC_CACHE_SIZE, &m_pXFont );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	m_pXFont->SetTextHeight( m_uHeight );
	m_pXFont->SetTextAntialiasLevel( m_uAALevel );
	m_pXFont->SetRLEWidth( m_uRLEWidth );
	m_pXFont->SetTextStyle( m_uStyle );
	m_pXFont->SetTextAlignment( m_uAlignment );
	m_pXFont->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
	m_pXFont->SetBkColor( D3DCOLOR_XRGB( 0, 0, 0 ) );
	m_pXFont->SetBkMode( m_nBackgroundMode );
	m_pXFont->SetIntercharacterSpacing( m_uSpacing );

	hr = XFONT_OpenDefaultFont( &m_pStatusFont );
	if ( FAILED( hr ) )
	{
		return hr;
	}
	
	return hr;
}


HRESULT
CXFFuncTest::DestroyResources()
{
	HRESULT		hr		= S_OK;

	RELEASE( m_pXFont );
	RELEASE( m_pStatusFont );

	FindClose( m_hFiles );
	
	return hr;
}


HRESULT
CXFFuncTest::ProcessInput()
{
	HRESULT		hr		= S_OK;

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

	if ( abs(xinpstate.Gamepad.sThumbLX) > 0.15f*32768.0f )
	{
		m_nPosX = 320 + (INT)((FLOAT)xinpstate.Gamepad.sThumbLX*320.0f/32768.0f);
	}
	else
	{
		m_nPosX = 320;
	}
	if ( abs(xinpstate.Gamepad.sThumbLY) > 0.15f*32768.0f )
	{
		m_nPosY = 240 - (INT)((FLOAT)xinpstate.Gamepad.sThumbLY*320.0f/32768.0f);
	}
	else
	{
		m_nPosY = 240;
	}

	// a pressed, update alignment (horizontal)
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] <= 15) || AutoRand() )
	{
		if ( XFONT_CENTER == (m_uAlignment & XFONT_CENTER) )
		{
			m_uAlignment -= XFONT_CENTER;
		}
		else if ( m_uAlignment & XFONT_RIGHT )
		{
			m_uAlignment += XFONT_CENTER-XFONT_RIGHT;
		}
		else
		{
			m_uAlignment += XFONT_RIGHT-XFONT_LEFT;
		}
		m_pXFont->SetTextAlignment( m_uAlignment );
	}

	// b pressed, update style
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] <= 15) || AutoRand() )
	{
		m_uStyle++;
		if ( m_uStyle > 4 ) m_uStyle = 1;
		m_pXFont->SetTextStyle( m_uStyle );
	}

	// x pressed, update alignment (vertical)
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] <= 15) || AutoRand() )
	{
		if ( XFONT_BASELINE == (m_uAlignment & XFONT_BASELINE) )
		{
			m_uAlignment -= XFONT_BASELINE;
		}
		else if ( m_uAlignment & XFONT_BOTTOM )
		{
			m_uAlignment += XFONT_BASELINE-XFONT_BOTTOM;
		}
		else
		{
			m_uAlignment += XFONT_BOTTOM-XFONT_TOP;
		}
		m_pXFont->SetTextAlignment( m_uAlignment );
	}

	// y pressed, update antialias mode
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] <= 15) || AutoRand() )
	{
		m_uAALevel += 2;
		if ( m_uAALevel > 4 ) m_uAALevel = 0;
		m_pXFont->SetTextAntialiasLevel( m_uAALevel );
	}

	// white pressed update background transparency mode
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] <= 15) || AutoRand() )
	{
		( XFONT_TRANSPARENT == m_nBackgroundMode ) ? m_nBackgroundMode = XFONT_OPAQUE : m_nBackgroundMode = XFONT_TRANSPARENT;
		m_pXFont->SetBkMode( m_nBackgroundMode );
	}

	// black pressed, update color
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] <= 15) || AutoRand() )
	{
		m_uColor++;
		switch( m_uColor )
		{
		case 0:
			m_pXFont->SetTextColor( D3DCOLOR_XRGB( 255, 255, 0 ) );
			m_pXFont->SetBkColor( D3DCOLOR_XRGB( 0, 0, 255 ) );
			break;
		case 1:
			m_pXFont->SetTextColor( D3DCOLOR_XRGB( 0, 0, 0 ) );
			m_pXFont->SetBkColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
			break;
		case 2:
			m_pXFont->SetTextColor( D3DCOLOR_XRGB( 255, 0, 0 ) );
			m_pXFont->SetBkColor( D3DCOLOR_XRGB( 0, 255, 0 ) );
			break;
		case 3:
			m_pXFont->SetTextColor( D3DCOLOR_XRGB( 255, 0, 255 ) );
			m_pXFont->SetBkColor( D3DCOLOR_XRGB( 255, 0, 0 ) );
			break;
		case 4:
			m_pXFont->SetTextColor( D3DCOLOR_XRGB( 0, 255, 0 ) );
			m_pXFont->SetBkColor( D3DCOLOR_XRGB( 0, 255, 255 ) );
			break;
		case 5:
			m_pXFont->SetTextColor( D3DCOLOR_XRGB( 0, 0, 255 ) );
			m_pXFont->SetBkColor( D3DCOLOR_XRGB( 255, 255, 0 ) );
			break;
		default:
			m_pXFont->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
			m_pXFont->SetBkColor( D3DCOLOR_XRGB( 0, 0, 0 ) );
			m_uColor = 0;
			break;
		}
	}

	// up pressed, increase font size
	if ( ((xinpstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && !(m_PrevInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ) || AutoRand() )
	{
		m_uHeight++;
		m_pXFont->SetTextHeight( m_uHeight );
	}

	// down pressed, decrease font size
	if ( ((xinpstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && !(m_PrevInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ) || AutoRand() )
	{
		m_uHeight--;
		m_pXFont->SetTextHeight( m_uHeight );
	}

	// left pressed, decrease spacing
	if ( ((xinpstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && !(m_PrevInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ) || AutoRand() )
	{
		m_uSpacing--;
		m_pXFont->SetIntercharacterSpacing( m_uSpacing );
	}

	// right pressed, increase spacing
	if ( ((xinpstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && !(m_PrevInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ) || AutoRand() )
	{
		m_uSpacing++;
		m_pXFont->SetIntercharacterSpacing( m_uSpacing );
	}

	// l trigger pressed, change RLE width
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] <= 15) || AutoRand() )
	{
		m_uRLEWidth = m_uRLEWidth << 1;
		if ( m_uRLEWidth > 8 ) m_uRLEWidth = 2;
		m_pXFont->SetRLEWidth( m_uRLEWidth );
	}

	// r trigger pressed, open next file
	if ( (xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] <= 15) || AutoRand() )
	{
		BOOL bSuccess = FALSE;
		bSuccess = FindNextFile( m_hFiles, &m_FindData );

		if ( FALSE == bSuccess )
		{
			m_hFiles = FindFirstFile( XFFUNC_PATH, &m_FindData );
		}

		if ( INVALID_HANDLE_VALUE != m_hFiles )
		{
			TCHAR	wszPath[MAX_PATH];
			TCHAR	wszFileName[MAX_PATH];

			lstrcpy( wszPath, XFFUNC_PATHW );
			mbstowcs( wszFileName, m_FindData.cFileName, strlen( m_FindData.cFileName ) + 1 );
			lstrcat( wszPath, wszFileName );

			RELEASE( m_pXFont );
			hr = XFONT_OpenTrueTypeFont( wszPath, XFFUNC_CACHE_SIZE, &m_pXFont );
			if ( SUCCEEDED( hr ) )
			{
				m_pXFont->SetTextHeight( m_uHeight );
				m_pXFont->SetTextAntialiasLevel( m_uAALevel );
				m_pXFont->SetRLEWidth( m_uRLEWidth );
				m_pXFont->SetTextStyle( m_uStyle );
				m_pXFont->SetTextAlignment( m_uAlignment );
				m_pXFont->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
				m_pXFont->SetBkColor( D3DCOLOR_XRGB( 0, 0, 0 ) );
				m_pXFont->SetBkMode( m_nBackgroundMode );
				m_pXFont->SetIntercharacterSpacing( m_uSpacing );
			}
		}
	}

	// start pressed, toggle auto mode
	if ( (xinpstate.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !(m_PrevInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START) )
	{
		m_bAuto = !m_bAuto;
	}

	// copy the current state to the previous state structure for use next time around
	memcpy( &m_PrevInputState, &xinpstate, sizeof(XINPUT_STATE) );
	
	return hr;
}


HRESULT
CXFFuncTest::Render()
{
	HRESULT		hr					= S_OK;
	TCHAR		wszBuffer[MAX_PATH];	
	TCHAR		wszStatusBuffer[MAX_PATH];
	LPDIRECT3DSURFACE8	pSurface;

	lstrcpy( wszBuffer, XFFUNC_TEST_STRING );

	m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, 0, 1.0, 0 );

	hr = m_pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pSurface );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	m_pXFont->TextOut( pSurface, wszBuffer, lstrlen( wszBuffer ), m_nPosX, m_nPosY );

	m_pStatusFont->SetTextAlignment( XFONT_LEFT|XFONT_TOP );

	// output font name
	mbstowcs( wszStatusBuffer, m_FindData.cFileName, strlen( m_FindData.cFileName ) +1 );
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 25, 25 );

	// output horizontal alignment
	if ( XFONT_CENTER == (XFONT_CENTER & m_uAlignment) )
	{
		lstrcpy( wszStatusBuffer, L"HAlign: Center" );
	}
	else if ( XFONT_RIGHT & m_uAlignment )
	{
		lstrcpy( wszStatusBuffer, L"HAlign: Right" );
	}
	else
	{
		lstrcpy( wszStatusBuffer, L"HAlign: Left" );
	}
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 25, 50 );

	// output vertical alignment
	if ( XFONT_BASELINE == (XFONT_BASELINE & m_uAlignment) )
	{
		lstrcpy( wszStatusBuffer, L"VAlign: Baseline" );
	}
	else if ( XFONT_BOTTOM & m_uAlignment )
	{
		lstrcpy( wszStatusBuffer, L"VAlign: Bottom" );
	}
	else
	{
		lstrcpy( wszStatusBuffer, L"VAlign: Top" );
	}
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 25, 75 );

	m_pStatusFont->SetTextAlignment( XFONT_RIGHT|XFONT_TOP );

	// output style info
	if ( XFONT_NORMAL == m_uStyle )
	{
		lstrcpy( wszStatusBuffer, L"Style: Normal" );
	}
	else if ( XFONT_BOLD == m_uStyle )
	{
		lstrcpy( wszStatusBuffer, L"Style: Bold" );
	}
	else if ( XFONT_ITALICS == m_uStyle )
	{
		lstrcpy( wszStatusBuffer, L"Style: Italics" );
	}
	else
	{
		lstrcpy( wszStatusBuffer, L"Style: Bold Italics" );
	}
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 610, 25 );

	// output background transparency mode
	if ( XFONT_TRANSPARENT == m_nBackgroundMode )
	{
		lstrcpy( wszStatusBuffer, L"Bk Mode: Transparent" );
	}
	else
	{
		lstrcpy( wszStatusBuffer, L"Bk Mode: Opaque" );
	}
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 610, 50 );

	// ouput AA level
	swprintf( wszStatusBuffer, L"Anti Alias Level: %d", m_uAALevel );
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 610, 75 );

	m_pStatusFont->SetTextAlignment( XFONT_LEFT|XFONT_BOTTOM );

	// output height
	swprintf( wszStatusBuffer, L"Height: %d", m_uHeight );
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 25, 400 );

	// output rle width
	swprintf( wszStatusBuffer, L"RLE Width: %d", m_uRLEWidth );
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 25, 425 );

	// output spacing
	swprintf( wszStatusBuffer, L"Spacing: %d", m_uSpacing );
	m_pStatusFont->TextOut( pSurface, wszStatusBuffer, lstrlen( wszStatusBuffer ), 25, 450 );


	RELEASE( pSurface );

	m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
	
	return hr;
}