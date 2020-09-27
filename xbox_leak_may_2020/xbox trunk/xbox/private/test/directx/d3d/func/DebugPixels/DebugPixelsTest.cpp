/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	DebugPixelsTest.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 10-Dec-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	10-Dec-2001 jeffsul
		Initial Version

--*/

#include "DebugPixelsTest.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xgmath.h>
#include <medialoader.h>

#include <XbMediaDebug.h>

#define NUM_BACK_BUFFERS 2

CDbgPixelsTest::CDbgPixelsTest()
{
	m_pD3DDevice		= NULL;
	m_pTexture			= NULL;
	m_pvTextureOrSurface = NULL;

	m_Format			= D3DFMT_X8R8G8B8;
	ZeroMemory( &m_PrevInputState, sizeof(XINPUT_STATE) );
}


CDbgPixelsTest::~CDbgPixelsTest()
{
}


HRESULT 
CDbgPixelsTest::ReadIni()
{
	HRESULT			hr				= S_OK;

	// grab media files from server

	DWORD			dwCreateFlag	= COPY_IF_NEWER;
	MLSetErrorLevel( 3 );
	MEDIAFILEDESC MediaFiles[] =
	{
		{"Extensions/bmp/Cats.bmp",	"T:\\Textures\\",			dwCreateFlag},
		{NULL,				NULL,						0}
	};
	EXECUTE( LoadMedia( MediaFiles ) );
	if ( FAILED( hr ) )
	{
		return hr;
	}


	return hr;
}


HRESULT
CDbgPixelsTest::InitGraphics()
{
	HRESULT			hr				= S_OK;

	D3DPRESENT_PARAMETERS	d3dpp;

	// Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = NUM_BACK_BUFFERS;
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_PresentationInterval = /*D3DPRESENT_INTERVAL_ONE*/D3DPRESENT_INTERVAL_IMMEDIATE;

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

	m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0L );

	return hr;
}


HRESULT
CDbgPixelsTest::DestroyGraphics()
{
	HRESULT			hr				= S_OK;

	RELEASE( m_pD3DDevice );

	return hr;
}


HRESULT
CDbgPixelsTest::InitResources()
{
	HRESULT			hr				= S_OK;
	HANDLE			hFiles			= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	FindData;

	hFiles = FindFirstFile( "T:\\Textures\\*.bmp", &FindData );
	if ( INVALID_HANDLE_VALUE == hFiles )
	{
		DbgPrint( "[] error: Unable to find file that matches T:\\Textures\\*.bmp (error: %x)\n", GetLastError() );
		return E_FAIL;
	}

	CHAR	szFileName[MAX_PATH];
	strcpy( szFileName, "T:\\Textures\\" );
	strcat( szFileName, FindData.cFileName );

	hr = D3DXCreateTextureFromFileA( m_pD3DDevice, szFileName, &m_pTexture );
	if ( FAILED( hr ) )
	{
		DbgPrint( "[] error: Unable to CreateTexture (error: %x)\n", hr );
	}

	m_pvTextureOrSurface = (INT)m_pTexture;

	return hr;
}


HRESULT
CDbgPixelsTest::DestroyResources()
{
	HRESULT			hr				= S_OK;

	RELEASE( m_pTexture );

	return hr;
}


HRESULT
CDbgPixelsTest::ProcessInput()
{
	HRESULT			hr				= S_OK;

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
		return S_OK;
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

	// cycle texture
	if ( xinpstate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 15  && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] <= 15 )
	{
		if ( 0 < m_pvTextureOrSurface && NUM_BACK_BUFFERS > m_pvTextureOrSurface )
		{
			m_pvTextureOrSurface++;
		} 
		else if ( NUM_BACK_BUFFERS == m_pvTextureOrSurface )
		{
			m_pvTextureOrSurface = -1;
		}
		else if ( -1 == m_pvTextureOrSurface )
		{
			m_pvTextureOrSurface = (INT)m_pTexture;
		}
		else
		{
			m_pvTextureOrSurface = 1;
		}
	}

	// copy the current state to the previous state structure for use next time around
	memcpy( &m_PrevInputState, &xinpstate, sizeof(XINPUT_STATE) );

	return hr;
}


HRESULT
CDbgPixelsTest::Update()
{
	HRESULT			hr				= S_OK;

	return hr;
}


HRESULT
CDbgPixelsTest::Render()
{
	HRESULT			hr				= S_OK;

	hr = DebugPixels( (LPVOID)m_pvTextureOrSurface, 0 );

	m_pD3DDevice->Swap( D3DSWAP_DEFAULT );

	return hr;
}