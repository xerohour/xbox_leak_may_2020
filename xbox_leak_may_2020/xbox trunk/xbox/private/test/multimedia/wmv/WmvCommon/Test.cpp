/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Test.cpp

Abstract:

	Abstract class implementation to inherit from for individual tests

Author:

	Jeff Sullivan (jeffsul) 14-Jun-2001

Revision History:

	14-Jun-2001 jeffsul
		Initial Version

--*/

#include "Test.h"

#include <xdbg.h>
#include <xlog.h>
#include <macros.h>

#include <stdio.h>

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;


//------------------------------------------------------------------------------
//	Inititialize static variable to FALSE so that XInitDevices() is called
//------------------------------------------------------------------------------

BOOL CTest::m_bInitDevicesCalled = FALSE;


/*++

	CTest constructor

Routine Description:

	constructor

Arguments:

	None

Return Value:

	None

--*/

CTest::CTest()
{
	m_bQuit = FALSE;
}


/*++

	CTest destructor

Routine Description:

	destructor

Arguments:

	None

Return Value:

	None

--*/

CTest::~CTest()
{
}


/*++

	Run

Routine Description:

	The test's main loop: initialize, enter loop, destroy

Arguments:

	None

Return Value:

	None

--*/

HRESULT CTest::Run()
{
	HRESULT hr = S_OK;

	// initialize all resources/devices
	EXECUTE( Init() );
	if ( FAILED( hr ) )
	{
		Destroy();
		return hr;
	}
	
	// loop for as long as test needs to run
	while ( !m_bQuit )
	{
		// calculate time change since last frame and update time variables
		DWORD dwTempTime = timeGetTime();
		m_dwDeltaTime = dwTempTime - m_dwCurrentTime;
		m_dwCurrentTime = dwTempTime;
		
		// do the real work of the test inside these functions
		EXECUTE( ProcessInput() )
		EXECUTE( Update() )
		EXECUTE( Render() )
	}
	
	// clean up 
	EXECUTE( Destroy() )

	return hr;
}


/*++

	Init

Routine Description:

	Common initialization function for all tests, calls inheritable functions to do
	initialization and starts timers

Arguments:

	None

Return Value:

	None

--*/

HRESULT CTest::Init()
{
	HRESULT hr = S_OK;

	EXECUTE( ReadIni() )
	if ( FAILED( hr ) )
	{
		return hr;
	}

	EXECUTE( InitGraphics() )
	if ( FAILED ( hr ) )
	{
		return hr;
	}

	EXECUTE( InitAudio() )
	if ( FAILED ( hr ) )
	{
		DestroyGraphics();
		return hr;
	}

	if ( !m_bInitDevicesCalled )
	{
		// we only need to call this once for all tests, hence static declaration of m_bInitDevicesCalled
		XInitDevices( 0, NULL );
		m_bInitDevicesCalled = TRUE;
	}

	EXECUTE( InitInput() )
	if ( FAILED ( hr ) )
	{
		DestroyGraphics();
		DestroyAudio();
		return hr;
	}

	EXECUTE( InitResources() )
	if ( FAILED ( hr ) )
	{
		DestroyGraphics();
		DestroyAudio();
		DestroyInput();
		return hr;
	}

	m_dwStartTime = m_dwCurrentTime = timeGetTime(); 

	return hr;
}


/*++

	Destroy

Routine Description:

	Common de-allocation function for all tests, calls inheritable functions to do
	un-initialization/deletion

Arguments:

	None

Return Value:

	None

--*/

HRESULT CTest::Destroy()
{
	HRESULT hr = S_OK;

	EXECUTE( DestroyGraphics() )
	EXECUTE( DestroyAudio() )
	EXECUTE( DestroyInput() )
	EXECUTE( DestroyResources() )

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
HRESULT CTest::InitD3DDevice( 
	LPDIRECT3DDEVICE8* ppDevice 
)
{
	D3DPRESENT_PARAMETERS	d3dpp;

	// Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = NULL;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	return Direct3D_CreateDevice(	D3DADAPTER_DEFAULT, 
									D3DDEVTYPE_HAL,
									NULL, 
									D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									&d3dpp, 
									ppDevice);
}


/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
HRESULT	CTest::GetNextInputState( 
	HANDLE* phInputDevice, 
	XINPUT_STATE* pInputState 
)
{
	HRESULT hr = S_OK;

	DWORD dwDeviceMap;
	DWORD dwSuccess;
	BOOL bDeviceChanges;
	DWORD dwInsertions;
	DWORD dwRemovals;

	bDeviceChanges = XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );
	if ( bDeviceChanges )
	{
		if ( dwInsertions & XDEVICE_PORT0_MASK )
		{
			*phInputDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL );
			if ( *phInputDevice == NULL )
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
	else if ( NULL == *phInputDevice )
	{
		*phInputDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL );
		if ( *phInputDevice == NULL )
		{
			DWORD dwError;
			dwError = GetLastError();
			DbgPrint( "Error Opening Input. Error#: %d\n", dwError);
			return E_FAIL;
		}
	}

	dwSuccess = XInputGetState( *phInputDevice, pInputState );
	if ( dwSuccess != ERROR_SUCCESS )
	{
		DbgPrint( "Error Getting Input State\n" );
		return dwSuccess;
	}

	return hr;
}
