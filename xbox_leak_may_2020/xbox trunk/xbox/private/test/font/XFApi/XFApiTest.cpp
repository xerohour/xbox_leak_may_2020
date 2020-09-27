/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	XFApiTest.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 05-Nov-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	05-Nov-2001 jeffsul
		Initial Version

--*/

#include "XFApiTest.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xgmath.h>
#include <medialoader.h>

#include <stdio.h>
#include <tchar.h>

CXFApiTest::CXFApiTest()
{
}


CXFApiTest::~CXFApiTest()
{
}


HRESULT CXFApiTest::ReadIni()
{ 
	HRESULT		hr		= S_OK;

	srand( timeGetTime() );
	ExecuteParameterChecks();

	m_bQuit = TRUE;

	return hr;
}

#define XFAPI_NUM_TESTS		1000
#define XFAPI_BMP_PATH		L"T:\\Fonts\\font.bmp"
#define XFAPI_FONT_PATH		L"T:\\Fonts\\xboxbk.ttf"
#define XFAPI_CACHE_SIZE	2048
#define XFAPI_TEST_STRING	L"The quick brown fox jumps over the lazy dog."


VOID CXFApiTest::ExecuteParameterChecks()
{
	XFONT*	pXFont	= NULL;
	UINT	i		= 0;
	HRESULT hr		= S_OK;
	MEMORYSTATUS		MemStatus			= {0};
	DWORD				dwInitAvailPhys		= 0;
	DWORD				dwAvailPhys 		= 0;

	// get current memory status
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	dwInitAvailPhys = dwAvailPhys  = MemStatus.dwAvailPhys;
	
	for ( i=0; i < XFAPI_NUM_TESTS; i++ )
	{	
		// create a default font
		hr = XFONT_OpenDefaultFont( &pXFont ); 
		if ( FAILED( hr ) )
		{
			DbgPrint( "[XFAPI]: ***Error creating default font (error: %x)***\n", hr );
		}
		RELEASE( pXFont );

		// open bitmap font
		hr = XFONT_OpenBitmapFont( XFAPI_BMP_PATH, XFAPI_CACHE_SIZE, &pXFont ); 
		if ( FAILED( hr ) )
		{
			DbgPrint( "[XFAPI]: ***Error creating bitmap font %s w/ cache size %d (error: %x)***\n", XFAPI_BMP_PATH, XFAPI_CACHE_SIZE, hr );
		}
		RELEASE( pXFont );

		// open bitmap memory font
/*		LPVOID	pvBitmapBuf = NULL;
		UINT	uFontSize = 33333;
		hr = XFONT_OpenBitmapFontFromMemory( pvBitmapBuf, uFontSize, &pXFont ); 
		if ( FAILED( hr ) )
		{
			DbgPrint( "[XFAPI]: ***Error creating bitmap memory font %x size %d (error: %x)***\n", pvBitmapBuf, uFontSize, hr );
		}
		RELEASE( pXFont );
*/

		// open true type font
		hr = XFONT_OpenTrueTypeFont( XFAPI_FONT_PATH, XFAPI_CACHE_SIZE, &pXFont ); 
		if ( FAILED( hr ) )
		{
			DbgPrint( "[XFAPI]: ***Error creating true type font %s cache size %d (error: %x)***\n", XFAPI_FONT_PATH, XFAPI_CACHE_SIZE, hr );
		}
		RELEASE( pXFont );
	}

	// compare memory
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[XFAPI] warning: Possible leak in XFONT_OpenXXX.  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, i );
	}
	else
	{
		DbgPrint( "[XFAPI] info: XFONT_OpenXXX memory is ok (%d runs).\n", i );
	}

	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	UINT		uHeight		= 16;
	UINT		uAALevel	= 0;
	UINT		uSpacing	= 0;
	UINT		uRLEWidth	= 2;
	UINT		uStyle		= XFONT_NORMAL;
	UINT		uAlignment	= XFONT_CENTER|XFONT_BASELINE;
	INT			nBkMode		= XFONT_TRANSPARENT;
	D3DRECT		Rect		= { 0, 0, 640, 480 };

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
								&pd3dDevice );
	if ( FAILED( hr ) )
	{
		DbgPrint( "[XFAPI]: ***Error creating d3d device (error: %x)***\n", hr );
		return;
	} 

	// open font to run more ops on
	hr = XFONT_OpenTrueTypeFont( XFAPI_FONT_PATH, XFAPI_CACHE_SIZE, &pXFont ); 
	//hr = XFONT_OpenBitmapFont( XFAPI_BMP_PATH, XFAPI_CACHE_SIZE, &pXFont ); 
	if ( FAILED( hr ) )
	{
		DbgPrint( "[XFAPI]: ***Error creating font (error: %x)***\n", hr );
	}
	uHeight = uHeight + XFAPI_NUM_TESTS;
	pXFont->SetTextHeight( uHeight );

	// get current memory status
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	dwAvailPhys  = MemStatus.dwAvailPhys;

	for ( i=0; i < XFAPI_NUM_TESTS; i++ )
	{

		//------------------------------------------------------------------------------
		//	update font attributes
		//------------------------------------------------------------------------------

		// update height
		//if ( 64 == uHeight )
		//{
		//	uHeight = 128;
		//}
		//else
		//{
		//	uHeight = 64;
		//}
		uHeight--;

		// update spacing
		uSpacing++;

		// update rle width
		uRLEWidth = uRLEWidth << 1;
		if ( uRLEWidth > 8 ) uRLEWidth = 2;

		// update aa level
		uAALevel += 2;
		if ( uAALevel > 4 ) uAALevel = 0;

		// update style
		uStyle++;
		if ( uStyle > 4 ) uStyle = 1;

		// update background mode
		( XFONT_TRANSPARENT == nBkMode ) ? nBkMode = XFONT_OPAQUE : nBkMode = XFONT_TRANSPARENT;

		// update alignment
		if ( XFONT_CENTER == (uAlignment & XFONT_CENTER) )
		{
			uAlignment -= XFONT_CENTER;
		}
		else if ( uAlignment & XFONT_RIGHT )
		{
			uAlignment += XFONT_CENTER-XFONT_RIGHT;
		}
		else
		{
			uAlignment += XFONT_RIGHT-XFONT_LEFT;
		}
		if ( XFONT_BASELINE == (uAlignment & XFONT_BASELINE) )
		{
			uAlignment -= XFONT_BASELINE;
		}
		else if ( uAlignment & XFONT_BOTTOM )
		{
			uAlignment += XFONT_BASELINE-XFONT_BOTTOM;
		}
		else
		{
			uAlignment += XFONT_BOTTOM-XFONT_TOP;
		}

		// update rect
		Rect.x1 = rand()%640;
		Rect.x2 = rand()%640;
		Rect.y1 = rand()%480;
		Rect.y2 = rand()%480;

		//------------------------------------------------------------------------------
		//	apply changes
		//------------------------------------------------------------------------------

		pXFont->SetTextHeight( uHeight );
		pXFont->SetTextAntialiasLevel( uAALevel );
		pXFont->SetRLEWidth( uRLEWidth );
		pXFont->SetTextStyle( uStyle );
		pXFont->SetTextAlignment( uAlignment );
		pXFont->SetBkMode( nBkMode );
		pXFont->SetIntercharacterSpacing( uSpacing );

		pXFont->SetClippingRectangle( &Rect );
		pXFont->SetTextColor( D3DCOLOR_XRGB( rand()%256, rand()%256, rand()%256 ) );
		pXFont->SetBkColor( D3DCOLOR_XRGB( rand()%256, rand()%256, rand()%256 ) );
	}

	uHeight		= 16;
	uAALevel	= 0;
	uSpacing	= 0;
	uRLEWidth	= 2;
	uStyle		= XFONT_NORMAL;
	uAlignment	= XFONT_CENTER|XFONT_BASELINE;
	nBkMode		= XFONT_TRANSPARENT;
	Rect.x1 = 0;
	Rect.x2 = 640;
	Rect.y1 = 0;
	Rect.y2 = 480;
	pXFont->SetTextHeight( uHeight );
	pXFont->SetTextAntialiasLevel( uAALevel );
	pXFont->SetRLEWidth( uRLEWidth );
	pXFont->SetTextStyle( uStyle );
	pXFont->SetTextAlignment( uAlignment );
	pXFont->SetBkMode( nBkMode );
	pXFont->SetIntercharacterSpacing( uSpacing );
	pXFont->SetClippingRectangle( &Rect );

	// compare memory
	Sleep( 100 );
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[XFAPI] warning: Possible leak in XFONT_SetXXX.  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, i );
	}
	else
	{
		DbgPrint( "[XFAPI] info: XFONT_SetXXX memory is ok (%d runs).\n", i );
	}

	dwAvailPhys  = MemStatus.dwAvailPhys;

	for ( i=0; i < XFAPI_NUM_TESTS; i++ )
	{

		//------------------------------------------------------------------------------
		//	call get functions
		//------------------------------------------------------------------------------

		UINT	uCellHeight;
		UINT	uDescent;
		UINT	uWidth;
		UINT	uLength = -1; // -1 for zero terminated string

		pXFont->GetFontMetrics( &uCellHeight, &uDescent );
		hr = pXFont->GetTextExtent( XFAPI_TEST_STRING, uLength, &uWidth );
		if ( FAILED( hr ) )
		{
			DbgPrint( "[XFAPI]: ***Warning: GetTextExtent returned %x***\n", hr );
		}
	}

	// compare memory
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[XFAPI] warning: Possible leak in XFONT_GetXXX.  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, i );
	}
	else
	{
		DbgPrint( "[XFAPI] info: XFONT_GetXXX memory is ok (%d runs).\n", i );
	}

	dwAvailPhys  = MemStatus.dwAvailPhys;

	for ( i=0; i < XFAPI_NUM_TESTS; i++ )
	{

		//------------------------------------------------------------------------------
		//	output text
		//------------------------------------------------------------------------------

		LPDIRECT3DSURFACE8	pSurface = NULL;
		D3DSURFACE_DESC		SurfaceDesc;
		D3DLOCKED_RECT		LockedRect;
		RECT				LockRect = { 0, 0, 640, 480 };
		UINT				uLength = -1;

		hr = pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pSurface );
		if ( SUCCEEDED( hr ) )
		{
			hr = pSurface->GetDesc( &SurfaceDesc );
		}

		if ( SUCCEEDED( hr ) )
		{
			hr = pSurface->LockRect( &LockedRect, &LockRect, D3DLOCK_TILED );
		}
		if ( SUCCEEDED( hr ) )
		{
			hr = pXFont->TextOutToMemory( LockedRect.pBits, LockedRect.Pitch, SurfaceDesc.Width, SurfaceDesc.Height, SurfaceDesc.Format, XFAPI_TEST_STRING, uLength, 320, 240 );
		}
		if ( FAILED( hr ) )
		{
			DbgPrint( "[XFAPI]: ***Warning: TextOutToMemory returned %x***\n", hr );
		}
		

		hr = pXFont->TextOut( pSurface, XFAPI_TEST_STRING, uLength, 320, 240 );
		if ( FAILED( hr ) )
		{
			DbgPrint( "[XFAPI]: ***Warning: TextOut returned %x***\n", hr );
		}

		RELEASE( pSurface );
	}

	// compare memory
	GlobalMemoryStatus( &MemStatus );
	if ( dwAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[XFAPI] warning: Possible leak in XFONT_TextOutXXX.  Available memory shrunk by %d bytes (%d runs).\n", dwAvailPhys-MemStatus.dwAvailPhys, i );
	}
	else
	{
		DbgPrint( "[XFAPI] info: XFONT_TextOutXXX memory is ok (%d runs).\n", i );
	}

	RELEASE( pXFont );
	RELEASE( pd3dDevice );

	// compare total memory
	Sleep( 5000 );
	GlobalMemoryStatus( &MemStatus );
	if ( dwInitAvailPhys > MemStatus.dwAvailPhys )
	{
		DbgPrint( "[XFAPI] warning: Possible leak in XFONT.  Available memory shrunk by %d bytes.\n", dwInitAvailPhys-MemStatus.dwAvailPhys );
	}
	else
	{
		DbgPrint( "[XFAPI] info: XFONT memory is ok.\n" );
	}

	DbgPrint( "[XFAPI] info: initial available memory = %d\n\t\tending available memory = %d\n", dwInitAvailPhys, MemStatus.dwAvailPhys );
}